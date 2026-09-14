/***************************************************************************
 *   Copyright © 2026 CygX1 <cygnx1@gmail.com>                              *
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

#ifndef LOCALREPOSITORY_H
#define LOCALREPOSITORY_H

#include <QString>

#include <functional>

class QObject;

/**
 * A folder of .deb files made into an apt repository, so that apt update and
 * apt upgrade see new versions dropped into it.
 *
 * The folder is indexed by the kydra-repo-index helper, running as the user.
 * apt is pointed at it by a sources file written as root through the QApt
 * worker, which asks for authorization the same way applying changes does.
 */
namespace LocalRepository
{
/// The sources file Kydra owns: /etc/apt/sources.list.d/kydra-local.sources
QString sourcesFilePath();

/// The folder apt reads on a machine of @p arch: <folder>/<arch> when the
/// folder has one subfolder per architecture, else the folder itself. Links
/// are resolved, so a folder reached through the home folder still works.
QString aptFolder(const QString &folder, const QString &arch);

/// The folder the sources file points apt at, or empty if there is none.
QString configuredAptFolder();

/// The first folder on the way to @p dir that apt's unprivileged user cannot
/// get through, or empty if it can read @p dir.
QString folderAptCannotRead(const QString &dir);

/// Path of the kydra-repo-index helper, or empty if it is not installed.
QString indexHelperPath();

/// The folder Refresh indexes before apt looks for updates: @p folder, the
/// local repository chosen in Kydra, once apt has been pointed at it
/// (@p configuredAptFolder is not empty) and while it is there; else empty,
/// and Refresh only asks apt - a share that is not mounted is apt's to report.
QString folderToIndexOnRefresh(const QString &folder, const QString &configuredAptFolder);

/// Writes the sources file pointing apt at @p aptFolder. @p done is called
/// with an empty string on success, else with the reason it failed.
void writeSourceFile(const QString &aptFolder, QObject *context,
                     const std::function<void(const QString &error)> &done);
}

#endif
