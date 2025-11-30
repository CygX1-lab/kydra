#include "DashboardWidget.h"
#include <QApt/Package>

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

DashboardWidget::DashboardWidget(QWidget *parent)
    : QWidget(parent)
    , m_backend(nullptr)
    , m_updatesWidget(nullptr)
    , m_updatesLabel(nullptr)
    , m_updateButton(nullptr)
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
        QTimer::singleShot(1000, this, &DashboardWidget::refreshUpdates);
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
        m_updatesLabel->setText(tr("%1 updates are available for your system.").arg(upgradeable));
        m_updatesLabel->setStyleSheet("color: #e74c3c; font-weight: bold;");
        m_updateButton->setVisible(true);
        m_updateButton->setText(tr("Update Now"));
    } else {
        m_updatesLabel->setText(tr("Your system is up to date."));
        m_updatesLabel->setStyleSheet("color: #27ae60; font-weight: bold;");
        m_updateButton->setVisible(false);
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
    
    QWidget *contentWidget = new QWidget();
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(20, 20, 20, 20);
    contentLayout->setSpacing(20);

    // 1. Hero Banner with Search
    QWidget *heroWidget = new QWidget();
    heroWidget->setFixedHeight(250); // Increased height for search bar
    heroWidget->setStyleSheet(
        "QWidget { "
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #6a11cb, stop:1 #2575fc); "
        "   border-radius: 10px; "
        "}"
    );
    
    QVBoxLayout *heroLayout = new QVBoxLayout(heroWidget);
    QLabel *heroTitle = new QLabel("Welcome to Kydra", heroWidget);
    heroTitle->setStyleSheet("color: white; font-size: 28px; font-weight: bold; background: transparent;");
    QLabel *heroSubtitle = new QLabel("Discover and manage your software with ease.", heroWidget);
    heroSubtitle->setStyleSheet("color: rgba(255, 255, 255, 0.9); font-size: 16px; background: transparent;");
    
    // Search Bar
    QLineEdit *searchEdit = new QLineEdit(heroWidget);
    searchEdit->setPlaceholderText("Search for applications...");
    searchEdit->setFixedWidth(400);
    searchEdit->setFixedHeight(40);
    searchEdit->setStyleSheet(
        "QLineEdit { "
        "   background-color: rgba(255, 255, 255, 0.9); "
        "   border: none; "
        "   border-radius: 20px; "
        "   padding: 0 15px; "
        "   font-size: 14px; "
        "   color: #333; "
        "}"
        "QLineEdit:focus { "
        "   background-color: #ffffff; "
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
    heroLayout->addSpacing(20);
    heroLayout->addWidget(searchEdit, 0, Qt::AlignCenter);
    heroLayout->addStretch();
    heroLayout->setContentsMargins(0, 0, 0, 0);

    contentLayout->addWidget(heroWidget);

    // 2. System Status / Updates
    m_updatesWidget = new QWidget();
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
    statusIcon->setPixmap(QIcon::fromTheme("system-software-update").pixmap(32, 32));
    
    m_updatesLabel = new QLabel("Checking for updates...");
    m_updatesLabel->setStyleSheet("font-size: 14px;");
    
    m_updateButton = new QPushButton("Update Now");
    m_updateButton->setVisible(false);
    connect(m_updateButton, &QPushButton::clicked, this, &DashboardWidget::showUpdates);
    
    updatesLayout->addWidget(statusIcon);
    updatesLayout->addWidget(m_updatesLabel, 1);
    updatesLayout->addWidget(m_updateButton);
    
    contentLayout->addWidget(m_updatesWidget);

    // 3. Categories
    QLabel *catHeader = new QLabel("Browse by Category");
    catHeader->setStyleSheet("font-size: 18px; font-weight: bold; margin-top: 10px;");
    contentLayout->addWidget(catHeader);

    QWidget *gridWidget = new QWidget();
    QGridLayout *gridLayout = new QGridLayout(gridWidget);
    gridLayout->setSpacing(15);

    struct Category { QString name; QString icon; };
    QList<Category> categories = {
        {"Internet", "applications-internet"},
        {"Multimedia", "applications-multimedia"},
        {"Graphics", "applications-graphics"},
        {"Office", "applications-office"},
        {"Development", "applications-development"},
        {"System", "applications-system"},
        {"Utilities", "applications-utilities"},
        {"Games", "applications-games"}
    };

    int row = 0;
    int col = 0;
    for (const auto &cat : categories) {
        QToolButton *btn = new QToolButton();
        btn->setText(cat.name);
        btn->setIcon(QIcon::fromTheme(cat.icon));
        btn->setIconSize(QSize(48, 48));
        btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        btn->setFixedHeight(100);
        btn->setStyleSheet(
            "QToolButton { "
            "   background-color: palette(window); "
            "   border: 1px solid palette(mid); "
            "   border-radius: 8px; "
            "   font-weight: bold; "
            "}"
            "QToolButton:hover { "
            "   background-color: palette(highlight); "
            "   color: palette(highlighted-text); "
            "}"
        );
        
        connect(btn, &QToolButton::clicked, this, [this, cat]() {
            emit categorySelected(cat.name);
        });

        gridLayout->addWidget(btn, row, col);
        col++;
        if (col > 3) {
            col = 0;
            row++;
        }
    }
    contentLayout->addWidget(gridWidget);

    // 4. Top Apps
    QLabel *topHeader = new QLabel("Featured Applications");
    topHeader->setStyleSheet("font-size: 18px; font-weight: bold; margin-top: 10px;");
    contentLayout->addWidget(topHeader);

    QWidget *topAppsWidget = new QWidget();
    QHBoxLayout *topAppsLayout = new QHBoxLayout(topAppsWidget);
    topAppsLayout->setSpacing(15);
    topAppsLayout->setContentsMargins(0, 0, 0, 0);

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
        btn->setFixedSize(120, 140);
        btn->setStyleSheet(
            "QToolButton { "
            "   border: 1px solid palette(mid); "
            "   border-radius: 8px; "
            "}"
            "QToolButton:hover { "
            "   background-color: palette(midlight); "
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
