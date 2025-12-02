#include "DashboardWidget.h"
#include <QApt/Package>
#include "muonapt/MuonStrings.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QGridLayout>
#include <QToolButton>
#include <QStyle>
#include <QLineEdit>
#include <QTimer>
#include <QIcon>
#include <QPixmap>
#include <QSet>

DashboardWidget::DashboardWidget(QWidget *parent)
    : QWidget(parent)
    , m_backend(nullptr)
    , m_updatesWidget(nullptr)
    , m_updatesLabel(nullptr)
    , m_updateButton(nullptr)
    , m_categoryGridWidget(nullptr)
    , m_categoryLayout(nullptr)
{
    setupUi();
}

void DashboardWidget::setBackend(QApt::Backend *backend)
{
    m_backend = backend;
    if (m_backend) {
        connect(m_backend, &QApt::Backend::cacheReloadFinished, this, &DashboardWidget::refreshUpdates);
        connect(m_backend, &QApt::Backend::packageChanged, this, &DashboardWidget::refreshUpdates);
        
        // Initial refresh after a short delay to ensure backend is ready
        QTimer::singleShot(1000, this, [this]() {
            refreshUpdates();
            populateCategories();
        });
    }
}

void DashboardWidget::refreshUpdates()
{
    updateSystemStatus();
}

void DashboardWidget::updateSystemStatus()
{
    if (!m_backend || !m_updatesWidget) return;
    
    int upgradeable = m_backend->packageCount(QApt::Package::Upgradeable);
    
    if (upgradeable > 0) {
        m_updatesLabel->setText(tr("<b>%1 updates available</b><br>Keep your system secure and up-to-date.").arg(upgradeable));
        m_updatesWidget->setStyleSheet(
            "QWidget { "
            "   background-color: #fff3cd; "
            "   border: 1px solid #ffeeba; "
            "   border-radius: 8px; "
            "   color: #856404; "
            "}"
        );
        m_updateButton->setVisible(true);
        m_updateButton->setText(tr("Update Now"));
        m_updateButton->setStyleSheet(
            "QPushButton { "
            "   background-color: #ffc107; "
            "   color: #212529; "
            "   border: none; "
            "   border-radius: 4px; "
            "   padding: 6px 12px; "
            "   font-weight: bold; "
            "}"
            "QPushButton:hover { "
            "   background-color: #e0a800; "
            "}"
        );
    } else {
        m_updatesLabel->setText(tr("<b>System is up to date</b><br>You are running the latest software versions."));
        m_updatesWidget->setStyleSheet(
            "QWidget { "
            "   background-color: #d4edda; "
            "   border: 1px solid #c3e6cb; "
            "   border-radius: 8px; "
            "   color: #155724; "
            "}"
        );
        m_updateButton->setVisible(false);
    }
}

void DashboardWidget::populateCategories()
{
    if (!m_backend || !m_categoryLayout) return;

    // Clear existing items
    QLayoutItem *item;
    while ((item = m_categoryLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    QApt::GroupList groups = m_backend->availableGroups();
    QSet<QString> availableGroupKeys;
    for (const auto &group : groups) {
        // QApt::Group is implicitly convertible to QString (the key)
        availableGroupKeys.insert(group);
    }

    struct CategoryDef { QString key; QString icon; QString color; };
    QList<CategoryDef> preferredCategories = {
        {"web", "applications-internet", "#3498db"},
        {"comm", "applications-internet", "#2980b9"},
        {"sound", "applications-multimedia", "#9b59b6"},
        {"graphics", "applications-graphics", "#e74c3c"},
        {"text", "applications-office", "#f1c40f"},
        {"devel", "applications-development", "#34495e"},
        {"admin", "applications-system", "#95a5a6"},
        {"utils", "applications-utilities", "#1abc9c"},
        {"games", "applications-games", "#e67e22"},
        {"education", "applications-education", "#2ecc71"},
        {"science", "applications-science", "#16a085"}
    };

    int row = 0;
    int col = 0;
    MuonStrings *strings = MuonStrings::global();

    for (const auto &catDef : preferredCategories) {
        if (availableGroupKeys.contains(catDef.key)) {
            QString name = strings->groupName(catDef.key);
            
            QToolButton *btn = new QToolButton();
            btn->setText(name);
            btn->setIcon(QIcon::fromTheme(catDef.icon));
            btn->setIconSize(QSize(48, 48));
            btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
            btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            btn->setFixedHeight(110);
            btn->setCursor(Qt::PointingHandCursor);
            
            // Dynamic stylesheet for each category
            QString style = QString(
                "QToolButton { "
                "   background-color: palette(button); "
                "   border: 1px solid palette(midlight); "
                "   border-radius: 12px; "
                "   font-weight: 600; "
                "   font-size: 14px; "
                "   padding: 10px; "
                "}"
                "QToolButton:hover { "
                "   background-color: palette(light); "
                "   border: 1px solid %1; "
                "   color: %1; "
                "}"
            ).arg(catDef.color);
            
            btn->setStyleSheet(style);
            
            connect(btn, &QToolButton::clicked, this, [this, name]() {
                emit categorySelected(name);
            });

            m_categoryLayout->addWidget(btn, row, col);
            col++;
            if (col > 3) {
                col = 0;
                row++;
            }
        }
    }
}

void DashboardWidget::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("QScrollArea { background-color: transparent; }");
    
    QWidget *contentWidget = new QWidget();
    contentWidget->setStyleSheet("QWidget { background-color: palette(window); }");
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(30, 30, 30, 30);
    contentLayout->setSpacing(30);

    // 1. Hero Banner with Search
    QWidget *heroWidget = new QWidget();
    heroWidget->setFixedHeight(280);
    heroWidget->setStyleSheet(
        "QWidget { "
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #4facfe, stop:1 #00f2fe); "
        "   border-radius: 15px; "
        "}"
    );
    
    QVBoxLayout *heroLayout = new QVBoxLayout(heroWidget);
    heroLayout->setContentsMargins(40, 40, 40, 40);
    
    QLabel *heroTitle = new QLabel("Welcome to Kydra", heroWidget);
    heroTitle->setStyleSheet("color: white; font-size: 32px; font-weight: 800; background: transparent;");
    QLabel *heroSubtitle = new QLabel("Discover and manage your software with ease.", heroWidget);
    heroSubtitle->setStyleSheet("color: rgba(255, 255, 255, 0.95); font-size: 18px; background: transparent; margin-top: 5px;");
    
    // Search Bar
    QLineEdit *searchEdit = new QLineEdit(heroWidget);
    searchEdit->setPlaceholderText("Search for applications...");
    searchEdit->setFixedWidth(450);
    searchEdit->setFixedHeight(50);
    searchEdit->setStyleSheet(
        "QLineEdit { "
        "   background-color: white; "
        "   border: none; "
        "   border-radius: 25px; "
        "   padding: 0 20px; "
        "   font-size: 16px; "
        "   color: #333; "
        "   selection-background-color: #4facfe; "
        "}"
        "QLineEdit:focus { "
        "   background-color: #ffffff; "
        "   border: 2px solid rgba(255, 255, 255, 0.5); "
        "}"
    );
    connect(searchEdit, &QLineEdit::returnPressed, this, [this, searchEdit]() {
        if (!searchEdit->text().isEmpty()) {
            emit searchRequested(searchEdit->text());
        }
    });

    heroLayout->addStretch();
    heroLayout->addWidget(heroTitle, 0, Qt::AlignCenter);
    heroLayout->addWidget(heroSubtitle, 0, Qt::AlignCenter);
    heroLayout->addSpacing(25);
    heroLayout->addWidget(searchEdit, 0, Qt::AlignCenter);
    heroLayout->addStretch();

    contentLayout->addWidget(heroWidget);

    // 2. System Status / Updates
    m_updatesWidget = new QWidget();
    // Default style, will be updated by updateSystemStatus
    m_updatesWidget->setStyleSheet(
        "QWidget { "
        "   background-color: palette(window); "
        "   border: 1px solid palette(mid); "
        "   border-radius: 8px; "
        "}"
    );
    QHBoxLayout *updatesLayout = new QHBoxLayout(m_updatesWidget);
    updatesLayout->setContentsMargins(20, 15, 20, 15);
    
    QLabel *statusIcon = new QLabel();

    QIcon updateIcon = QIcon::fromTheme("system-upgrade", QIcon::fromTheme("view-refresh"));
    QPixmap updatePixmap = updateIcon.pixmap(48, 48);
    if (updatePixmap.isNull()) {
        // Fallback if even "view-refresh" didn't provide a pixmap
        updateIcon = QIcon::fromTheme("dialog-information");
        updatePixmap = updateIcon.pixmap(48, 48);
    }
    statusIcon->setPixmap(updatePixmap);
    statusIcon->setStyleSheet("background: transparent; border: none;");
    
    m_updatesLabel = new QLabel("Checking for updates...");
    m_updatesLabel->setStyleSheet("font-size: 15px; background: transparent; border: none;");
    m_updatesLabel->setWordWrap(true);
    
    m_updateButton = new QPushButton("Update Now");
    m_updateButton->setCursor(Qt::PointingHandCursor);
    m_updateButton->setVisible(false);
    connect(m_updateButton, &QPushButton::clicked, this, &DashboardWidget::showUpdates);
    
    updatesLayout->addWidget(statusIcon);
    updatesLayout->addSpacing(15);
    updatesLayout->addWidget(m_updatesLabel, 1);
    updatesLayout->addWidget(m_updateButton);
    
    contentLayout->addWidget(m_updatesWidget);

    // 3. Categories
    QLabel *catHeader = new QLabel("Browse by Category");
    catHeader->setStyleSheet("font-size: 22px; font-weight: 700; margin-top: 10px; color: palette(text);");
    contentLayout->addWidget(catHeader);

    m_categoryGridWidget = new QWidget();
    m_categoryGridWidget->setStyleSheet("background: transparent;");
    m_categoryLayout = new QGridLayout(m_categoryGridWidget);
    m_categoryLayout->setSpacing(20);
    m_categoryLayout->setContentsMargins(0, 10, 0, 10);
    
    contentLayout->addWidget(m_categoryGridWidget);

    // 4. Top Apps
    QLabel *topHeader = new QLabel("Featured Applications");
    topHeader->setStyleSheet("font-size: 22px; font-weight: 700; margin-top: 10px; color: palette(text);");
    contentLayout->addWidget(topHeader);

    QWidget *topAppsWidget = new QWidget();
    topAppsWidget->setStyleSheet("background: transparent;");
    QHBoxLayout *topAppsLayout = new QHBoxLayout(topAppsWidget);
    topAppsLayout->setSpacing(20);
    topAppsLayout->setContentsMargins(0, 10, 0, 10);

    struct App { QString name; QString id; QString icon; };
    QList<App> topApps = {
        {"Firefox", "firefox", "firefox"},
        {"VLC", "vlc", "vlc"},
        {"GIMP", "gimp", "gimp"},
        {"LibreOffice", "libreoffice", "libreoffice-startcenter"},
        {"Kdenlive", "kdenlive", "kdenlive"},
        {"Blender", "blender", "blender"}
    };

    for (const auto &app : topApps) {
        QToolButton *btn = new QToolButton();
        btn->setText(app.name);
        btn->setIcon(QIcon::fromTheme(app.icon));
        btn->setIconSize(QSize(64, 64));
        btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        btn->setFixedSize(130, 150);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet(
            "QToolButton { "
            "   background-color: palette(button); "
            "   border: 1px solid palette(midlight); "
            "   border-radius: 12px; "
            "   font-weight: 600; "
            "   padding: 10px; "
            "}"
            "QToolButton:hover { "
            "   background-color: palette(light); "
            "   border: 1px solid palette(highlight); "

            "}"
        );

        connect(btn, &QToolButton::clicked, this, [this, app]() {
            emit packageSelected(app.id);
        });

        topAppsLayout->addWidget(btn);
    }
    topAppsLayout->addStretch();
    contentLayout->addWidget(topAppsWidget);

    contentLayout->addStretch();
    scrollArea->setWidget(contentWidget);
    mainLayout->addWidget(scrollArea);
}
