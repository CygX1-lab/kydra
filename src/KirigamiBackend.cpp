/***************************************************************************
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

#include "KirigamiBackend.h"
#include "MuonSettings.h"
#include "PackageModel/PackageIconExtractor.h"

#include <QApt/Backend>
#include <QApt/Package>
#include <QIcon>

#include <KLocalizedString>

PackageListModel::PackageListModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_backend(nullptr)
{
}

PackageListModel::~PackageListModel() = default;

void PackageListModel::setBackend(QApt::Backend *backend)
{
    beginResetModel();
    m_backend = backend;
    endResetModel();
}

void PackageListModel::setPackages(const QVector<QApt::Package*> &packages)
{
    beginResetModel();
    m_packages = packages;
    endResetModel();
    emit countChanged();
}

int PackageListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_packages.count();
}

QVariant PackageListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_packages.count())
        return QVariant();

    QApt::Package *package = m_packages.at(index.row());
    if (!package)
        return QVariant();

    switch (role) {
    case NameRole:
        return package->name();
    case DescriptionRole:
        return package->shortDescription();
    case VersionRole:
        return package->version();
    case IconRole:
        return PackageIconExtractor::instance()->getPackageIcon(package);
    case IsInstalledRole:
        return package->isInstalled();
    case PackageIdRole:
        return package->name();
    case CurrentVersionRole:
        return package->installedVersion();
    case NewVersionRole:
        return package->availableVersion();
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> PackageListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[DescriptionRole] = "description";
    roles[VersionRole] = "version";
    roles[IconRole] = "icon";
    roles[IsInstalledRole] = "isInstalled";
    roles[PackageIdRole] = "packageId";
    roles[CurrentVersionRole] = "currentVersion";
    roles[NewVersionRole] = "newVersion";
    return roles;
}

void PackageListModel::refresh()
{
    if (!m_backend)
        return;

    beginResetModel();
    m_packages.clear();
    
    // Get all packages from backend
    const auto packages = m_backend->availablePackages();
    for (QApt::Package *package : packages) {
        m_packages.append(package);
    }
    
    endResetModel();
    emit countChanged();
}

void PackageListModel::search(const QString &searchTerm)
{
    if (!m_backend || searchTerm.isEmpty()) {
        refresh();
        return;
    }

    beginResetModel();
    m_packages.clear();
    
    // Search packages
    const auto packages = m_backend->availablePackages();
    for (QApt::Package *package : packages) {
        if (package->name().contains(searchTerm, Qt::CaseInsensitive) ||
            package->shortDescription().contains(searchTerm, Qt::CaseInsensitive)) {
            m_packages.append(package);
        }
    }
    
    endResetModel();
    emit countChanged();
}

KirigamiBackend::KirigamiBackend::KirigamiBackend(QObject *parent)
    : QObject(parent)
    , m_backend(nullptr)
    , m_availablePackages(new PackageListModel(this))
    , m_updates(new PackageListModel(this))
    , m_installedPackages(new PackageListModel(this))
    , m_kirigamiMode(false)
    , m_selectedPackage(nullptr)
{
    m_availablePackages->setBackend(m_backend);
    m_updates->setBackend(m_backend);
    m_installedPackages->setBackend(m_backend);
}

KirigamiBackend::~KirigamiBackend() = default;

void KirigamiBackend::setBackend(QApt::Backend *backend)
{
    m_backend = backend;
    m_availablePackages->setBackend(backend);
    m_updates->setBackend(backend);
    m_installedPackages->setBackend(backend);
    
    // Refresh models
    // Refresh models will be called when packages are loaded
}

PackageListModel *KirigamiBackend::availablePackages() const
{
    return m_availablePackages;
}

PackageListModel *KirigamiBackend::updatesPackages() const
{
    return m_updates;
}

PackageListModel *KirigamiBackend::installedPackages() const
{
    return m_installedPackages;
}

bool KirigamiBackend::kirigamiMode() const
{
    return m_kirigamiMode;
}

void KirigamiBackend::setKirigamiMode(bool enabled)
{
    if (m_kirigamiMode != enabled) {
        m_kirigamiMode = enabled;
        emit kirigamiModeChanged();
    }
}

void KirigamiBackend::togglePackage(const QString &packageId)
{
    if (!m_backend)
        return;

    QApt::Package *package = m_backend->package(packageId);
    if (!package)
        return;

    // For now, just emit a signal to indicate the package was toggled
    // The actual marking logic should be handled by the main application
    emit packageToggled(packageId, package->isInstalled());
}

void KirigamiBackend::updatePackage(const QString &packageId)
{
    if (!m_backend)
        return;

    QApt::Package *package = m_backend->package(packageId);
    if (!package || !package->isInstalled())
        return;

    // For now, just emit a signal to indicate the package was selected for update
    // The actual marking logic should be handled by the main application
    emit packageToggled(packageId, true);
}

void KirigamiBackend::installPackage(const QString &packageId)
{
    if (!m_backend)
        return;

    QApt::Package *package = m_backend->package(packageId);
    if (!package || package->isInstalled())
        return;

    // For now, just emit a signal to indicate the package was selected for install
    // The actual marking logic should be handled by the main application
    emit packageToggled(packageId, false);
}

void KirigamiBackend::removePackage(const QString &packageId)
{
    if (!m_backend)
        return;

    QApt::Package *package = m_backend->package(packageId);
    if (!package || !package->isInstalled())
        return;

    // For now, just emit a signal to indicate the package was selected for removal
    // The actual marking logic should be handled by the main application
    emit packageToggled(packageId, true);
}

QObject* KirigamiBackend::selectedPackage() const
{
    // Since QApt::Package doesn't inherit from QObject, we need to create a wrapper
    // For now, return nullptr and we'll implement a proper wrapper later
    return nullptr;
}

void KirigamiBackend::selectPackage(const QString &packageId)
{
    if (!m_backend)
        return;

    QApt::Package *package = m_backend->package(packageId);
    if (m_selectedPackage != package) {
        m_selectedPackage = package;
        emit selectedPackageChanged();
    }
}

void KirigamiBackend::searchPackages(const QString &searchTerm)
{
    m_availablePackages->search(searchTerm);
}

void KirigamiBackend::filterByCategory(const QString &category)
{
    if (!m_backend || category == "all") {
        m_availablePackages->refresh();
        return;
    }

    // Filter packages by category
    QVector<QApt::Package*> filteredPackages;
    const auto packages = m_backend->availablePackages();
    
    for (QApt::Package *package : packages) {
        QString packageSection = QString(package->section()).toLower();
        QString packageCategory = categorizePackage(packageSection);
        
        if (packageCategory == category.toLower()) {
            filteredPackages.append(package);
        }
    }
    
    m_availablePackages->setPackages(filteredPackages);
}

QString KirigamiBackend::categorizePackage(const QString &section) const
{
    // Map package sections to categories
    if (section.contains("games") || section.contains("amusement")) {
        return "applications";
    } else if (section.contains("devel") || section.contains("lib")) {
        return "development";
    } else if (section.contains("graphics") || section.contains("image")) {
        return "graphics";
    } else if (section.contains("sound") || section.contains("video") || section.contains("media")) {
        return "multimedia";
    } else if (section.contains("admin") || section.contains("utils") || section.contains("system")) {
        return "system";
    } else if (section.contains("web") || section.contains("net")) {
        return "applications";
    } else if (section.contains("editors") || section.contains("text")) {
        return "applications";
    } else if (section.contains("science") || section.contains("math")) {
        return "applications";
    }
    
    return "applications"; // Default category
}

void KirigamiBackend::previewChanges()
{
    emit showPreviewDialog();
}

void KirigamiBackend::applyChanges()
{
    emit showTransactionDialog();
}

void KirigamiBackend::checkForUpdates()
{
    if (!m_backend)
        return;

    // This would trigger the update check
    emit showTransactionDialog();
}

void KirigamiBackend::refreshModels()
{
    if (!m_backend)
        return;

    // Refresh available packages
    m_availablePackages->refresh();
    
    // Refresh updates
    QVector<QApt::Package*> updates;
    const auto packages = m_backend->availablePackages();
    for (QApt::Package *package : packages) {
        if (package->state() & QApt::Package::Upgradeable) {
            updates.append(package);
        }
    }
    m_updates->setPackages(updates);
    
    // Refresh installed packages
    QVector<QApt::Package*> installed;
    for (QApt::Package *package : packages) {
        if (package->isInstalled()) {
            installed.append(package);
        }
    }
    m_installedPackages->setPackages(installed);
}