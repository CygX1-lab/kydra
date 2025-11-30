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

#include "PackageModel.h"
#include "PackageIconExtractor.h"
#include "VirtualPackage.h"
#include "LocalPackageManager.h"

#include <QStringBuilder>
#include <QIcon>
#include <KLocalizedString>
#include <KFormat>

PackageModel::PackageModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_packages(QApt::PackageList())
    , m_virtualPackages(QList<VirtualPackage>())
{
    connect(LocalPackageManager::instance(), &LocalPackageManager::iconExtracted,
            this, &PackageModel::onIconExtracted);
            
    // Connect to FlatpakManager
    connect(FlatpakManager::instance(), &FlatpakManager::packagesChanged,
            this, &PackageModel::onFlatpaksChanged);
}

int PackageModel::rowCount(const QModelIndex & /*parent*/) const
{
    return m_packages.size() + m_virtualPackages.size() + m_flatpakPackages.size();
}

int PackageModel::columnCount(const QModelIndex & /*parent*/) const
{
    return 6;
}

QVariant PackageModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return false;
    }
    
    int row = index.row();
    
    // Determine if this is an APT package, virtual package, or Flatpak
    if (row < m_packages.size()) {
        // APT package
        QApt::Package *package = m_packages.at(row);
        switch (role) {
        case NameRole:
            if (package->isForeignArch()) {
                return QString(package->name() % QLatin1String(" (")
                        % package->architecture() % ')');
            }
            return package->name();
        case IconRole:
            return PackageIconExtractor::instance()->getPackageIcon(package);
        case DescriptionRole:
            return package->shortDescription();
        case StatusRole:
        case ActionRole:
            return package->state();
        case SupportRole:
            return package->isSupported();
        case InstalledSizeRole:
            return package->installedSize();
        case InstalledSizeDisplayRole:
            {
                qint64 size = package->installedSize();
                if (size != -1) {
                    return KFormat().formatByteSize(size);
                }
            }
            return QVariant();
        case InstalledVersionRole:
            return package->installedVersion();
        case AvailableVersionRole:
            return package->availableVersion();
        case IsLocalRole:
            return LocalPackageManager::instance()->isLocalInstallPackage(package->name());
        case Qt::ToolTipRole:
            return QVariant();
        }
    } else if (row < m_packages.size() + m_virtualPackages.size()) {
        // Virtual package
        int virtualIdx = row - m_packages.size();
        const VirtualPackage &vPkg = m_virtualPackages.at(virtualIdx);
        switch (role) {
        case NameRole:
            return vPkg.name();
        case IconRole:
            {
                QString iconPath = vPkg.iconPath();
                if (!iconPath.isEmpty()) {
                    return QIcon(iconPath);
                }
                return QIcon::fromTheme("package-x-generic"); // Generic package icon
            }
        case DescriptionRole:
            return vPkg.shortDescription();
        case StatusRole:
        case ActionRole:
            return 0; // Not installed state
        case SupportRole:
            return false; // Virtual packages don't have official support
        case InstalledSizeRole:
            return vPkg.installedSize();
        case InstalledSizeDisplayRole:
            {
                qint64 size = vPkg.installedSize();
                if (size != -1) {
                    return KFormat().formatByteSize(size);
                }
            }
            return QVariant();
        case InstalledVersionRole:
            return QString(); // Not installed
        case AvailableVersionRole:
            return vPkg.availableVersion();
        case IsLocalRole:
            return true;
        case Qt::ToolTipRole:
            return QString("Local package: %1").arg(vPkg.filename());
        }
    } else {
        // Flatpak package
        int flatpakIdx = row - m_packages.size() - m_virtualPackages.size();
        if (flatpakIdx >= 0 && flatpakIdx < m_flatpakPackages.size()) {
            const FlatpakPackage &fPkg = m_flatpakPackages.at(flatpakIdx);
            switch (role) {
            case NameRole:
                return fPkg.name;
            case IconRole:
                if (QIcon::hasThemeIcon(fPkg.id)) {
                    return QIcon::fromTheme(fPkg.id);
                }
                return QIcon::fromTheme("application-x-executable");
            case DescriptionRole:
                return fPkg.description;
            case StatusRole:
            case ActionRole:
                return 0; // TODO: Map Flatpak state
            case SupportRole:
                return true;
            case InstalledSizeRole:
                return 0; // Unknown size
            case InstalledSizeDisplayRole:
                return QString();
            case InstalledVersionRole:
                return fPkg.version;
            case AvailableVersionRole:
                return fPkg.version;
            case IsLocalRole:
                return false;
            case Qt::ToolTipRole:
                return QString("Flatpak: %1").arg(fPkg.id);
            }
        }
    }

    return QVariant();
}

QVariant PackageModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation);
    if (role == Qt::DisplayRole) {
        switch (section) {
        case 0:
            return QVariant(i18n("Package"));
        case 1:
            return QVariant(i18n("Status"));
        case 2:
            return QVariant(i18n("Requested"));
        case 3:
            return QVariant(i18n("Installed Size"));
        case 4:
            return QVariant(i18n("Installed Version"));
        case 5:
            return QVariant(i18n("Available Version"));
        }
    }
    return QVariant();
}

void PackageModel::setPackages(const QApt::PackageList &list)
{
    beginResetModel();
    m_packages = list;
    endResetModel();
}

void PackageModel::setVirtualPackages(const QList<VirtualPackage> &virtualPackages)
{
    beginResetModel();
    m_virtualPackages = virtualPackages;
    endResetModel();
}

void PackageModel::addVirtualPackages(const QList<VirtualPackage> &virtualPackages)
{
    int currentTotal = m_packages.size() + m_virtualPackages.size();
    int newCount = virtualPackages.size();
    
    beginInsertRows(QModelIndex(), currentTotal, currentTotal + newCount - 1);
    m_virtualPackages.append(virtualPackages);
    endInsertRows();
}

void PackageModel::clearVirtualPackages()
{
    if (m_virtualPackages.isEmpty()) {
        return;
    }
    
    int aptCount = m_packages.size();
    int virtualCount = m_virtualPackages.size();
    
    beginRemoveRows(QModelIndex(), aptCount, aptCount + virtualCount - 1);
    m_virtualPackages.clear();
    endRemoveRows();
}

void PackageModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, m_packages.size() + m_virtualPackages.size() + m_flatpakPackages.size() - 1);
    m_packages.clear();
    m_virtualPackages.clear();
    m_flatpakPackages.clear();
    endRemoveRows();
}

void PackageModel::externalDataChanged()
{
    // A package being changed means that any number of other packages can have
    // changed, so say everything changed to trigger refreshes.
    // A package being changed means that any number of other packages can have
    // changed, so say everything changed to trigger refreshes.
    int totalRows = m_packages.size() + m_virtualPackages.size() + m_flatpakPackages.size();
    if (totalRows > 0) {
        emit dataChanged(index(0, 0), index(totalRows - 1, 0));
    }
}

QApt::Package *PackageModel::packageAt(const QModelIndex &index) const
{
    int row = index.row();
    if (row >= 0 && row < m_packages.size()) {
        return m_packages.at(row);
    }
    return nullptr; // Virtual package or invalid
}

bool PackageModel::isVirtualPackage(const QModelIndex &index) const
{
    return index.row() >= m_packages.size();
}

VirtualPackage PackageModel::virtualPackageAt(const QModelIndex &index) const
{
    int virtualIdx = index.row() - m_packages.size();
    if (virtualIdx >= 0 && virtualIdx < m_virtualPackages.size()) {
        return m_virtualPackages.at(virtualIdx);
    }
    // Return empty VirtualPackage on error
    return VirtualPackage(LocalPackageInfo());
}

QApt::PackageList PackageModel::packages() const
{
    return m_packages;
}

void PackageModel::setFlatpakPackages(const QList<FlatpakPackage> &flatpakPackages)
{
    beginResetModel();
    m_flatpakPackages = flatpakPackages;
    endResetModel();
}

bool PackageModel::isFlatpakPackage(const QModelIndex &index) const
{
    return index.row() >= (m_packages.size() + m_virtualPackages.size());
}

FlatpakPackage PackageModel::flatpakPackageAt(const QModelIndex &index) const
{
    int flatpakIdx = index.row() - m_packages.size() - m_virtualPackages.size();
    if (flatpakIdx >= 0 && flatpakIdx < m_flatpakPackages.size()) {
        return m_flatpakPackages.at(flatpakIdx);
    }
    return FlatpakPackage();
}

void PackageModel::onFlatpaksChanged()
{
    setFlatpakPackages(FlatpakManager::instance()->listPackages());
}

void PackageModel::onIconExtracted(const QString &filePath, const QString & /*iconPath*/)
{
    // Find the virtual package with this file path and emit dataChanged
    for (int i = 0; i < m_virtualPackages.size(); ++i) {
        if (m_virtualPackages[i].filename() == filePath) {
            int row = m_packages.size() + i;
            QModelIndex idx = index(row, 0);
            emit dataChanged(idx, idx, QVector<int>() << IconRole);
            return;
        }
    }
}

#include "PackageModel.moc"
