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

#ifndef ENHANCEDDETAILSWIDGET_H
#define ENHANCEDDETAILSWIDGET_H

#include "DetailsWidget.h"
#include <QWidget>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextBrowser>
#include <QScrollArea>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QGroupBox>

namespace QApt {
    class Backend;
    class Package;
}

class EnhancedDetailsWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor headerGradientStart READ headerGradientStart WRITE setHeaderGradientStart)
    Q_PROPERTY(QColor headerGradientEnd READ headerGradientEnd WRITE setHeaderGradientEnd)
    Q_PROPERTY(int tabTransitionDuration READ tabTransitionDuration WRITE setTabTransitionDuration)

public:
    explicit EnhancedDetailsWidget(QWidget *parent = nullptr);
    ~EnhancedDetailsWidget() override;

    // Gradient colors for header
    QColor headerGradientStart() const;
    QColor headerGradientEnd() const;
    void setHeaderGradientStart(const QColor &color);
    void setHeaderGradientEnd(const QColor &color);

    // Tab transition animation duration
    int tabTransitionDuration() const;
    void setTabTransitionDuration(int duration);

    // Backend and package management
    void setBackend(QApt::Backend *backend);
    void setPackage(QApt::Package *package);
    void clear();

public Q_SLOTS:
    void refreshCurrentTab();
    void emitHideButtons();

Q_SIGNALS:
    void setInstall(QApt::Package *package);
    void setRemove(QApt::Package *package);
    void setUpgrade(QApt::Package *package);
    void setReInstall(QApt::Package *package);
    void setKeep(QApt::Package *package);
    void setPurge(QApt::Package *package);
    void emitHideButtonsSignal();

private Q_SLOTS:
    void animateTabTransition(int fromIndex, int toIndex);
    void onTabChanged(int index);

private:
    void setupUI();
    void setupHeader();
    void setupTabs();
    void applyGradientHeader();
    void applyImprovedTypography();
    void addTabTransitionAnimations();
    void createOverviewTab();
    void createFilesTab();
    void createDependenciesTab();
    void createVersionsTab();
    void createCollapsibleTechnicalDetails(QVBoxLayout *parentLayout);
    void createCollapsibleDependencies(QVBoxLayout *parentLayout);
    void updateTechnicalDetails();
    
    // Main layout and components
    QVBoxLayout *m_mainLayout;
    QTabWidget *m_tabWidget;
    
    // Enhanced header components
    QWidget *m_headerWidget;
    QHBoxLayout *m_headerLayout;
    QLabel *m_iconLabel;
    QLabel *m_nameLabel;
    QLabel *m_descriptionLabel;
    QLabel *m_versionLabel;
    QPushButton *m_installButton;
    QPushButton *m_updateButton;
    QPushButton *m_removeButton;
    
    // Enhanced content components
    QTextBrowser *m_descriptionBrowser;
    QTextBrowser *m_filesBrowser;
    QTextBrowser *m_dependenciesBrowser;
    QTextBrowser *m_versionsBrowser;
    QTextBrowser *m_reverseDepsBrowser;
    
    // Collapsible sections
    QGroupBox *m_technicalGroup;
    QGroupBox *m_dependenciesGroup;
    QGroupBox *m_reverseDepsGroup;
    QLabel *m_maintainerLabel;
    QLabel *m_originLabel;
    QLabel *m_sectionLabel;
    QLabel *m_architectureLabel;
    QLabel *m_sizeLabel;
    
    // Data
    QApt::Backend *m_backend;
    QApt::Package *m_package;
    
    // Styling
    QColor m_headerGradientStart;
    QColor m_headerGradientEnd;
    int m_tabTransitionDuration;
    
    // Animations
    QPropertyAnimation *m_tabFadeOutAnimation;
    QPropertyAnimation *m_tabFadeInAnimation;
    
    // Tab content widgets
    QWidget *m_overviewTab;
    QWidget *m_filesTab;
    QWidget *m_dependenciesTab;
    QWidget *m_versionsTab;
};

#endif // ENHANCEDDETAILSWIDGET_H