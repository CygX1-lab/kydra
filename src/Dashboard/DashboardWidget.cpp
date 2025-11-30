#include "DashboardWidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QGridLayout>
#include <QToolButton>
#include <QStyle>

DashboardWidget::DashboardWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
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

    // 1. Hero Banner
    QWidget *heroWidget = new QWidget();
    heroWidget->setFixedHeight(200);
    heroWidget->setStyleSheet(
        "QWidget { "
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #6a11cb, stop:1 #2575fc); "
        "   border-radius: 10px; "
        "}"
    );
    
    QVBoxLayout *heroLayout = new QVBoxLayout(heroWidget);
    QLabel *heroTitle = new QLabel("Welcome to Kydra", heroWidget);
    heroTitle->setStyleSheet("color: white; font-size: 24px; font-weight: bold; background: transparent;");
    QLabel *heroSubtitle = new QLabel("Discover and manage your software with ease.", heroWidget);
    heroSubtitle->setStyleSheet("color: rgba(255, 255, 255, 0.8); font-size: 16px; background: transparent;");
    
    heroLayout->addStretch();
    heroLayout->addWidget(heroTitle);
    heroLayout->addWidget(heroSubtitle);
    heroLayout->addStretch();
    heroLayout->setContentsMargins(30, 0, 0, 0);

    contentLayout->addWidget(heroWidget);

    // 2. Categories
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

    // 3. Top Apps
    QLabel *topHeader = new QLabel("Top Applications");
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
        {"LibreOffice", "libreoffice", "libreoffice-startcenter"}
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
