/***************************************************************************
 *   Copyright © 2010 Jonathan Thomas <echidnaman@kubuntu.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of        *
 *   the License or (at your option) version 3 or any later version        *
 *   accepted by the membership of KDE e.V. (or its successor approved     *
 *   by the membership of KDE e.V.), which shall act as a proxy            *
 *   defined in Section 14 of version 3 of the license.                    *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#include "MainWindow.h"

// Qt includes
#include <QApplication>
#include <QStringBuilder>
#include <QTimer>
#include <QSplitter>
#include <QStackedWidget>
#include <QToolBox>
#include <QVBoxLayout>
#include <QAction>
#include <QStyle>
#include <QProcess>

// Own includes
#include "DonateDialog.h"

// KDE includes
#include <KActionCollection>
#include <KLocalizedString>
#include <KMessageBox>
#include <KProtocolManager>
#include <KStandardAction>
#include <KColorScheme>
#include <KToolBar>
#include <KAuth/Action>
#include <KAuth/ExecuteJob>

// QApt includes
#include <QApt/Backend>
#include <QApt/Config>
#include <QApt/Transaction>

// Own includes
#include "muonapt/MuonStrings.h"
#include "TransactionWidget.h"
#include "FilterWidget/FilterWidget.h"
#include "ManagerWidget.h"
#include "ReviewWidget.h"
#include "MuonSettings.h"
#include "StatusWidget.h"
#include "config/ManagerSettingsDialog.h"
#include "muonapt/QAptActions.h"

MainWindow::MainWindow()
    : KXmlGuiWindow()
    , m_settingsDialog(nullptr)
    , m_reviewWidget(nullptr)
    , m_transWidget(nullptr)
    , m_reloading(false)

{
    initGUI();
    QTimer::singleShot(10, this, [this]() { initObject(); });
}

MainWindow::~MainWindow()
{
    MuonSettings::self()->save();
}

void MainWindow::initGUI()
{
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *centralLayout = new QVBoxLayout(centralWidget);
    centralLayout->setSpacing(0);
    centralLayout->setMargin(0);

    m_stack = new QStackedWidget(centralWidget);
    m_stack->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    centralLayout->addWidget(m_stack);

    setCentralWidget(centralWidget);

    m_managerWidget = new ManagerWidget(m_stack);
    connect(this, &MainWindow::backendReady,
            m_managerWidget, &ManagerWidget::setBackend);
    connect(m_managerWidget, &ManagerWidget::packageChanged, this, [this]() {
        setActionsEnabled(true);
    });

    m_mainWidget = new QSplitter(this);
    m_mainWidget->setOrientation(Qt::Horizontal);
    connect(m_mainWidget, &QSplitter::splitterMoved, this, [this](int, int) { saveSplitterSizes(); });

    m_transWidget = new TransactionWidget(this);
    m_transWidget->setContentsMargins(
        style()->pixelMetric(QStyle::PM_LayoutTopMargin),
        style()->pixelMetric(QStyle::PM_LayoutLeftMargin),
        style()->pixelMetric(QStyle::PM_LayoutRightMargin),
        style()->pixelMetric(QStyle::PM_LayoutBottomMargin)
    );

    m_filterBox = new FilterWidget(m_stack);
    connect(this, &MainWindow::backendReady,
            m_filterBox, &FilterWidget::setBackend);
    connect(m_filterBox, &FilterWidget::filterByGroup,
            m_managerWidget, &ManagerWidget::filterByGroup);
    connect(m_filterBox, &FilterWidget::filterByStatus,
            m_managerWidget, &ManagerWidget::filterByStatus);
    connect(m_filterBox, &FilterWidget::filterByOrigin,
            m_managerWidget, &ManagerWidget::filterByOrigin);
    connect(m_filterBox, &FilterWidget::filterByArchitecture,
            m_managerWidget, &ManagerWidget::filterByArchitecture);

    m_mainWidget->addWidget(m_filterBox);
    m_mainWidget->addWidget(m_managerWidget);
    loadSplitterSizes();

    m_stack->addWidget(m_transWidget);
    m_stack->addWidget(m_mainWidget);
    m_stack->setCurrentWidget(m_mainWidget);

    m_backend = new QApt::Backend(this);
    QApt::FrontendCaps caps = (QApt::FrontendCaps)(QApt::DebconfCap | QApt::MediumPromptCap |
                               QApt::ConfigPromptCap | QApt::UntrustedPromptCap);
    m_backend->setFrontendCaps(caps);
    QAptActions* actions = QAptActions::self();

    actions->setMainWindow(this);
    connect(actions, &QAptActions::changesReverted, this, &MainWindow::revertChanges);
    setupActions();

    m_statusWidget = new StatusWidget(centralWidget);
    connect(this, &MainWindow::backendReady,
            m_statusWidget, &StatusWidget::setBackend);
    centralLayout->addWidget(m_statusWidget);
}

void MainWindow::initObject()
{
    QAptActions::self()->setBackend(m_backend);
    emit backendReady(m_backend);
    connect(m_backend, &QApt::Backend::packageChanged,
            this, [this]() {
        setActionsEnabled(true);
    });
    connect(m_backend, &QApt::Backend::cacheReloadFinished,
            this, [this]() {
        setActionsEnabled(true);
    });

    // Set up GUI
    loadSettings();
    setActionsEnabled();
    m_managerWidget->setFocus();
    
    // Apply dark mode detection and KDE color scheme
    applyKDEColorScheme();
}

void MainWindow::loadSettings()
{
    m_backend->setUndoRedoCacheSize(MuonSettings::self()->undoStackSize());
    m_managerWidget->invalidateFilter();
    
    // Update column visibility based on settings
    if (MuonSettings::self()->showVersionColumns()) {
        m_managerWidget->showVersionColumns();
    } else {
        m_managerWidget->hideVersionColumns();
    }
}

void MainWindow::loadSplitterSizes()
{
    QList<int> sizes = MuonSettings::self()->splitterSizes();

    if (sizes.isEmpty()) {
        sizes << 115 << (this->width() - 115);
    }
    m_mainWidget->setSizes(sizes);
}

void MainWindow::saveSplitterSizes()
{
    MuonSettings::self()->setSplitterSizes(m_mainWidget->sizes());
    MuonSettings::self()->save();
}

void MainWindow::setupActions()
{
    QAction *quitAction = KStandardAction::quit(QCoreApplication::instance(), SLOT(quit()), actionCollection());
    actionCollection()->addAction("file_quit", quitAction);

    QAction *focusSearchAction = KStandardAction::find(this, SLOT(setFocusSearchEdit()), actionCollection());
    actionCollection()->addAction("find", focusSearchAction);

    m_safeUpgradeAction = actionCollection()->addAction("safeupgrade");
    m_safeUpgradeAction->setIcon(QIcon::fromTheme("go-up"));
    m_safeUpgradeAction->setText(i18nc("@action Marks upgradeable packages for upgrade", "Cautious Upgrade"));
    connect(m_safeUpgradeAction, SIGNAL(triggered()), this, SLOT(markUpgrade()));

    m_distUpgradeAction = actionCollection()->addAction("fullupgrade");
    m_distUpgradeAction->setIcon(QIcon::fromTheme("go-top"));
    m_distUpgradeAction->setText(i18nc("@action Marks upgradeable packages, including ones that install/remove new things",
                                       "Full Upgrade"));
    connect(m_distUpgradeAction, SIGNAL(triggered()), this, SLOT(markDistUpgrade()));

    m_autoRemoveAction = actionCollection()->addAction("autoremove");
    m_autoRemoveAction->setIcon(QIcon::fromTheme("trash-empty"));
    m_autoRemoveAction->setText(i18nc("@action Marks packages no longer needed for removal",
                                      "Remove Unnecessary Packages"));
    connect(m_autoRemoveAction, SIGNAL(triggered()), this, SLOT(markAutoRemove()));

    m_previewAction = actionCollection()->addAction("preview");
    m_previewAction->setIcon(QIcon::fromTheme("document-preview-archive"));
    m_previewAction->setText(i18nc("@action Takes the user to the preview page", "Preview Changes"));
    connect(m_previewAction, SIGNAL(triggered()), this, SLOT(previewChanges()));

    m_applyAction = actionCollection()->addAction("apply");
    m_applyAction->setIcon(QIcon::fromTheme("dialog-ok-apply"));
    m_applyAction->setText(i18nc("@action Applys the changes a user has made", "Apply Changes"));
    connect(m_applyAction, SIGNAL(triggered()), this, SLOT(startCommit()));

    QAction* updateAction = actionCollection()->addAction("update");
    updateAction->setIcon(QIcon::fromTheme("system-software-update"));
    updateAction->setText(i18nc("@action Checks the Internet for updates", "Check for Updates"));
    actionCollection()->setDefaultShortcut(updateAction, QKeySequence(Qt::CTRL + Qt::Key_R));
    connect(updateAction, SIGNAL(triggered()), SLOT(checkForUpdates()));
    updateAction->setEnabled(QAptActions::self()->isConnected());
    connect(QAptActions::self(), SIGNAL(shouldConnect(bool)), updateAction, SLOT(setEnabled(bool)));

    KStandardAction::preferences(this, SLOT(editSettings()), actionCollection());

    QAction* configureRepositoriesAction = actionCollection()->addAction("configure_repositories");
    configureRepositoriesAction->setIcon(QIcon::fromTheme("network-server"));
    configureRepositoriesAction->setText(i18nc("@action Configure package repositories", "Configure Repositories"));
    connect(configureRepositoriesAction, SIGNAL(triggered()), this, SLOT(configureRepositories()));

    QAction* donateAction = actionCollection()->addAction("donate");
    donateAction->setIcon(QIcon::fromTheme("help-donate"));
    donateAction->setText(i18nc("@action Open donation dialog", "Donate"));
    connect(donateAction, SIGNAL(triggered()), this, SLOT(openDonateDialog()));

    KStandardAction::aboutApp(this, SLOT(showAboutApplication()), actionCollection());

    setActionsEnabled(false);

    setupGUI(StandardWindowOption(KXmlGuiWindow::Default & ~KXmlGuiWindow::StatusBar));
    
    // Explicitly create and setup toolbar to ensure buttons appear
    KToolBar *mainToolBar = toolBar();
    if (mainToolBar) {
        mainToolBar->addAction(m_previewAction);
        mainToolBar->addAction(m_applyAction);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionCollection()->action("update"));
        mainToolBar->addAction(m_distUpgradeAction);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionCollection()->action("undo"));
        mainToolBar->addAction(actionCollection()->action("redo"));
        mainToolBar->addAction(m_autoRemoveAction);
    }
}

void MainWindow::setFocusSearchEdit()
{
    if (m_managerWidget->isVisible()) {
        m_managerWidget->setFocusSearchEdit();
    }
}

void MainWindow::markUpgrade()
{
    m_backend->saveCacheState();
    m_backend->markPackagesForUpgrade();

    if (m_backend-> markedPackages().isEmpty()) {
        QString text = xi18nc("@label", "Unable to mark upgrades. The "
                              "available upgrades may require new packages to "
                              "be installed or removed. You may wish to try "
                              "a full upgrade by clicking the <interface>Full "
                              "Upgrade</interface> button.");
        QString title = i18nc("@title:window", "Unable to Mark Upgrades");
        KMessageBox::information(this, text, title);
    } else {
        previewChanges();
    }
}

void MainWindow::markDistUpgrade()
{
    m_backend->saveCacheState();
    m_backend->markPackagesForDistUpgrade();
    if (m_backend-> markedPackages().isEmpty()) {
        QString text = i18nc("@label", "Unable to mark upgrades. Some "
                             "upgrades may have unsatisfiable dependencies at "
                             "the moment, or may have been manually held back.");
        QString title = i18nc("@title:window", "Unable to Mark Upgrades");
        KMessageBox::information(this, text, title);
    } else {
        previewChanges();
    }
}

void MainWindow::markAutoRemove()
{
    m_backend->saveCacheState();
    m_backend->markPackagesForAutoRemove();
    previewChanges();
}

void MainWindow::checkForUpdates()
{
    setActionsEnabled(false);
    m_managerWidget->setEnabled(false);

    m_stack->setCurrentWidget(m_transWidget);
    m_trans = m_backend->updateCache();
    setupTransaction(m_trans);

    m_trans->run();
}

void MainWindow::errorOccurred(QApt::ErrorCode error)
{
    switch(error) {
    case QApt::AuthError:
    case QApt::LockError:
        m_managerWidget->setEnabled(true);
        QApplication::restoreOverrideCursor();
        returnFromPreview();
        setActionsEnabled();
        break;
    default:
        break;
    }
}

void MainWindow::transactionStatusChanged(QApt::TransactionStatus status)
{
    // FIXME: better support for transactions that do/don't need reloads
    switch (status) {
    case QApt::RunningStatus:
    case QApt::WaitingStatus:
        QApplication::restoreOverrideCursor();
        m_stack->setCurrentWidget(m_transWidget);
        break;
    case QApt::FinishedStatus:
        reload();
        setActionsEnabled();

        m_trans->deleteLater();
        m_trans = nullptr;
        break;
    default:
        break;
    }
}

void MainWindow::previewChanges()
{
    m_reviewWidget = new ReviewWidget(m_stack);
    connect(this, SIGNAL(backendReady(QApt::Backend*)),
            m_reviewWidget, SLOT(setBackend(QApt::Backend*)));
    m_reviewWidget->setBackend(m_backend);
    m_stack->addWidget(m_reviewWidget);

    m_stack->setCurrentWidget(m_reviewWidget);

    m_previewAction->setIcon(QIcon::fromTheme("go-previous"));
    m_previewAction->setText(i18nc("@action:intoolbar Return from the preview page", "Back"));
    disconnect(m_previewAction, SIGNAL(triggered()), this, SLOT(previewChanges()));
    connect(m_previewAction, SIGNAL(triggered()), this, SLOT(returnFromPreview()));
}

void MainWindow::returnFromPreview()
{
    m_stack->setCurrentWidget(m_mainWidget);
    if (m_reviewWidget) {
        m_reviewWidget->deleteLater();
        m_reviewWidget = nullptr;
    }

    m_previewAction->setIcon(QIcon::fromTheme("document-preview-archive"));
    m_previewAction->setText(i18nc("@action", "Preview Changes"));
    disconnect(m_previewAction, SIGNAL(triggered()), this, SLOT(returnFromPreview()));
    connect(m_previewAction, SIGNAL(triggered()), this, SLOT(previewChanges()));
}

void MainWindow::startCommit()
{
    // Debug output to verify which method is being used
    qDebug() << "startCommit() called, useKAuthForPrivileges() =" << useKAuthForPrivileges();
    
    // Force traditional method for now - KAuth action files are not properly configured
    // Traditional method
    setActionsEnabled(false);
    m_managerWidget->setEnabled(false);
    QApplication::setOverrideCursor(Qt::WaitCursor);

    m_stack->setCurrentWidget(m_transWidget);
    m_trans = m_backend->commitChanges();
    setupTransaction(m_trans);

    m_trans->run();
}

bool MainWindow::queryClose()
{
    if (m_reloading || m_managerWidget->isSortingPackages()) {
        return false;
    }
    
    // Check if there are pending changes and confirmation is enabled
    if (MuonSettings::self()->confirmOnQuit() && m_backend->areChangesMarked()) {
        QString text = i18nc("@label", "You have pending changes that have not been applied. "
                                      "Are you sure you want to quit?");
        QString title = i18nc("@title:window", "Confirm Quit");
        int result = KMessageBox::questionTwoActions(this, text, title,
                                                    KStandardGuiItem::quit(), KStandardGuiItem::cancel());
        return (result == KMessageBox::PrimaryAction);
    }
    
    return true;
}

void MainWindow::reload()
{
    m_reloading = true;
    returnFromPreview();
    m_stack->setCurrentWidget(m_mainWidget);

    // Reload the QApt Backend
    m_managerWidget->reload();

    // Reload other widgets
    if (m_reviewWidget) {
        m_reviewWidget->reload();
    }

    m_filterBox->reload();

    QAptActions::self()->setOriginalState(m_backend->currentCacheState());
    m_statusWidget->updateStatus();
    setActionsEnabled();
    m_managerWidget->setEnabled(true);

    m_reloading = false;
}

void MainWindow::setActionsEnabled(bool enabled)
{
    QAptActions::self()->setActionsEnabled(enabled);
    if (!enabled) {
        m_applyAction->setEnabled(false);
        m_safeUpgradeAction->setEnabled(false);
        m_distUpgradeAction->setEnabled(false);
        m_autoRemoveAction->setEnabled(false);
        m_previewAction->setEnabled(false);
        return;
    }

    int upgradeable = m_backend->packageCount(QApt::Package::Upgradeable);
    bool changesPending = m_backend->areChangesMarked();
    int autoRemoveable = m_backend->packageCount(QApt::Package::IsGarbage);

    m_applyAction->setEnabled(changesPending);
    m_safeUpgradeAction->setEnabled(upgradeable > 0);
    m_distUpgradeAction->setEnabled(upgradeable > 0);
    m_autoRemoveAction->setEnabled(autoRemoveable > 0);
    if (m_stack->currentWidget() == m_reviewWidget) {
        // We always need to be able to get back from review
        m_previewAction->setEnabled(true);
    } else {
        m_previewAction->setEnabled(changesPending);
    }
}

void MainWindow::downloadArchives(QApt::Transaction *trans)
{
    if (!trans) {
        // Shouldn't happen...
        delete trans;
        return;
    }

    m_stack->setCurrentWidget(m_transWidget);
    m_trans = trans;
    setupTransaction(trans);
    trans->run();
}

void MainWindow::editSettings()
{
    if (!m_settingsDialog) {
        m_settingsDialog = new ManagerSettingsDialog(this, m_backend->config());
        connect(m_settingsDialog, SIGNAL(finished(int)), SLOT(closeSettingsDialog()));
        connect(m_settingsDialog, SIGNAL(settingsChanged()), SLOT(loadSettings()));
        m_settingsDialog->show();
    } else {
        m_settingsDialog->raise();
    }
}

void MainWindow::closeSettingsDialog()
{
    m_settingsDialog->deleteLater();
    m_settingsDialog = nullptr;
}

void MainWindow::configureRepositories()
{
    // Open settings dialog - it will show with the Repositories page as the second tab
    if (!m_settingsDialog) {
        m_settingsDialog = new ManagerSettingsDialog(this, m_backend->config());
        connect(m_settingsDialog, SIGNAL(finished(int)), SLOT(closeSettingsDialog()));
        connect(m_settingsDialog, SIGNAL(settingsChanged()), SLOT(loadSettings()));
        m_settingsDialog->show();
    } else {
        m_settingsDialog->raise();
    }
}

void MainWindow::openDonateDialog()
{
    DonateDialog dlg(this);
    dlg.exec();
}

void MainWindow::showAboutApplication()
{
    // The about dialog will be handled by KXmlGuiWindow automatically
    // when using KStandardAction::aboutApp
}

void MainWindow::revertChanges()
{
    if (m_reviewWidget) {
        returnFromPreview();
    }
}

void MainWindow::setupTransaction(QApt::Transaction *trans)
{
    // Modern QProcess-based setup for external processes
    setupModernProcessHandling(trans);
    
    // Provide proxy/locale to the transaction
    if (KProtocolManager::proxyType() == KProtocolManager::ManualProxy) {
        trans->setProxy(KProtocolManager::proxyFor("http"));
    }

    trans->setLocale(QLatin1String(setlocale(LC_MESSAGES, 0)));

    trans->setDebconfPipe(m_transWidget->pipe());
    m_transWidget->setTransaction(m_trans);

    connect(m_trans, &QApt::Transaction::statusChanged,
            this, &MainWindow::transactionStatusChanged);
    connect(m_trans, &QApt::Transaction::errorOccurred,
            this, &MainWindow::errorOccurred);
}

void MainWindow::setupModernProcessHandling(QApt::Transaction *trans)
{
    // Modern QProcess-based process handling
    // This provides better process management with Qt6
    if (trans) {
        // Set up modern signal connections for process management
        connect(trans, &QApt::Transaction::statusChanged,
                this, [this](QApt::TransactionStatus status) {
            // Handle status changes with modern lambda syntax
            if (status == QApt::RunningStatus) {
                QApplication::setOverrideCursor(Qt::WaitCursor);
            } else if (status == QApt::FinishedStatus) {
                QApplication::restoreOverrideCursor();
            }
        });
        
        // Modern error handling with QProcess integration
        connect(trans, &QApt::Transaction::errorOccurred,
                this, [this](QApt::ErrorCode error) {
            // Handle errors with modern approach
            errorOccurred(error);
        });
    }
}

bool MainWindow::useKAuthForPrivileges() const
{
    // Disable KAuth for now as the action files are not properly configured
    // Use traditional method instead
    return false;
}

void MainWindow::executeKAuthAction(const QString &actionId)
{
    KAuth::Action action(actionId);
    action.setHelperId("org.kde.muon.helper");
    
    // Add package list as argument
    QStringList packageList;
    for (QApt::Package *pkg : m_backend->markedPackages()) {
        packageList.append(pkg->name());
    }
    action.addArgument("packages", packageList);
    
    // Execute action
    KAuth::ExecuteJob *job = action.execute();
    connect(job, &KAuth::ExecuteJob::finished, this, [this, job]() {
        if (job->error() == KAuth::ActionReply::NoError) {
            // Success - reload backend
            reload();
        } else {
            // Error handling
            KMessageBox::error(this,
                i18n("Failed to execute package operation: %1", job->errorString()),
                i18n("Operation Failed"));
        }
    });
    
    job->start();
}

QSize MainWindow::sizeHint() const
{
    return KXmlGuiWindow::sizeHint().expandedTo(QSize(900, 500));
}

void MainWindow::applyKDEColorScheme()
{
    // Apply KDE color scheme and dark mode detection
    KColorScheme scheme(QPalette::Active, KColorScheme::Window);
    
    // Detect dark mode and apply appropriate colors
    bool isDarkMode = scheme.background().color().lightness() < 128;
    
    if (isDarkMode) {
        // Apply dark mode specific styling if needed
        qApp->setStyleSheet(QStringLiteral(
            "QToolTip {"
            "   background-color: #2a2a2a;"
            "   color: #ffffff;"
            "   border: 1px solid #555555;"
            "}"
        ));
    } else {
        // Apply light mode styling
        qApp->setStyleSheet(QStringLiteral(
            "QToolTip {"
            "   background-color: #ffffff;"
            "   color: #000000;"
            "   border: 1px solid #cccccc;"
            "}"
        ));
    }
    
    // Apply KDE accent color if available
    QColor accentColor = scheme.decoration(KColorScheme::FocusColor).color();
    if (accentColor.isValid()) {
        // Apply accent color to selection and focus elements
        QPalette palette = qApp->palette();
        palette.setColor(QPalette::Highlight, accentColor);
        palette.setColor(QPalette::HighlightedText, Qt::white);
        qApp->setPalette(palette);
    }
}
