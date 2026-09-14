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

#include "AppIdentity.h"

#include <KAboutData>
#include <QFile>
#include <QGuiApplication>
#include <QTest>

// The window's id, the desktop file and the icon agreeing - what Plasma's task
// manager needs to show Kydra's own icon for the running window.
class AppIdentityTest : public QObject
{
    Q_OBJECT

private:
    static QString sourceFile(const char *dir, const QString &name)
    {
        return QString::fromUtf8(dir) + QLatin1Char('/') + name;
    }

    static QString desktopFile()
    {
        return sourceFile(KYDRA_DATADIR, AppIdentity::desktopFileName() + QStringLiteral(".desktop"));
    }

    /** The value of `key` in the desktop file's [Desktop Entry]. */
    static QString desktopEntry(const QString &key)
    {
        QFile file(desktopFile());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return QString();
        }
        const QString prefix = key + QLatin1Char('=');
        while (!file.atEnd()) {
            const QString line = QString::fromUtf8(file.readLine()).trimmed();
            if (line.startsWith(prefix)) {
                return line.mid(prefix.size());
            }
        }
        return QString();
    }

    static KAboutData kydraAboutData()
    {
        return KAboutData(QStringLiteral("kydra"), QStringLiteral("Kydra"), QStringLiteral("1"));
    }

private Q_SLOTS:
    void should_name_the_window_after_the_desktop_file_when_applied()
    {
        KAboutData about = kydraAboutData();
        AppIdentity::applyTo(about);
        KAboutData::setApplicationData(about);
        QCOMPARE(about.desktopFileName(), QStringLiteral("org.kydra.app"));
        // what Qt hands the compositor as the window's app id
        QCOMPARE(QGuiApplication::desktopFileName(), QStringLiteral("org.kydra.app"));
    }

    void should_not_match_the_desktop_file_by_default()
    {
        // The bug: left to itself, KAboutData names the window after a desktop
        // file that does not exist, and the task manager falls back.
        QVERIFY(kydraAboutData().desktopFileName() != AppIdentity::desktopFileName());
    }

    void should_ship_a_desktop_file_by_that_name_naming_that_icon()
    {
        QVERIFY2(QFile::exists(desktopFile()), qPrintable(desktopFile()));
        QCOMPARE(desktopEntry(QStringLiteral("Icon")), AppIdentity::iconName());
    }

    void should_name_the_same_desktop_file_in_the_metainfo()
    {
        QFile metainfo(sourceFile(KYDRA_DATADIR, QStringLiteral("org.kydra.app.metainfo.xml")));
        QVERIFY(metainfo.open(QIODevice::ReadOnly | QIODevice::Text));
        const QString launchable = QStringLiteral("<launchable type=\"desktop-id\">%1.desktop"
                                                  "</launchable>")
                                       .arg(AppIdentity::desktopFileName());
        QVERIFY(QString::fromUtf8(metainfo.readAll()).contains(launchable));
    }

    void should_install_the_icon_under_that_name()
    {
        const QString icon = sourceFile(KYDRA_ICONDIR, AppIdentity::iconName() + QStringLiteral(".svg"));
        QVERIFY2(QFile::exists(icon), qPrintable(icon));
    }
};

QTEST_MAIN(AppIdentityTest)

#include "AppIdentityTest.moc"
