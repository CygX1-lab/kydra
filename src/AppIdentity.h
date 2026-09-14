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

#ifndef KYDRA_APPIDENTITY_H
#define KYDRA_APPIDENTITY_H

#include <KAboutData>
#include <QString>

/**
 * The one name Plasma knows Kydra by: its desktop file (org.kydra.app.desktop),
 * its icon (org.kydra.app) and the id of its window - the app id on Wayland,
 * _KDE_NET_WM_DESKTOP_FILE on X11. The three must agree, or the task manager
 * cannot tell that a running window belongs to Kydra's launcher and shows a
 * generic icon instead of Kydra's own (and of the icon theme's).
 */
namespace AppIdentity
{
/** The desktop file's name without ".desktop", as KAboutData wants it. */
inline QString desktopFileName()
{
    return QStringLiteral("org.kydra.app");
}

/** The icon the desktop file names and the package installs. */
inline QString iconName()
{
    return QStringLiteral("org.kydra.app");
}

/**
 * Names the application after its desktop file. Call it before
 * KAboutData::setApplicationData(), which hands the name on to Qt for the
 * window; left unset, KAboutData makes up "org.kde.kydra", which no
 * desktop file has.
 */
inline void applyTo(KAboutData &about)
{
    about.setDesktopFileName(desktopFileName());
}
} // namespace AppIdentity

#endif // KYDRA_APPIDENTITY_H
