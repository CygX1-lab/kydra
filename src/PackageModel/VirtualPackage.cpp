/***************************************************************************
 *   Copyright © 2025 Kydra Project                                          *
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

#include "VirtualPackage.h"

VirtualPackage::VirtualPackage()
{
}

VirtualPackage::VirtualPackage(const LocalPackageInfo &info)
    : m_info(info)
{
}

QString VirtualPackage::name() const
{
    return m_info.packageName;
}

QString VirtualPackage::version() const
{
    return m_info.version;
}

QString VirtualPackage::architecture() const
{
    return m_info.architecture;
}

QString VirtualPackage::origin() const
{
    return m_info.origin; // Should be "local"
}

QString VirtualPackage::section() const
{
    return m_info.section;
}

QString VirtualPackage::shortDescription() const
{
    // Extract first line of description
    QString desc = m_info.description;
    int newlinePos = desc.indexOf('\n');
    if (newlinePos > 0) {
        return desc.left(newlinePos);
    }
    return desc;
}

QString VirtualPackage::longDescription() const
{
    // Extract everything after first line
    QString desc = m_info.description;
    int newlinePos = desc.indexOf('\n');
    if (newlinePos > 0 && newlinePos < desc.length() - 1) {
        return desc.mid(newlinePos + 1);
    }
    return QString();
}

QString VirtualPackage::description() const
{
    return m_info.description;
}

QString VirtualPackage::maintainer() const
{
    return m_info.maintainer;
}

QString VirtualPackage::homepage() const
{
    return m_info.homepage;
}

qint64 VirtualPackage::installedSize() const
{
    // Parse installedSize string to qint64
    // Format is typically a number (in KB)
    bool ok;
    qint64 size = m_info.installedSize.toLongLong(&ok);
    if (ok) {
        return size * 1024; // Convert KB to bytes
    }
    return -1;
}

QString VirtualPackage::filename() const
{
    return m_info.filename;
}

QString VirtualPackage::iconPath() const
{
    LocalPackageManager *manager = LocalPackageManager::instance();
    if (manager) {
        return manager->getPackageIcon(m_info.filename);
    }
    return QString();
}

QStringList VirtualPackage::dependencies() const
{
    return m_info.dependencies;
}

QStringList VirtualPackage::recommends() const
{
    return m_info.recommends;
}

QStringList VirtualPackage::suggests() const
{
    return m_info.suggests;
}

QStringList VirtualPackage::conflicts() const
{
    return m_info.conflicts;
}

QString VirtualPackage::availableVersion() const
{
    return m_info.version;
}
