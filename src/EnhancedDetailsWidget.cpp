/***************************************************************************
 *   Enhanced Details Widget for Kydra Package Manager                     *
 *   Copyright © 2025 Kydra Project                                        *
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

#include "EnhancedDetailsWidget.h"

// Qt includes
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextBrowser>
#include <QScrollArea>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QPainter>
#include <QPalette>
#include <QDebug>

#include "PackageModel/FlatpakManager.h"

// KDE includes
#include <KLocalizedString>
#include <KFormat>
#include <KIconLoader>

// QApt includes
#include <QApt/Backend>
#include <QApt/Package>

// Own includes
#include "PackageModel/PackageIconExtractor.h"
#include "PackageModel/LocalPackageManager.h"
#include "muonapt/MuonStrings.h"

EnhancedDetailsWidget::EnhancedDetailsWidget(QWidget *parent)
    : QWidget(parent)
    , m_backend(nullptr)
    , m_package(nullptr)
    , m_isVirtual(false)
    , m_isLocal(false)
    , m_isFlatpak(false)
    , m_headerGradientStart(QPalette().color(QPalette::Window))
    , m_headerGradientEnd(QPalette().color(QPalette::Window).darker(110))
    , m_tabTransitionDuration(200)
{
    AppStreamHelper::instance()->init();
    setupUI();
    hide(); // Hide until a package is selected
}

EnhancedDetailsWidget::~EnhancedDetailsWidget() = default;

void EnhancedDetailsWidget::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);
    
    // Setup enhanced header
    setupHeader();
    
    // Setup tabbed interface
    setupTabs();
    
    // Apply animations
    addTabTransitionAnimations();
}

void EnhancedDetailsWidget::setupHeader()
{
    // Create gradient header with icon + name
    m_headerWidget = new QWidget(this);
    m_headerWidget->setFixedHeight(120);
    m_headerWidget->setAutoFillBackground(true);
    
    m_headerLayout = new QHBoxLayout(m_headerWidget);
    m_headerLayout->setContentsMargins(20, 10, 20, 10);
    m_headerLayout->setSpacing(15);
    
    // Package icon
    m_iconLabel = new QLabel(m_headerWidget);
    m_iconLabel->setFixedSize(64, 64);
    m_iconLabel->setScaledContents(true);
    m_iconLabel->setAlignment(Qt::AlignCenter);
    
    // Package name and description
    auto *textLayout = new QVBoxLayout();
    textLayout->setSpacing(5);
    
    m_nameLabel = new QLabel(m_headerWidget);
    QFont nameFont = m_nameLabel->font();
    nameFont.setPointSize(16);
    nameFont.setBold(true);
    m_nameLabel->setFont(nameFont);
    m_nameLabel->setWordWrap(true);
    
    m_descriptionLabel = new QLabel(m_headerWidget);
    QFont descFont = m_descriptionLabel->font();
    descFont.setPointSize(10);
    m_descriptionLabel->setFont(descFont);
    m_descriptionLabel->setWordWrap(true);
    m_descriptionLabel->setStyleSheet("QLabel { color: palette(mid); }");
    
    m_versionLabel = new QLabel(m_headerWidget);
    m_versionLabel->setStyleSheet("QLabel { color: palette(mid); font-size: 9pt; }");
    
    textLayout->addWidget(m_nameLabel);
    textLayout->addWidget(m_descriptionLabel);
    textLayout->addWidget(m_versionLabel);
    textLayout->addWidget(m_versionLabel);
    textLayout->addStretch();
    
    // Screenshot preview (initially hidden)
    m_screenshotLabel = new QLabel(m_headerWidget);
    m_screenshotLabel->setFixedSize(200, 112); // 16:9 aspect ratio
    m_screenshotLabel->setScaledContents(true);
    m_screenshotLabel->setStyleSheet("border: 1px solid palette(mid); border-radius: 4px; background: black;");
    m_screenshotLabel->hide();
    
    m_headerLayout->addWidget(m_screenshotLabel);
    
    // Action buttons
    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(8);
    
    m_installButton = new QPushButton(m_headerWidget);
    m_installButton->setIcon(QIcon::fromTheme("list-add"));
    m_installButton->setText(i18nc("@action:button", "Install"));
    m_installButton->setStyleSheet("QPushButton { padding: 8px 16px; font-weight: bold; }");
    m_installButton->setStyleSheet("QPushButton { padding: 8px 16px; font-weight: bold; }");
    connect(m_installButton, &QPushButton::clicked, this, [this]() { 
        if (m_isVirtual) {
            emit installLocalPackage(m_virtualPackage.filename());
        } else {
            emit setInstall(m_package); 
        }
    });
    
    m_updateButton = new QPushButton(m_headerWidget);
    m_updateButton->setIcon(QIcon::fromTheme("system-software-update"));
    m_updateButton->setText(i18nc("@action:button", "Update"));
    m_updateButton->setStyleSheet("QPushButton { padding: 8px 16px; font-weight: bold; }");
    connect(m_updateButton, &QPushButton::clicked, this, [this]() { emit setUpgrade(m_package); });
    
    m_removeButton = new QPushButton(m_headerWidget);
    m_removeButton->setIcon(QIcon::fromTheme("edit-delete"));
    m_removeButton->setText(i18nc("@action:button", "Remove"));
    m_removeButton->setStyleSheet("QPushButton { padding: 8px 16px; font-weight: bold; }");
    connect(m_removeButton, &QPushButton::clicked, this, [this]() { emit setRemove(m_package); });
    
    buttonLayout->addWidget(m_installButton);
    buttonLayout->addWidget(m_updateButton);
    buttonLayout->addWidget(m_removeButton);
    buttonLayout->addStretch();
    
    m_headerLayout->addWidget(m_iconLabel);
    m_headerLayout->addLayout(textLayout, 1);
    m_headerLayout->addLayout(buttonLayout);
    
    // Apply gradient background
    applyGradientHeader();
    
    // Add shadow effect
    auto *shadowEffect = new QGraphicsDropShadowEffect();
    shadowEffect->setBlurRadius(10);
    shadowEffect->setColor(QColor(0, 0, 0, 30));
    shadowEffect->setOffset(0, 2);
    m_headerWidget->setGraphicsEffect(shadowEffect);
    
    m_mainLayout->addWidget(m_headerWidget);
}

void EnhancedDetailsWidget::setupTabs()
{
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setDocumentMode(true);
    m_tabWidget->setStyleSheet(R"(
        QTabWidget::pane {
            border: none;
            background-color: palette(window);
        }
        QTabBar::tab {
            padding: 12px 20px;
            margin: 2px;
            border-radius: 4px;
            background-color: palette(button);
        }
        QTabBar::tab:selected {
            background-color: palette(highlight);
            color: palette(highlighted-text);
        }
        QTabBar::tab:hover:!selected {
            background-color: palette(button-hover);
        }
    )");
    
    connect(m_tabWidget, &QTabWidget::currentChanged, this, &EnhancedDetailsWidget::onTabChanged);
    
    // Create tab content
    createOverviewTab();
    createFilesTab();
    createDependenciesTab();
    createVersionsTab();
    
    m_mainLayout->addWidget(m_tabWidget);
}

void EnhancedDetailsWidget::createOverviewTab()
{
    m_overviewTab = new QWidget();
    auto *layout = new QVBoxLayout(m_overviewTab);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(15);
    
    // Long description with improved typography
    m_descriptionBrowser = new QTextBrowser(m_overviewTab);
    m_descriptionBrowser->setStyleSheet(R"(
        QTextBrowser {
            border: 1px solid palette(mid);
            border-radius: 8px;
            padding: 15px;
            background-color: palette(base);
            font-family: 'Noto Sans', sans-serif;
            font-size: 11pt;
            line-height: 1.6;
        }
    )");
    m_descriptionBrowser->setOpenExternalLinks(true);
    
    layout->addWidget(m_descriptionBrowser);
    layout->addStretch();
    
    m_tabWidget->addTab(m_overviewTab, i18nc("@title:tab", "Overview"));
}

void EnhancedDetailsWidget::createFilesTab()
{
    m_filesTab = new QWidget();
    auto *layout = new QVBoxLayout(m_filesTab);
    layout->setContentsMargins(20, 20, 20, 20);
    
    m_filesBrowser = new QTextBrowser(m_filesTab);
    m_filesBrowser->setStyleSheet(R"(
        QTextBrowser {
            border: 1px solid palette(mid);
            border-radius: 8px;
            padding: 10px;
            background-color: palette(base);
            font-family: monospace;
            font-size: 9pt;
        }
    )");
    
    layout->addWidget(m_filesBrowser);
    
    m_tabWidget->addTab(m_filesTab, i18nc("@title:tab", "Files"));
}

void EnhancedDetailsWidget::createDependenciesTab()
{
    m_dependenciesTab = new QWidget();
    auto *layout = new QVBoxLayout(m_dependenciesTab);
    layout->setContentsMargins(20, 20, 20, 20);
    
    m_dependenciesBrowser = new QTextBrowser(m_dependenciesTab);
    m_dependenciesBrowser->setStyleSheet(R"(
        QTextBrowser {
            border: 1px solid palette(mid);
            border-radius: 8px;
            padding: 10px;
            background-color: palette(base);
            font-family: monospace;
            font-size: 9pt;
        }
    )");
    
    layout->addWidget(m_dependenciesBrowser);
    
    m_tabWidget->addTab(m_dependenciesTab, i18nc("@title:tab", "Dependencies"));
}

void EnhancedDetailsWidget::createVersionsTab()
{
    m_versionsTab = new QWidget();
    auto *layout = new QVBoxLayout(m_versionsTab);
    layout->setContentsMargins(20, 20, 20, 20);
    
    m_versionsBrowser = new QTextBrowser(m_versionsTab);
    m_versionsBrowser->setStyleSheet(R"(
        QTextBrowser {
            border: 1px solid palette(mid);
            border-radius: 8px;
            padding: 10px;
            background-color: palette(base);
            font-family: monospace;
            font-size: 9pt;
        }
    )");
    
    layout->addWidget(m_versionsBrowser);
    
    m_tabWidget->addTab(m_versionsTab, i18nc("@title:tab", "Versions"));
}

void EnhancedDetailsWidget::applyGradientHeader()
{
    // Create gradient background for header
    QPalette palette = m_headerWidget->palette();
    QLinearGradient gradient(0, 0, m_headerWidget->width(), m_headerWidget->height());
    
    if (m_isLocal) {
        // Distinctive gradient for local packages (Blue-ish)
        gradient.setColorAt(0.0, QColor(66, 133, 244).lighter(150));
        gradient.setColorAt(1.0, QColor(66, 133, 244).lighter(110));
    } else if (m_isFlatpak) {
        // Distinctive gradient for Flatpaks (Purple-ish)
        gradient.setColorAt(0.0, QColor(142, 68, 173).lighter(150));
        gradient.setColorAt(1.0, QColor(142, 68, 173).lighter(110));
    } else {
        // Standard gradient
        gradient.setColorAt(0.0, m_headerGradientStart);
        gradient.setColorAt(1.0, m_headerGradientEnd);
    }
    
    palette.setBrush(QPalette::Window, QBrush(gradient));
    m_headerWidget->setPalette(palette);
    m_headerWidget->setAutoFillBackground(true);
}

void EnhancedDetailsWidget::applyImprovedTypography()
{
    // Apply improved typography to text browsers
    QString typographyStyle = R"(
        font-family: 'Noto Sans', sans-serif;
        font-size: 11pt;
        line-height: 1.6;
        color: palette(text);
    )";
    
    m_descriptionBrowser->setStyleSheet(typographyStyle);
}

void EnhancedDetailsWidget::addTabTransitionAnimations()
{
    // Create fade animations for tab transitions
    m_tabFadeOutAnimation = new QPropertyAnimation(m_tabWidget, "windowOpacity");
    m_tabFadeOutAnimation->setDuration(m_tabTransitionDuration / 2);
    m_tabFadeOutAnimation->setStartValue(1.0);
    m_tabFadeOutAnimation->setEndValue(0.8);
    
    m_tabFadeInAnimation = new QPropertyAnimation(m_tabWidget, "windowOpacity");
    m_tabFadeInAnimation->setDuration(m_tabTransitionDuration / 2);
    m_tabFadeInAnimation->setStartValue(0.8);
    m_tabFadeInAnimation->setEndValue(1.0);
}

void EnhancedDetailsWidget::animateTabTransition(int fromIndex, int toIndex)
{
    Q_UNUSED(fromIndex)
    Q_UNUSED(toIndex)
    
    // Chain fade out and fade in animations
    connect(m_tabFadeOutAnimation, &QPropertyAnimation::finished, [this]() {
        m_tabFadeInAnimation->start();
    });
    
    m_tabFadeOutAnimation->start();
}

// Property getters and setters
QColor EnhancedDetailsWidget::headerGradientStart() const
{
    return m_headerGradientStart;
}

QColor EnhancedDetailsWidget::headerGradientEnd() const
{
    return m_headerGradientEnd;
}

void EnhancedDetailsWidget::setHeaderGradientStart(const QColor &color)
{
    m_headerGradientStart = color;
    applyGradientHeader();
}

void EnhancedDetailsWidget::setHeaderGradientEnd(const QColor &color)
{
    m_headerGradientEnd = color;
    applyGradientHeader();
}

int EnhancedDetailsWidget::tabTransitionDuration() const
{
    return m_tabTransitionDuration;
}

void EnhancedDetailsWidget::setTabTransitionDuration(int duration)
{
    m_tabTransitionDuration = duration;
    if (m_tabFadeOutAnimation) {
        m_tabFadeOutAnimation->setDuration(duration / 2);
    }
    if (m_tabFadeInAnimation) {
        m_tabFadeInAnimation->setDuration(duration / 2);
    }
}

// Backend and package management
void EnhancedDetailsWidget::setBackend(QApt::Backend *backend)
{
    m_backend = backend;
}

void EnhancedDetailsWidget::setPackage(QApt::Package *package)
{
    m_package = package;
    m_isVirtual = false;
    m_isLocal = false;
    m_isFlatpak = false;
    
    if (!package) {
        clear();
        return;
    }
    
    // Update header information
    m_iconLabel->setPixmap(PackageIconExtractor::instance()->getPackageIcon(package).pixmap(64, 64));
    m_nameLabel->setText(package->name());
    m_descriptionLabel->setText(package->shortDescription());
    m_versionLabel->setText(i18nc("@label", "Version: %1", package->version()));
    
    // AppStream Integration
    QString screenshotUrl = AppStreamHelper::instance()->getScreenshotUrl(package->name());
    if (!screenshotUrl.isEmpty()) {
        // In a real implementation, we would fetch this asynchronously via QNetworkAccessManager
        // For now, we'll just log it or set a placeholder if we had a local cache
        // m_screenshotLabel->show();
        // Network fetching logic would go here
        qDebug() << "Found screenshot URL for" << package->name() << ":" << screenshotUrl;
    } else {
        m_screenshotLabel->hide();
    }
    
    // Check if it's a local package
    if (LocalPackageManager::instance()->isLocalInstallPackage(package->name())) {
        m_isLocal = true;
        m_versionLabel->setText(i18nc("@label", "Version: %1 (Local Package)", package->version()));
    }
    
    // Update gradient header
    applyGradientHeader();
    
    // Update button visibility based on package state
    bool isInstalled = package->isInstalled();
    bool isUpgradeable = (package->state() & QApt::Package::Upgradeable);
    
    m_installButton->setVisible(!isInstalled);
    m_updateButton->setVisible(isInstalled && isUpgradeable);
    m_removeButton->setVisible(isInstalled);
    
    // Update tab content
    refreshCurrentTab();
    
    show();
}

void EnhancedDetailsWidget::setVirtualPackage(const VirtualPackage &package)
{
    m_virtualPackage = package;
    m_isVirtual = true;
    m_isLocal = true;
    m_isFlatpak = false;
    m_package = nullptr;
    
    // Update header information
    // Use extracted icon if available
    QString iconPath = package.iconPath();
    if (!iconPath.isEmpty()) {
        m_iconLabel->setPixmap(QIcon(iconPath).pixmap(64, 64));
    } else {
        m_iconLabel->setPixmap(QIcon::fromTheme("application-x-deb").pixmap(64, 64));
    }
    m_nameLabel->setText(package.name());
    m_descriptionLabel->setText(package.shortDescription());
    m_versionLabel->setText(i18nc("@label", "Version: %1 (Local Package)", package.version()));
    
    // Update gradient for local package
    applyGradientHeader();
    
    // Update button visibility
    // Virtual packages are by definition "not installed" in the APT sense (or at least treated as such for display)
    // So Install button should be visible.
    m_installButton->setVisible(true);
    m_updateButton->setVisible(false);
    m_removeButton->setVisible(false);
    
    // Update tab content
    refreshCurrentTab();
    
    show();
}

void EnhancedDetailsWidget::setFlatpak(const QString &flatpakId)
{
    FlatpakPackage pkg = FlatpakManager::instance()->getPackage(flatpakId);
    
    m_package = nullptr;
    m_isVirtual = false;
    m_isLocal = false;
    m_isFlatpak = true;
    
    // Update header
    // Try to get icon from AppStream or theme
    QString iconName = pkg.id;
    if (QIcon::hasThemeIcon(iconName)) {
         m_iconLabel->setPixmap(QIcon::fromTheme(iconName).pixmap(64, 64));
    } else {
         m_iconLabel->setPixmap(QIcon::fromTheme("application-x-executable").pixmap(64, 64));
    }
    
    m_nameLabel->setText(pkg.name);
    m_descriptionLabel->setText(pkg.description);
    m_versionLabel->setText(i18nc("@label", "Version: %1 (Flatpak)", pkg.version));
    
    applyGradientHeader();
    
    // Buttons
    m_installButton->setVisible(!pkg.isInstalled);
    m_removeButton->setVisible(pkg.isInstalled);
    m_updateButton->setVisible(false); // TODO: Check updates
    
    // Connect buttons (simplified for now)
    disconnect(m_installButton, nullptr, nullptr, nullptr);
    disconnect(m_removeButton, nullptr, nullptr, nullptr);
    
    connect(m_installButton, &QPushButton::clicked, this, [this, pkg]() {
        FlatpakManager::instance()->installPackage(pkg.id, pkg.remote);
    });
    
    connect(m_removeButton, &QPushButton::clicked, this, [this, pkg]() {
        FlatpakManager::instance()->removePackage(pkg.id);
    });
    
    refreshCurrentTab();
    show();
}

void EnhancedDetailsWidget::clear()
{
    m_package = nullptr;
    m_isVirtual = false;
    m_isLocal = false;
    m_isFlatpak = false;
    
    // Clear header
    m_iconLabel->clear();
    m_nameLabel->clear();
    m_descriptionLabel->clear();
    m_versionLabel->clear();
    
    // Clear tab content
    m_descriptionBrowser->clear();
    m_filesBrowser->clear();
    m_dependenciesBrowser->clear();
    m_versionsBrowser->clear();
    
    hide();
}

void EnhancedDetailsWidget::refreshCurrentTab()
{
    if (!m_package && !m_isVirtual) {
        return;
    }
    
    int currentIndex = m_tabWidget->currentIndex();
    
    if (m_isVirtual) {
        switch (currentIndex) {
        case 0: // Overview tab
            {
                QString descriptionText = m_virtualPackage.longDescription();
                descriptionText += QString("\n\n") + i18nc("@info", "This is a local package file: %1", m_virtualPackage.filename());
                m_descriptionBrowser->setPlainText(descriptionText);
            }
            break;
        case 1: // Files tab
            m_filesBrowser->setPlainText(i18nc("@info", "File list not available for uninstalled local packages."));
            break;
        case 2: // Dependencies tab
            {
                QString dependenciesText;
                QStringList deps = m_virtualPackage.dependencies();
                
                if (!deps.isEmpty()) {
                    dependenciesText += i18nc("@title", "Dependencies:\n");
                    foreach(const QString &dep, deps) {
                        dependenciesText.append("  " + dep + '\n');
                    }
                } else {
                    dependenciesText = i18nc("@info", "No dependencies found");
                }
                m_dependenciesBrowser->setPlainText(dependenciesText);
            }
            break;
        case 3: // Versions tab
            m_versionsBrowser->setPlainText(m_virtualPackage.version() + " " + i18nc("@label", "(local file)"));
            break;
        }
        return;
    }
    
    switch (currentIndex) {
    case 0: // Overview tab
        {
            // Long description with improved typography
            QString descriptionText = m_package->longDescription();
            
            // Add support information
            if (m_package->isSupported()) {
                QDateTime endDate = m_package->supportedUntil();
                descriptionText += QString("\n\n") + 
                    i18nc("@info Tells how long Canonical, Ltd. will support a package",
                          "Canonical provides critical updates for %1 until %2.",
                          m_package->name(), endDate.date().toString());
            } else {
                descriptionText += QString("\n\n") + 
                    i18nc("@info Tells how long Canonical, Ltd. will support a package",
                          "Canonical does not provide updates for %1. Some updates "
                          "may be provided by the Ubuntu community.", m_package->name());
            }
            
            m_descriptionBrowser->setPlainText(descriptionText);
        }
        break;
        
    case 1: // Files tab
        if (m_package->isInstalled()) {
            QStringList filesList = m_package->installedFilesList();
            filesList.sort();
            QString filesText;
            
            foreach(const QString &file, filesList) {
                filesText.append(file + '\n');
            }
            
            m_filesBrowser->setPlainText(filesText);
        } else {
            m_filesBrowser->setPlainText(i18nc("@info", "Package is not installed"));
        }
        break;
        
    case 2: // Dependencies tab
        {
            QString dependenciesText;
            QStringList deps = m_package->dependencyList(false);
            QStringList revDeps = m_package->requiredByList();
            
            if (!deps.isEmpty()) {
                dependenciesText += i18nc("@title", "Dependencies:\n");
                foreach(const QString &dep, deps) {
                    dependenciesText.append("  " + dep + '\n');
                }
                dependenciesText += '\n';
            }
            
            if (!revDeps.isEmpty()) {
                dependenciesText += i18nc("@title", "Reverse Dependencies:\n");
                foreach(const QString &revDep, revDeps) {
                    dependenciesText.append("  " + revDep + '\n');
                }
            }
            
            if (dependenciesText.isEmpty()) {
                dependenciesText = i18nc("@info", "No dependencies found");
            }
            
            m_dependenciesBrowser->setPlainText(dependenciesText);
        }
        break;
        
    case 3: // Versions tab
        {
            QStringList availableVersions = m_package->availableVersions();
            QString versionsText;
            
            for (int i = 0; i < availableVersions.count(); ++i) {
                if (i == 0) {
                    versionsText.append(availableVersions[i] + " " + i18nc("@label", "(current)") + '\n');
                } else {
                    versionsText.append(availableVersions[i] + '\n');
                }
            }
            
            m_versionsBrowser->setPlainText(versionsText);
        }
        break;
    }
}

void EnhancedDetailsWidget::onTabChanged(int index)
{
    Q_UNUSED(index)
    refreshCurrentTab();
}

void EnhancedDetailsWidget::emitHideButtons()
{
    emit emitHideButtonsSignal();
}