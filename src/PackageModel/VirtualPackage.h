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

#ifndef VIRTUALPACKAGE_H
#define VIRTUALPACKAGE_H

#include "LocalPackageManager.h"
#include <QString>

/**
 * @brief Wrapper class for local .deb packages that don't exist in APT database
 * 
 * This class provides a package-like interface for .deb files that are only
 * available locally and not in any configured repository. It wraps LocalPackageInfo
 * and provides methods compatible with the package display system.
 */
class VirtualPackage
{
public:
    VirtualPackage();
    explicit VirtualPackage(const LocalPackageInfo &info);
    
    // Package identification
    QString name() const;
    QString version() const;
    QString architecture() const;
    QString origin() const;
    QString section() const;
    
    // Package description
    QString shortDescription() const;
    QString longDescription() const;
    QString description() const;
    
    // Package metadata
    QString maintainer() const;
    QString homepage() const;
    qint64 installedSize() const;
    QString filename() const;
    QString iconPath() const;
    
    // Dependencies
    QStringList dependencies() const;
    QStringList recommends() const;
    QStringList suggests() const;
    QStringList conflicts() const;
    
    // State information (virtual packages are always uninstalled)
    bool isInstalled() const { return false; }
    bool isVirtual() const { return true; }
    QString installedVersion() const { return QString(); }
    QString availableVersion() const;
    
    // Get the underlying LocalPackageInfo
    const LocalPackageInfo& packageInfo() const { return m_info; }
    
private:
    LocalPackageInfo m_info;
};

#endif // VIRTUALPACKAGE_H
