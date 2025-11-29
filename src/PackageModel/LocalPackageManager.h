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

#ifndef LOCALPACKAGEMANAGER_H
#define LOCALPACKAGEMANAGER_H

#include <QObject>
#include <QMap>
#include <QSet>
#include <QStringList>
#include <QFileSystemWatcher>
#include <QMutex>

namespace QApt {
    class Backend;
    class Package;
}

struct LocalPackageInfo {
    QString filename;
    QString packageName;
    QString version;
    QString architecture;
    QString maintainer;
    QString description;
    QString section;
    QStringList dependencies;
    QStringList conflicts;
    QStringList provides;
    QStringList replaces;
    QStringList suggests;
    QStringList recommends;
    QStringList enhances;
    QStringList preDepends;
    QStringList breaks;
    QString installedSize;
    QString downloadSize;
    QString homepage;
    QString priority;
    QString standardsVersion;
    QString source;
    QString origin;
    
    LocalPackageInfo() {}
};

class LocalPackageManager : public QObject
{
    Q_OBJECT
    
public:
    static LocalPackageManager *instance();
    static void setBackend(QApt::Backend *backend);
    
    explicit LocalPackageManager(QApt::Backend *backend, QObject *parent = nullptr);
    ~LocalPackageManager();
    
    void setLocalDebFolders(const QStringList &folders);
    QStringList localDebFolders() const;
    
    void scanLocalPackages();
    void detectLocalInstallPackages();
    
    QMap<QString, LocalPackageInfo> localPackages() const;
    QSet<QString> localInstallPackages() const;
    
    bool isLocalPackage(const QString &packageName) const;
    bool isLocalInstallPackage(const QString &packageName) const;
    bool hasLocalFile(const QString &packageName) const;
    
    LocalPackageInfo localPackageInfo(const QString &packageName) const;
    
    QStringList getLocalPackageFiles() const;
    
    // Get packages that exist in local folder but NOT in APT database
    QList<LocalPackageInfo> getVirtualPackages() const;
    
    bool parseDebFile(const QString &filePath, LocalPackageInfo &info);
    void addTemporaryPackage(const LocalPackageInfo &info);
    
signals:
    void localPackagesChanged();
    void localInstallPackagesDetected();
    void scanProgress(int current, int total);
    void scanFinished();
    
private slots:
    void onDirectoryChanged(const QString &path);
    
private:
    void scanDirectory(const QString &directory);
    void parseControlFile(const QString &controlData, LocalPackageInfo &info);
    QString extractField(const QString &data, const QString &field) const;
    bool isPackageFromLocalInstall(const QString &packageName);
    
    QApt::Backend *m_backend;
    QStringList m_localDebFolders;
    QMap<QString, LocalPackageInfo> m_localPackages;
    QSet<QString> m_localInstallPackages;
    QFileSystemWatcher *m_fileWatcher;
    mutable QMutex m_mutex;
    
    static const QString LOCAL_ORIGIN;
};

#endif // LOCALPACKAGEMANAGER_H