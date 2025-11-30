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

#include "PackageProxyModel.h"

// KDE includes
#include <KLocalizedString>

// QApt includes
#include <QApt/Backend>

// Own includes
#include "PackageModel.h"
#include "MuonSettings.h"
#include "LocalPackageManager.h"

// Qt includes
#include <QRegularExpression>

constexpr int status_sort_magic = (QApt::Package::Installed |
                                   QApt::Package::Upgradeable |
                                   QApt::Package::NowBroken |
                                   QApt::Package::New);

bool packageStatusLessThan(QApt::Package *p1, QApt::Package *p2)
{
    return (p1->state() & (status_sort_magic))  <
           (p2->state() & (status_sort_magic));
}

constexpr int requested_sort_magic = (QApt::Package::ToInstall
                                         | QApt::Package::ToUpgrade
                                         | QApt::Package::ToRemove
                                         | QApt::Package::ToPurge
                                         | QApt::Package::ToReInstall
                                         | QApt::Package::ToDowngrade
                                         | QApt::Package::ToKeep);

bool packageRequestedLessThan(QApt::Package *p1, QApt::Package *p2)
{
    return (p1->state() & (requested_sort_magic))  <
           (p2->state() & (requested_sort_magic));
}

PackageProxyModel::PackageProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , m_backend(0)
    , m_stateFilter((QApt::Package::State)0)
    , m_sortByRelevancy(false)
    , m_useSearchResults(false)
{
}

void PackageProxyModel::setBackend(QApt::Backend *backend)
{
    m_backend = backend;
    m_packages = static_cast<PackageModel *>(sourceModel())->packages();
}

void PackageProxyModel::search(const QString &searchText)
{
    // 1-character searches are painfully slow. >= 2 chars are fine, though
    if (searchText.size() > 1) {
        // First try xapian-based search
        m_searchPackages = m_backend->search(searchText);
        
        // If xapian search returns no results and slow search is enabled, try supplemental search
        if (m_searchPackages.isEmpty() && MuonSettings::self()->useSlowSearch()) {
            m_searchPackages = performSlowSearch(searchText);
        }
        
        if (!m_useSearchResults) {
            m_sortByRelevancy = true;
        }
        m_useSearchResults = true;
    } else {
        m_searchPackages.clear();
        m_packages =  static_cast<PackageModel *>(sourceModel())->packages();
        m_sortByRelevancy = false;
        m_useSearchResults = false;
    }

    invalidate();
}

void PackageProxyModel::setSortByRelevancy(bool enabled)
{
    m_sortByRelevancy = enabled;
    invalidate();
}

bool PackageProxyModel::isSortedByRelevancy() const
{
    return m_sortByRelevancy;
}

void PackageProxyModel::setGroupFilter(const QString &filterText)
{
    m_groupFilter = filterText;
    invalidate();
}

void PackageProxyModel::setStateFilter(QApt::Package::State state)
{
    m_stateFilter = state;
    invalidate();
}

void PackageProxyModel::setOriginFilter(const QString &origin)
{
    m_originFilter = origin;
    invalidate();
}

void PackageProxyModel::setArchFilter(const QString &arch)
{
    m_archFilter = arch;
    invalidate();
}

bool PackageProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    PackageModel *model = static_cast<PackageModel *>(sourceModel());
    QModelIndex index = model->index(sourceRow, 0, sourceParent);
    
    // Handle Virtual Packages
    if (model->isVirtualPackage(index)) {
        VirtualPackage vPkg = model->virtualPackageAt(index);
        
        if (!m_groupFilter.isEmpty()) {
            if (!vPkg.section().contains(m_groupFilter)) {
                return false;
            }
        }

        // Virtual packages currently don't support state filtering in the same way
        // But they are usually "Not Installed"
        if (m_stateFilter != 0) {
             // For now, if we filter by specific state, we might hide virtual packages
             // unless we map their state. Virtual packages are generally "not installed".
             // If filter includes "NotInstalled", show it.
             if (!(m_stateFilter & QApt::Package::NotInstalled)) {
                 return false;
             }
        }

        if (!m_originFilter.isEmpty()) {
            if (m_originFilter == "local") {
                // Always show in local filter
                return true;
            } else {
                if (vPkg.origin() != m_originFilter) {
                    return false;
                }
            }
        }

        if (!m_archFilter.isEmpty()) {
            if (vPkg.architecture() != m_archFilter) {
                return false;
            }
        }
        
        // Search filtering for virtual packages
        if (m_useSearchResults) {
            // Basic search implementation for virtual packages
            // Since m_searchPackages only contains QApt::Package*, we can't use it directly.
            // We rely on the fact that if we are using search results, we probably shouldn't show virtual packages
            // UNLESS we implement search for them too.
            // For now, let's hide them if search is active, OR implement a simple check.
            return false; // TODO: Implement search for virtual packages
        }

        return true;
    }

    // Handle APT Packages
    QApt::Package *package = model->packageAt(index);
    // We have a package as internal pointer
    if (!package) {
        return false;
    }

    if (!m_groupFilter.isEmpty()) {
        if (!QString(package->section()).contains(m_groupFilter)) {
            return false;
        }
    }

    if (m_stateFilter != 0) {
        if ((bool)(package->state() & m_stateFilter) == false) {
            return false;
        }
    }

    if (!m_originFilter.isEmpty()) {
        if (m_originFilter == "local") {
            // For "local" origin filter, show ONLY packages that are either:
            // 1. Installed locally (not from any repository)
            // 2. Have local .deb files available in configured directories
            LocalPackageManager *localManager = LocalPackageManager::instance();
            if (!localManager) {
                return false; 
            }
            
            // Check if this package is locally installed (not from repos)
            bool isLocalInstall = localManager->isLocalInstallPackage(package->name());
            
            // Check if this package has a local .deb file available
            bool hasLocalFile = localManager->hasLocalFile(package->name());
            
            // CRITICAL: Only show packages that are ACTUALLY local
            if (!isLocalInstall && !hasLocalFile) {
                return false;
            }
            // If we reach here, it's a local package - show it
        } else {
            // For other origins, use the standard origin check
            if (package->origin() != m_originFilter) {
                return false;
            }
        }
    }

    if (!m_archFilter.isEmpty()) {
        if (package->architecture() != m_archFilter) {
            return false;
        }
    }

    if (!MuonSettings::self()->showMultiArchDupes()) {
        if (package->isMultiArchDuplicate())
            return false;
    }

    if (m_useSearchResults)
        return m_searchPackages.contains(package);

    return true;
}

QApt::Package *PackageProxyModel::packageAt(const QModelIndex &index) const
{
    // Since our representation is almost bound to change, we need to grab the parent model's index
    QModelIndex sourceIndex = mapToSource(index);
    QApt::Package *package = static_cast<PackageModel *>(sourceModel())->packageAt(sourceIndex);
    return package;
}

bool PackageProxyModel::isVirtualPackage(const QModelIndex &index) const
{
    QModelIndex sourceIndex = mapToSource(index);
    return static_cast<PackageModel *>(sourceModel())->isVirtualPackage(sourceIndex);
}

VirtualPackage PackageProxyModel::virtualPackageAt(const QModelIndex &index) const
{
    QModelIndex sourceIndex = mapToSource(index);
    return static_cast<PackageModel *>(sourceModel())->virtualPackageAt(sourceIndex);
}

bool PackageProxyModel::isFlatpakPackage(const QModelIndex &index) const
{
    QModelIndex sourceIndex = mapToSource(index);
    return static_cast<PackageModel *>(sourceModel())->isFlatpakPackage(sourceIndex);
}

FlatpakPackage PackageProxyModel::flatpakPackageAt(const QModelIndex &index) const
{
    QModelIndex sourceIndex = mapToSource(index);
    return static_cast<PackageModel *>(sourceModel())->flatpakPackageAt(sourceIndex);
}

void PackageProxyModel::reset()
{
    beginRemoveRows(QModelIndex(), 0, m_packages.size());
    m_packages =  static_cast<PackageModel *>(sourceModel())->packages();
    endRemoveRows();
    invalidate();
}

bool PackageProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    PackageModel *model = static_cast<PackageModel *>(sourceModel());
    
    bool leftIsVirtual = model->isVirtualPackage(left);
    bool rightIsVirtual = model->isVirtualPackage(right);
    
    // If both are virtual
    if (leftIsVirtual && rightIsVirtual) {
        VirtualPackage leftPkg = model->virtualPackageAt(left);
        VirtualPackage rightPkg = model->virtualPackageAt(right);
        
        switch (left.column()) {
            case 0: // Name
                return leftPkg.name() < rightPkg.name();
            case 3: // Installed Size
                return leftPkg.installedSize() < rightPkg.installedSize();
            case 4: // Installed Version
                return false; // Neither installed
            case 5: // Available Version
                return QApt::Package::compareVersion(leftPkg.availableVersion(), rightPkg.availableVersion()) < 0;
            default:
                return false;
        }
    }
    
    // If one is virtual and other is not
    if (leftIsVirtual != rightIsVirtual) {
        // Put virtual packages at the end? Or beginning?
        // Let's put them at the end for now
        return !leftIsVirtual; 
    }

    // Both are APT packages
    QApt::Package *leftPackage = model->packageAt(left);
    QApt::Package *rightPackage = model->packageAt(right);
    
    if (!leftPackage || !rightPackage) {
        return false;
    }

    switch (left.column()) {
      case 0:
          if (m_sortByRelevancy) {
              // This is expensive for very large datasets. It takes about 3 seconds with 30,000 packages
              // The order in m_packages is based on relevancy when returned by m_backend->search()
              // Use this order to determine less than
              return (m_searchPackages.indexOf(leftPackage) > m_searchPackages.indexOf(rightPackage));
          } else {
              QString leftString = left.data(PackageModel::NameRole).toString();
              QString rightString = right.data(PackageModel::NameRole).toString();

              return leftString < rightString;
          }
      case 1:
          return packageStatusLessThan(leftPackage, rightPackage);
      case 2:
          return packageRequestedLessThan(leftPackage, rightPackage);
      case 3: /* Installed size */
          {
              qlonglong leftSize = left.data(PackageModel::InstalledSizeRole).toLongLong();
              qlonglong rightSize = right.data(PackageModel::InstalledSizeRole).toLongLong();
              return leftSize < rightSize;
          }
      case 4: /* Installed version */
          return QApt::Package::compareVersion(leftPackage->installedVersion(), rightPackage->installedVersion()) < 0;
      case 5: /* Available version */
          return QApt::Package::compareVersion(leftPackage->availableVersion(), rightPackage->availableVersion()) < 0;
    }

    return false;
}

QApt::PackageList PackageProxyModel::performSlowSearch(const QString &searchText) const
{
    QApt::PackageList results;
    const QApt::PackageList allPackages = static_cast<PackageModel *>(sourceModel())->packages();
    
    if (searchText.isEmpty()) {
        return results;
    }
    
    // Convert search text to lowercase for case-insensitive matching
    QString searchTextLower = searchText.toLower();
    
    for (QApt::Package *package : allPackages) {
        bool matches = false;
        
        // Search in package name
        if (QString(package->name()).toLower().contains(searchTextLower)) {
            matches = true;
        }
        // Search in short description
        else if (package->shortDescription().toLower().contains(searchTextLower)) {
            matches = true;
        }
        // Search in long description
        else if (package->longDescription().toLower().contains(searchTextLower)) {
            matches = true;
        }
        // Search in maintainer
        else if (package->maintainer().toLower().contains(searchTextLower)) {
            matches = true;
        }
        // Search in section
        else if (QString(package->section()).toLower().contains(searchTextLower)) {
            matches = true;
        }
        // Search in origin
        else if (package->origin().toLower().contains(searchTextLower)) {
            matches = true;
        }
        
        if (matches) {
            results.append(package);
        }
    }
    
    return results;
}
