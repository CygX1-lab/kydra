/***************************************************************************
 *   Copyright © 2010-2013 Jonathan Thomas <echidnaman@kubuntu.org>        *
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

#include "MainWindow.h"

#include <QApplication>
#include <KAboutData>
#include "../KydraVersion.h"
#include <KDBusService>
#include <KLocalizedString>
#include <QSessionManager>
#include <QCommandLineParser>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon::fromTheme("kydra"));
    KLocalizedString::setApplicationDomain("kydra");
    KAboutData about("kydra", i18n("Kydra Package Manager"), version, i18n("A modern KDE-native package manager"),
                     KAboutLicense::GPL, i18n("© 2024 CygX1"));
    about.addAuthor(i18n("CygX1"), i18n("Current Maintainer"), "cygnx1@gmail.com");
    about.addCredit(i18n("Jonathan Thomas"), i18n("Original Muon Author"), "echidnaman@kubuntu.org");
    about.addCredit(i18n("Harald Sitter"), i18n("Top Gardener"), "sitter@kde.org", QString(), QStringLiteral("apachelogger"));
    about.setProductName("kydra/kydra");
    about.setBugAddress("cygnx1@gmail.com");
    KAboutData::setApplicationData(about);

    {
        QCommandLineParser parser;
        about.setupCommandLine(&parser);
        parser.process(app);
        about.processCommandLine(&parser);
    }


    KDBusService service(KDBusService::Unique);

    auto disableSessionManagement = [](QSessionManager &sm) {
        sm.setRestartHint(QSessionManager::RestartNever);
    };
    QObject::connect(&app, &QGuiApplication::commitDataRequest, disableSessionManagement);
    QObject::connect(&app, &QGuiApplication::saveStateRequest, disableSessionManagement);

    MainWindow *mainWindow = new MainWindow;
    mainWindow->show();

    return app.exec();
}
