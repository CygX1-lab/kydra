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
#include <QFileDialog>
#ifdef HAVE_KIRIGAMI
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#endif

// Own includes
#include "DonateDialog.h"
#include "KirigamiBackend.h"

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
#include <QApt/DebFile>

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
#include "PackageModel/LocalPackageManager.h"

MainWindow::MainWindow()
    : KXmlGuiWindow()
    , m_settingsDialog(nullptr)
    , m_reviewWidget(nullptr)
    , m_transWidget(nullptr)
    , m_reloading(false)
    , m_qmlEngine(nullptr)
    , m_kirigamiBackend(nullptr)
    , m_useKirigamiUI(false)
{
    // Check if Kirigami is available and user wants to use it
#ifdef HAVE_KIRIGAMI
    m_useKirigamiUI = MuonSettings::self()->useKirigamiUI();
#endif
    
    if (m_useKirigamiUI) {
        initKirigamiUI();
    } else {
        initTraditionalUI();
    }
}

MainWindow::~MainWindow()
{
    MuonSettings::self()->save();
    
#ifdef HAVE_KIRIGAMI
    if (m_qmlEngine) {
        delete m_qmlEngine;
    }
    if (m_kirigamiBackend) {
        delete m_kirigamiBackend;
    }
#endif
}

void MainWindow::initTraditionalUI()
{
    initGUI();
    QTimer::singleShot(10, this, [this]() { initObject(); });
}

void MainWindow::initKirigamiUI()
{
#ifdef HAVE_KIRIGAMI
    // Initialize QML engine
    m_qmlEngine = new QQmlApplicationEngine(this);
    
    // Create Kirigami backend bridge
    m_kirigamiBackend = new KirigamiBackend(this);
    
    // Set up backend first
    m_backend = new QApt::Backend(this);
    QApt::FrontendCaps caps = (QApt::FrontendCaps)(QApt::DebconfCap | QApt::MediumPromptCap |
                               QApt::ConfigPromptCap | QApt::UntrustedPromptCap);
    m_backend->setFrontendCaps(caps);
    m_kirigamiBackend->setBackend(m_backend);
    
    // Expose backend to QML
    m_qmlEngine->rootContext()->setContextProperty("kydraBackend", m_kirigamiBackend);
    m_qmlEngine->rootContext()->setContextProperty("qaBackend", m_backend);
    
    // Load main QML file
    m_qmlEngine->load(QUrl(QStringLiteral("qrc:/qml/KydraKirigami.qml")));
    
    // Connect backend signals
    connect(m_kirigamiBackend, &KirigamiBackend::showPreviewDialog,
            this, &MainWindow::previewChanges);
    connect(m_kirigamiBackend, &KirigamiBackend::showTransactionDialog,
            this, [this]() {
        // Handle transaction dialog
        if (m_trans) {
            m_stack->setCurrentWidget(m_transWidget);
        }
    });
    
    // Initialize backend
    QAptActions* actions = QAptActions::self();
    actions->setMainWindow(this);
    connect(actions, &QAptActions::changesReverted, this, &MainWindow::revertChanges);
    
    // Set up window properties
    if (!m_qmlEngine->rootObjects().isEmpty()) {
        QQuickWindow *window = qobject_cast<QQuickWindow*>(m_qmlEngine->rootObjects().first());
        if (window) {
            // Set window properties
            window->setTitle(i18n("Kydra Package Manager"));
            window->setMinimumSize(QSize(800, 600));
            
            // Connect window signals
            connect(window, &QQuickWindow::closing, this, [this](QQuickCloseEvent *event) {
                if (!queryClose()) {
                    event->setAccepted(false);
                }
            });
        }
    }
    
    // Emit backend ready signal
    QTimer::singleShot(10, this, [this]() {
        emit backendReady(m_backend);
        setActionsEnabled(true);
    });
#endif
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
    connect(m_managerWidget, &ManagerWidget::installLocalPackage,
            this, &MainWindow::installLocalPackageFile);

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
    
    // Initialize LocalPackageManager
    LocalPackageManager::setBackend(m_backend);
    LocalPackageManager *localManager = LocalPackageManager::instance();
    QString localDebFolder = MuonSettings::self()->localDebFolder();
    if (!localDebFolder.isEmpty()) {
        localManager->setLocalDebFolders(localDebFolder.split(';'));
    }
}

void MainWindow::initObject()
{
    QAptActions::self()->setBackend(m_backend);
    
    // Initialize local package manager with delay to avoid blocking UI
    QTimer::singleShot(1000, this, [this]() {
        LocalPackageManager::setBackend(m_backend);
        LocalPackageManager *localManager = LocalPackageManager::instance();
        if (localManager) {
            QStringList folders = MuonSettings::self()->localDebFolder().split(';');
            folders.removeAll(QString()); // Remove empty strings
            
            if (!folders.isEmpty()) {
                // Connect to both scan finished and local install detection finished
                connect(localManager, &LocalPackageManager::scanFinished, this, [this, localManager]() {
                    // After scan finishes, detect local install packages
                    localManager->detectLocalInstallPackages();
                });
                
                connect(localManager, &LocalPackageManager::localInstallPackagesDetected, this, [this]() {
                    // Reload filters after both scanning and local install detection completes
                    if (m_filterBox) {
                        m_filterBox->reload();
                    }
                });
                
                localManager->setLocalDebFolders(folders);
                localManager->scanLocalPackages();
            } else {
                // Even if no folders are configured, try to detect locally installed packages
                localManager->detectLocalInstallPackages();
                if (m_filterBox) {
                    m_filterBox->reload();
                }
            }
        } else {
            // If no local manager, just reload filters to ensure proper initialization
            if (m_filterBox) {
                m_filterBox->reload();
            }
        }
    });
    
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
    
    // Update LocalPackageManager settings
    LocalPackageManager *localManager = LocalPackageManager::instance();
    if (localManager) {
        QString localDebFolder = MuonSettings::self()->localDebFolder();
        if (!localDebFolder.isEmpty()) {
            localManager->setLocalDebFolders(localDebFolder.split(';'));
        } else {
            localManager->setLocalDebFolders(QStringList());
        }
    }
    
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
    m_safeUpgradeAction->setText(i18nc("@action", "Upgrade"));
    m_safeUpgradeAction->setToolTip(i18nc("@info:tooltip", "Mark upgradeable packages for cautious upgrade"));
    connect(m_safeUpgradeAction, SIGNAL(triggered()), this, SLOT(markUpgrade()));

    m_distUpgradeAction = actionCollection()->addAction("fullupgrade");
    m_distUpgradeAction->setIcon(QIcon::fromTheme("go-top"));
    m_distUpgradeAction->setText(i18nc("@action", "Full Upgrade"));
    m_distUpgradeAction->setToolTip(i18nc("@info:tooltip", "Mark all upgradeable packages, including those requiring new installations"));
    connect(m_distUpgradeAction, SIGNAL(triggered()), this, SLOT(markDistUpgrade()));

    m_autoRemoveAction = actionCollection()->addAction("autoremove");
    m_autoRemoveAction->setIcon(QIcon::fromTheme("trash-empty"));
    m_autoRemoveAction->setText(i18nc("@action", "Clean"));
    m_autoRemoveAction->setToolTip(i18nc("@info:tooltip", "Remove unnecessary packages"));
    connect(m_autoRemoveAction, SIGNAL(triggered()), this, SLOT(markAutoRemove()));

    m_previewAction = actionCollection()->addAction("preview");
    m_previewAction->setIcon(QIcon::fromTheme("document-preview-archive"));
    m_previewAction->setText(i18nc("@action", "Preview"));
    m_previewAction->setToolTip(i18nc("@info:tooltip", "Preview pending changes"));
    connect(m_previewAction, SIGNAL(triggered()), this, SLOT(previewChanges()));

    m_applyAction = actionCollection()->addAction("apply");
    m_applyAction->setIcon(QIcon::fromTheme("dialog-ok-apply"));
    m_applyAction->setText(i18nc("@action", "Apply"));
    m_applyAction->setToolTip(i18nc("@info:tooltip", "Apply pending changes"));
    connect(m_applyAction, SIGNAL(triggered()), this, SLOT(startCommit()));

    QAction* updateAction = actionCollection()->addAction("update");
    updateAction->setIcon(QIcon::fromTheme("system-software-update"));
    updateAction->setText(i18nc("@action", "Refresh"));
    updateAction->setToolTip(i18nc("@info:tooltip", "Check for package updates"));
    actionCollection()->setDefaultShortcut(updateAction, QKeySequence(Qt::ControlModifier | Qt::Key_R));
    connect(updateAction, SIGNAL(triggered()), SLOT(checkForUpdates()));
    updateAction->setEnabled(QAptActions::self()->isConnected());
    connect(QAptActions::self(), SIGNAL(shouldConnect(bool)), updateAction, SLOT(setEnabled(bool)));

    KStandardAction::preferences(this, SLOT(editSettings()), actionCollection());

    QAction* configureRepositoriesAction = actionCollection()->addAction("configure_repositories");
    configureRepositoriesAction->setIcon(QIcon::fromTheme("network-server"));
    configureRepositoriesAction->setText(i18nc("@action", "Sources"));
    configureRepositoriesAction->setToolTip(i18nc("@info:tooltip", "Configure package repositories"));
    connect(configureRepositoriesAction, SIGNAL(triggered()), this, SLOT(configureRepositories()));

    QAction* donateAction = actionCollection()->addAction("donate");
    donateAction->setIcon(QIcon::fromTheme("help-donate"));
    donateAction->setText(i18nc("@action", "Support"));
    donateAction->setToolTip(i18nc("@info:tooltip", "Support the project"));
    connect(donateAction, SIGNAL(triggered()), this, SLOT(openDonateDialog()));

    KStandardAction::aboutApp(this, SLOT(showAboutApplication()), actionCollection());

    // Local package actions
    m_addLocalFolderAction = actionCollection()->addAction("add_local_folder");
    m_addLocalFolderAction->setIcon(QIcon::fromTheme("folder-new"));
    m_addLocalFolderAction->setText(i18nc("@action", "Add Local Folder"));
    m_addLocalFolderAction->setToolTip(i18nc("@info:tooltip", "Add a directory containing local .deb packages"));
    connect(m_addLocalFolderAction, SIGNAL(triggered()), this, SLOT(addLocalFolder()));

    m_installLocalPackageAction = actionCollection()->addAction("install_local_package");
    m_installLocalPackageAction->setIcon(QIcon::fromTheme("package-x-generic"));
    m_installLocalPackageAction->setText(i18nc("@action", "Install Local Package"));
    m_installLocalPackageAction->setToolTip(i18nc("@info:tooltip", "Install a local .deb package file"));
    connect(m_installLocalPackageAction, SIGNAL(triggered()), this, SLOT(installLocalPackage()));

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
        mainToolBar->addSeparator();
        mainToolBar->addAction(m_addLocalFolderAction);
        mainToolBar->addAction(m_installLocalPackageAction);
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

void MainWindow::addLocalFolder()
{
    QString directory = QFileDialog::getExistingDirectory(this,
        i18nc("@title:window", "Select Local Package Directory"),
        QDir::homePath(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    
    if (!directory.isEmpty()) {
        QStringList currentFolders = MuonSettings::self()->localDebFolder().split(';');
        if (!currentFolders.contains(directory)) {
            currentFolders.append(directory);
            MuonSettings::self()->setLocalDebFolder(currentFolders.join(';'));
            MuonSettings::self()->save();
            
            // Update local package manager
            LocalPackageManager *localManager = LocalPackageManager::instance();
            if (localManager) {
                localManager->setLocalDebFolders(currentFolders);
                localManager->scanLocalPackages();
            }
            
            // Reload filters to show the new local files origin
            m_filterBox->reload();
            
            KMessageBox::information(this,
                i18nc("@info", "Local package directory added successfully. Scanning for packages..."),
                i18nc("@title:window", "Directory Added"));
        } else {
            KMessageBox::information(this,
                i18nc("@info", "This directory is already being monitored for local packages."),
                i18nc("@title:window", "Directory Already Added"));
        }
    }
}

void MainWindow::installLocalPackage()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        i18nc("@title:window", "Select Local Package File"),
        QDir::homePath(),
        i18nc("@label", "Debian Package Files (*.deb)"));
    
    if (!fileName.isEmpty()) {
        installLocalPackageFile(fileName);
    }
}

void MainWindow::installLocalPackageFile(const QString &fileName)
{
    if (fileName.isEmpty()) return;

    // Validate the .deb file
    LocalPackageManager *localManager = LocalPackageManager::instance();
    if (!localManager) {
        KMessageBox::error(this,
            i18nc("@info", "Local package manager is not available."),
            i18nc("@title:window", "Error"));
        return;
    }
    
    LocalPackageInfo info;
    if (!localManager->parseDebFile(fileName, info)) {
        KMessageBox::error(this,
            i18nc("@info", "The selected file is not a valid Debian package."),
            i18nc("@title:window", "Invalid Package File"));
        return;
    }
    
    // Check if package is already installed
    QApt::Package *existingPackage = m_backend->package(info.packageName);
    if (existingPackage && existingPackage->isInstalled()) {
        QString message = i18nc("@info",
            "Package '%1' version %2 is already installed.\n"
            "Version: %3\n"
            "Do you want to reinstall or upgrade it?",
            info.packageName, info.version, existingPackage->installedVersion());
        
        int result = KMessageBox::questionTwoActions(this, message,
            i18nc("@title:window", "Package Already Installed"),
            KGuiItem(i18nc("@action:button", "Reinstall")),
            KGuiItem(i18nc("@action:button", "Cancel")));
        
        if ( result != KMessageBox::PrimaryAction ) {
            return;
        }
    }
    
    // Perform safety checks
    QProcess safetyCheck;
    safetyCheck.start("apt-get", QStringList() << "-s" << "install" << fileName);
    if (!safetyCheck.waitForFinished(30000)) {
        KMessageBox::information(this,
            i18nc("@info", "Safety check timeout. Proceeding with installation."),
            i18nc("@title:window", "Safety Check Timeout"));
    } else if (safetyCheck.exitCode() != 0) {
        QString errorOutput = safetyCheck.readAllStandardError();
        KMessageBox::error(this,
            i18nc("@info", "Safety check failed:\n%1", errorOutput),
            i18nc("@title:window", "Installation Error"));
        return;
    }
    
    // Install the package
    setActionsEnabled(false);
    m_managerWidget->setEnabled(false);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    
    m_stack->setCurrentWidget(m_transWidget);
    
    // Create DebFile object from filename
    QApt::DebFile debFile(fileName);
    
    if (!debFile.isValid()) {
        KMessageBox::error(this,
            i18nc("@info", "The selected file is not a valid Debian package (QApt validation failed)."),
            i18nc("@title:window", "Invalid Package File"));
        return;
    }
    
    qDebug() << "Installing local package:" << fileName << "Valid:" << debFile.isValid();
    
    m_trans = m_backend->installFile(debFile);
    setupTransaction(m_trans);
    
    m_trans->run();
}

void MainWindow::openDebFile(const QString &debFilePath)
{
    qDebug() << "Opening .deb file:" << debFilePath;
    
    if (!QFile::exists(debFilePath)) {
        KMessageBox::error(this,
            i18nc("@info", "The specified .deb file does not exist: %1", debFilePath),
            i18nc("@title:window", "File Not Found"));
        return;
    }
    
    // Parse the .deb file to get package information
    LocalPackageManager *localManager = LocalPackageManager::instance();
    if (!localManager) {
        KMessageBox::error(this,
            i18nc("@info", "Local package manager is not available."),
            i18nc("@title:window", "Error"));
        return;
    }
    
    LocalPackageInfo info;
    if (!localManager->parseDebFile(debFilePath, info)) {
        KMessageBox::error(this,
            i18nc("@info", "The selected file is not a valid Debian package."),
            i18nc("@title:window", "Invalid Package File"));
        return;
    }
    
    // Add this file to the local package manager temporarily
    // This ensures it appears in the local files filter
    info.filename = debFilePath;
    
    // Use the proper method to add temporary package
    LocalPackageManager *manager = LocalPackageManager::instance();
    if (manager) {
        manager->addTemporaryPackage(info);
        
        // Switch to Local Files filter to show this package
        if (m_filterBox) {
            // Use the existing origin filter mechanism
            // The OriginFilter should handle setting the filter to "local"
            // We need to trigger a filter update
            m_filterBox->reload();
        }
        
        // Try to select and highlight this specific package
        if (m_managerWidget) {
            // Find the package in the model
            QApt::Package *package = m_backend->package(info.packageName);
            if (package) {
                // Use the existing selection mechanism
                // The ManagerWidget should have a way to select packages
                // For now, we'll just ensure the filter is set correctly
                qDebug() << "Found package" << info.packageName << "in backend, filter should show it";
            }
        }
        
        KMessageBox::information(this,
            i18nc("@info", "Local package '%1' version %2 has been loaded for installation.",
                  info.packageName, info.version),
            i18nc("@title:window", "Package Loaded"));
    }
}
