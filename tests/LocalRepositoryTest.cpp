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

#include "DebFileArgument.h"
#include "LocalRepository.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTemporaryDir>
#include <QTest>
#include <QUrl>

class LocalRepositoryTest : public QObject
{
    Q_OBJECT

private:
    QTemporaryDir m_tmp;

    QString path(const QString &relative) const { return m_tmp.filePath(relative); }

    static QString canonical(const QString &p) { return QFileInfo(p).canonicalFilePath(); }

    static QFile::Permissions ownerOnly()
    {
        return QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner;
    }

    static QFile::Permissions openToAll()
    {
        return ownerOnly() | QFile::ReadGroup | QFile::ExeGroup | QFile::ReadOther
            | QFile::ExeOther;
    }

    static void touch(const QString &p)
    {
        QFile file(p);
        QVERIFY(file.open(QIODevice::WriteOnly));
    }

    static void setMode(const QString &p, QFile::Permissions permissions)
    {
        QVERIFY(QFile::setPermissions(p, permissions));
    }

private Q_SLOTS:
    void initTestCase()
    {
        QVERIFY(m_tmp.isValid());
        // QTemporaryDir makes itself private; the tests decide who may enter.
        setMode(m_tmp.path(), openToAll());
    }

    void should_point_apt_at_the_arch_subfolder_when_the_folder_has_one()
    {
        QVERIFY(QDir().mkpath(path("store/arm64")));
        QVERIFY(QDir().mkpath(path("store/amd64")));
        QCOMPARE(LocalRepository::aptFolder(path("store"), QStringLiteral("arm64")),
                 canonical(path("store/arm64")));
        QCOMPARE(LocalRepository::aptFolder(path("store"), QStringLiteral("amd64")),
                 canonical(path("store/amd64")));
    }

    void should_point_apt_at_the_folder_itself_when_it_has_no_arch_subfolder()
    {
        QVERIFY(QDir().mkpath(path("flat")));
        QCOMPARE(LocalRepository::aptFolder(path("flat"), QStringLiteral("arm64")),
                 canonical(path("flat")));
    }

    void should_resolve_links_when_the_folder_is_reached_through_one()
    {
        // As ~/Documents is a link to a network share: apt is given the share.
        QVERIFY(QDir().mkpath(path("share/debs/arm64")));
        QVERIFY(QFile::link(path("share"), path("link-to-share")));
        QCOMPARE(LocalRepository::aptFolder(path("link-to-share/debs"), QStringLiteral("arm64")),
                 canonical(path("share/debs/arm64")));
    }

    void should_name_the_blocking_folder_when_apt_cannot_get_through()
    {
        QVERIFY(QDir().mkpath(path("private/debs")));
        setMode(path("private/debs"), openToAll());
        setMode(path("private"), ownerOnly());
        const QString blocked = LocalRepository::folderAptCannotRead(path("private/debs"));
        setMode(path("private"), openToAll());
        QCOMPARE(blocked, path("private"));
    }

    void should_name_the_folder_itself_when_apt_cannot_list_it()
    {
        QVERIFY(QDir().mkpath(path("unlisted")));
        setMode(path("unlisted"), ownerOnly() | QFile::ExeOther);
        const QString blocked = LocalRepository::folderAptCannotRead(path("unlisted"));
        setMode(path("unlisted"), openToAll());
        QCOMPARE(blocked, path("unlisted"));
    }

    void should_find_nothing_blocking_inside_when_every_folder_is_open()
    {
        QVERIFY(QDir().mkpath(path("public/debs")));
        setMode(path("public"), openToAll());
        setMode(path("public/debs"), openToAll());
        // Folders above the temporary one belong to whoever runs the test, so
        // only the verdict on the tree this test made is checked.
        const QString blocked = LocalRepository::folderAptCannotRead(path("public/debs"));
        QVERIFY2(!blocked.startsWith(m_tmp.path()), qPrintable(blocked));
    }

    void should_index_the_local_repository_on_refresh_when_one_is_set_up()
    {
        QVERIFY(QDir().mkpath(path("repo")));
        QCOMPARE(LocalRepository::folderToIndexOnRefresh(path("repo"), canonical(path("repo"))),
                 path("repo"));
    }

    void should_only_ask_apt_on_refresh_when_no_local_repository_is_set_up()
    {
        QVERIFY(QDir().mkpath(path("chosen")));
        QCOMPARE(LocalRepository::folderToIndexOnRefresh(QString(), QString()), QString());
        // chosen in Kydra, but apt not pointed at it (yet)
        QCOMPARE(LocalRepository::folderToIndexOnRefresh(path("chosen"), QString()), QString());
    }

    void should_only_ask_apt_on_refresh_when_the_folder_is_not_there()
    {
        // a share that is not mounted: apt update says so, and Refresh goes on
        QCOMPARE(LocalRepository::folderToIndexOnRefresh(path("unmounted"), path("unmounted")),
                 QString());
    }

    void should_accept_a_deb_path_relative_to_the_working_directory()
    {
        touch(path("hello_1.0_all.deb"));
        QCOMPARE(debFileFromArguments({QStringLiteral("hello_1.0_all.deb")}, m_tmp.path()),
                 path("hello_1.0_all.deb"));
    }

    void should_accept_a_file_url()
    {
        touch(path("url_1.0_all.deb"));
        const QString url = QUrl::fromLocalFile(path("url_1.0_all.deb")).toString();
        QCOMPARE(debFileFromArguments({url}, QDir::rootPath()), path("url_1.0_all.deb"));
    }

    void should_ignore_arguments_that_are_not_local_deb_files()
    {
        touch(path("notes.txt"));
        const QStringList others = {QStringLiteral("notes.txt"),
                                    QStringLiteral("missing_1.0_all.deb"),
                                    QStringLiteral("https://example.invalid/remote_1.0_all.deb")};
        QCOMPARE(debFileFromArguments(others, m_tmp.path()), QString());
    }

    void should_pick_the_deb_when_other_arguments_come_first()
    {
        touch(path("picked_1.0_all.deb"));
        const QStringList arguments = {QStringLiteral("notes.txt"),
                                       QStringLiteral("picked_1.0_all.deb")};
        QCOMPARE(debFileFromArguments(arguments, m_tmp.path()), path("picked_1.0_all.deb"));
    }
};

QTEST_GUILESS_MAIN(LocalRepositoryTest)

#include "LocalRepositoryTest.moc"
