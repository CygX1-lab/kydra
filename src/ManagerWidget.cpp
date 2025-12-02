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

#include "ManagerWidget.h"

// Qt includes
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QSplitter>

// KDE includes
#include <KLocalizedString>

// QApt includes
#include <QApt/Backend>

// Own includes
#include "muonapt/MuonStrings.h"
#include "MuonSettings.h"
#include "DetailsWidget.h"
#include "PackageModel/PackageModel.h"
#include "PackageModel/PackageProxyModel.h"
#include "PackageModel/PackageView.h"
#include "PackageModel/PackageDelegate.h"
#include "PackageModel/LocalPackageManager.h"
#include "PackageModel/VirtualPackage.h"

ManagerWidget::ManagerWidget(QWidget *parent)
    : PackageWidget(parent)
{
    setPackagesType(PackageWidget::AvailablePackages);

    hideHeaderLabel();
    
    // Restore saved column state first
    QByteArray savedState = QByteArray::fromBase64(MuonSettings::self()->managerListColumns().toLatin1());
    if (!savedState.isEmpty()) {
        restoreColumnsState(savedState);
    }
    
    // Then apply the version column setting to override the saved state
    if (MuonSettings::self()->showVersionColumns()) {
        // Show installed and available version columns (indices 4 and 5)
        m_packageView->header()->setSectionHidden(4, false); // Installed Version
        m_packageView->header()->setSectionHidden(5, false); // Available Version
    } else {
        // Hide version columns if setting is false
        m_packageView->header()->setSectionHidden(4, true);  // Installed Version
        m_packageView->header()->setSectionHidden(5, true);  // Available Version
    }
    
    // Reduce visual noise by hiding less important columns by default
    // Hide maintainer and origin columns to reduce clutter
    m_packageView->header()->setSectionHidden(6, true);  // Maintainer column
    m_packageView->header()->setSectionHidden(7, true);  // Origin column
    
    showSearchEdit();
}

ManagerWidget::~ManagerWidget()
{
    // Save current column state
    MuonSettings::self()->setManagerListColumns(saveColumnsState().toBase64());
    
    // Save the version columns setting based on current visibility
    // Only update this if there's actual column state to save
    QByteArray currentState = saveColumnsState();
    if (!currentState.isEmpty()) {
        MuonSettings *settings = MuonSettings::self();
        bool versionColumnsVisible = !m_packageView->header()->isSectionHidden(4) && !m_packageView->header()->isSectionHidden(5);
        settings->setShowVersionColumns(versionColumnsVisible);
        settings->save();
    }
}

void ManagerWidget::reload()
{
    PackageWidget::reload();
    startSearch();
}

void ManagerWidget::filterByGroup(const QString &groupName)
{
    QString groupKey = MuonStrings::global()->groupKey(groupName);
    if (groupName == i18nc("@item:inlistbox Item that resets the filter to \"all\"", "All")) {
        groupKey.clear();
    }
    m_proxyModel->setGroupFilter(groupKey);
}

void ManagerWidget::filterByStatus(const QApt::Package::State state)
{
    m_proxyModel->setStateFilter(state);
}

void ManagerWidget::filterByOrigin(const QString &originName)
{
    QString origin;
    
    // Special handling for "local" origin which is not known by the backend
    if (originName == "local") {
        origin = "local";
        
        // Load virtual packages from local folder
        LocalPackageManager *localManager = LocalPackageManager::instance();
        if (localManager) {
            QList<LocalPackageInfo> virtualPackageInfos = localManager->getVirtualPackages();
            QList<VirtualPackage> virtualPackages;
            
            qDebug() << "Loading" << virtualPackageInfos.size() << "virtual packages for local filter";
            
            for (const LocalPackageInfo &info : virtualPackageInfos) {
                virtualPackages.append(VirtualPackage(info));
            }
            
            m_model->setVirtualPackages(virtualPackages);
        }
    } else {
        origin = m_backend->origin(originName);
        
        // Clear virtual packages when switching away from local filter
        m_model->clearVirtualPackages();
    }
    
    m_proxyModel->setOriginFilter(origin);
}

void ManagerWidget::filterByArchitecture(const QString &arch)
{
    m_proxyModel->setArchFilter(arch);
}

void ManagerWidget::showVersionColumns()
{
    if (m_packageView && m_packageView->header()) {
        m_packageView->header()->setSectionHidden(4, false); // Installed Version
        m_packageView->header()->setSectionHidden(5, false); // Available Version
    }
}

void ManagerWidget::hideVersionColumns()
{
    if (m_packageView && m_packageView->header()) {
        m_packageView->header()->setSectionHidden(4, true);  // Installed Version
        m_packageView->header()->setSectionHidden(5, true);  // Available Version
    }
}


