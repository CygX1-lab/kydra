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

#include "LocalRepository.h"

#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QUrl>

#include <KLocalizedString>

QString LocalRepository::sourcesFilePath()
{
    return QStringLiteral("/etc/apt/sources.list.d/kydra-local.sources");
}

QString LocalRepository::aptFolder(const QString &folder, const QString &arch)
{
    // The same rule kydra-repo-index uses to decide which folders to index.
    const QDir dir(folder);
    const QString perArch = dir.filePath(arch);
    const QString chosen = QFileInfo(perArch).isDir() ? perArch : folder;
    return QFileInfo(chosen).canonicalFilePath();
}

QString LocalRepository::configuredAptFolder()
{
    QFile file(sourcesFilePath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString();
    }
    while (!file.atEnd()) {
        const QString line = QString::fromUtf8(file.readLine()).trimmed();
        if (line.startsWith(QLatin1String("URIs:"))) {
            return QUrl(line.mid(5).trimmed()).toLocalFile();
        }
    }
    return QString();
}

QString LocalRepository::folderAptCannotRead(const QString &dir)
{
    // apt reads a file: source as its own user, _apt, which is neither the
    // owner nor in the group of anything here - only the "other" bits count.
    const QFileInfo target(dir);
    if (!target.permission(QFileDevice::ReadOther) || !target.permission(QFileDevice::ExeOther)) {
        return target.absoluteFilePath();
    }
    QDir up(dir);
    while (up.cdUp()) {
        const QFileInfo info(up.absolutePath());
        if (!info.permission(QFileDevice::ExeOther)) {
            return info.absoluteFilePath();
        }
    }
    return QString();
}

QString LocalRepository::indexHelperPath()
{
    // A build tree has a copy beside the binary; an installed Kydra has it in
    // libexec.
    const QStringList dirs = {QCoreApplication::applicationDirPath(),
                              QStringLiteral(KYDRA_LIBEXECDIR)};
    for (const QString &dir : dirs) {
        const QFileInfo helper(dir + QStringLiteral("/kydra-repo-index"));
        if (helper.isFile() && helper.isExecutable()) {
            return helper.absoluteFilePath();
        }
    }
    return QString();
}

void LocalRepository::writeSourceFile(const QString &aptFolder, QObject *context,
                                      const std::function<void(const QString &error)> &done)
{
    // Encoded, so a folder with spaces in its name is still one URI to apt.
    const QString uri = QString::fromLatin1(QUrl::fromLocalFile(aptFolder).toEncoded());
    const QString contents = QStringLiteral(
        "# Written by Kydra (Settings > Set Up Local Repository).\n"
        "# The index is not signed: whatever is put in this folder is trusted.\n"
        "Types: deb\n"
        "URIs: %1\n"
        "Suites: ./\n"
        "Trusted: yes\n").arg(uri);

    // The QApt worker writes files as root once polkit agrees - the same
    // service and the same password prompt as refreshing and applying changes.
    QDBusMessage call = QDBusMessage::createMethodCall(
        QStringLiteral("org.kubuntu.qaptworker3"), QStringLiteral("/"),
        QStringLiteral("org.kubuntu.qaptworker3"), QStringLiteral("writeFileToDisk"));
    call << contents << sourcesFilePath();
    call.setInteractiveAuthorizationAllowed(true);

    // Long enough for someone to type a password into the prompt.
    const int timeoutMs = 5 * 60 * 1000;
    auto *watcher = new QDBusPendingCallWatcher(
        QDBusConnection::systemBus().asyncCall(call, timeoutMs), context);
    QObject::connect(watcher, &QDBusPendingCallWatcher::finished, context,
                     [done](QDBusPendingCallWatcher *finished) {
        const QDBusPendingReply<bool> reply = *finished;
        finished->deleteLater();
        if (reply.isError()) {
            done(reply.error().message());
        } else if (!reply.value()) {
            done(i18nc("@info", "Authorization was refused, or the file could not be written."));
        } else {
            done(QString());
        }
    });
}
