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

#include "LocalPackageManager.h"

// Qt includes
#include <QDir>
#include <QFile>
#include <QProcess>
#include <QTextStream>
#include <QDebug>
#include <QTemporaryDir>
#include <QFileInfo>
#include <QtConcurrent>
#include <QRegularExpression>

// QApt includes
#include <QApt/Backend>
#include <QApt/Package>

const QString LocalPackageManager::LOCAL_ORIGIN = "local";

static LocalPackageManager *s_instance = nullptr;
static QApt::Backend *s_backend = nullptr;

LocalPackageManager *LocalPackageManager::instance()
{
    if (!s_instance) {
        s_instance = new LocalPackageManager(s_backend, nullptr);
    }
    return s_instance;
}

void LocalPackageManager::setBackend(QApt::Backend *backend)
{
    s_backend = backend;
    if (s_instance) {
        s_instance->m_backend = backend;
    }
}

LocalPackageManager::LocalPackageManager(QApt::Backend *backend, QObject *parent)
    : QObject(parent)
    , m_backend(backend)
    , m_fileWatcher(new QFileSystemWatcher(this))
{
    connect(m_fileWatcher, &QFileSystemWatcher::directoryChanged,
            this, &LocalPackageManager::onDirectoryChanged);
}

LocalPackageManager::~LocalPackageManager()
{
    s_instance = nullptr;
}

void LocalPackageManager::setLocalDebFolders(const QStringList &folders)
{
    // Remove old directories from watcher
    if (!m_localDebFolders.isEmpty()) {
        m_fileWatcher->removePaths(m_localDebFolders);
    }
    
    m_localDebFolders = folders;
    m_localDebFolders.removeAll(QString()); // Filter out empty paths
    
    // Add new directories to watcher
    if (!m_localDebFolders.isEmpty()) {
        m_fileWatcher->addPaths(m_localDebFolders);
    }
    
    // Rescan packages
    scanLocalPackages();
}

QStringList LocalPackageManager::localDebFolders() const
{
    return m_localDebFolders;
}

void LocalPackageManager::scanLocalPackages()
{
    // Use QtConcurrent to run scanning in background thread
    QtConcurrent::run([this]() {
        qDebug() << "Scanning local packages in folders:" << m_localDebFolders;
        m_localPackages.clear();
        
        int total = 0;
        int current = 0;
        
        // Count total .deb files first
        for (const QString &folder : m_localDebFolders) {
            QDir dir(folder);
            if (dir.exists()) {
                dir.setNameFilters(QStringList() << "*.deb");
                total += dir.entryList(QDir::Files).size();
            }
        }
        
        emit scanProgress(0, total);
        
        // Scan each directory
        for (const QString &folder : m_localDebFolders) {
            scanDirectory(folder);
            current += QDir(folder).entryList(QStringList() << "*.deb", QDir::Files).size();
            emit scanProgress(current, total);
        }
        
        emit scanFinished();
        emit localPackagesChanged();
    });
}


void LocalPackageManager::detectLocalInstallPackages()
{
    // Use QtConcurrent to run detection in background thread
    QtConcurrent::run([this]() {
        {
            QMutexLocker locker(&m_mutex);
            m_localInstallPackages.clear();
        }
        
        qDebug() << "Detecting local packages using 'apt list --installed'...";
        
        QProcess process;
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        env.insert("LC_ALL", "C");
        process.setProcessEnvironment(env);
        
        // Use apt list to find local packages efficiently
        // -o APT::Color::Mode=never ensures no color codes
        process.start("apt", QStringList() << "list" << "--installed" << "-o" << "APT::Color::Mode=never");
        
        if (!process.waitForFinished(60000)) { // 60s timeout
            qWarning() << "Timeout running apt list";
            emit localInstallPackagesDetected();
            return;
        }
        
        if (process.exitCode() != 0) {
            qWarning() << "apt list failed:" << process.readAllStandardError();
            emit localInstallPackagesDetected();
            return;
        }
        
        QTextStream stream(&process);
        while (!stream.atEnd()) {
            QString line = stream.readLine();
            if (line.contains("[installed,local]")) {
                // Format: package/now version arch [installed,local]
                QString packageName = line.section('/', 0, 0);
                if (!packageName.isEmpty()) {
                    QMutexLocker locker(&m_mutex);
                    m_localInstallPackages.insert(packageName);
                    // qDebug() << "Found local package:" << packageName;
                }
            }
        }
        
        {
            QMutexLocker locker(&m_mutex);
            qDebug() << "Detected" << m_localInstallPackages.size() << "local packages.";
        }
        emit localInstallPackagesDetected();
    });
}

void LocalPackageManager::scanDirectory(const QString &directory)
{
    QDir dir(directory);
    if (!dir.exists()) {
        return;
    }
    
    dir.setNameFilters(QStringList() << "*.deb");
    dir.setFilter(QDir::Files);
    
    QFileInfoList files;
    files = dir.entryInfoList();
    if (files.isEmpty() && dir.exists()) {
        qWarning() << "No .deb files found in directory" << directory;
    }
    
    for (const QFileInfo &fileInfo : files) {
        if (!fileInfo.exists() || !fileInfo.isReadable()) {
            continue;
        }
        
        LocalPackageInfo info;
        if (parseDebFile(fileInfo.absoluteFilePath(), info)) {
            info.filename = fileInfo.absoluteFilePath();
            
            QMutexLocker locker(&m_mutex);
            m_localPackages[info.packageName] = info;
        }
    }
}

bool LocalPackageManager::parseDebFile(const QString &filePath, LocalPackageInfo &info)
{
    // Check if file exists and is readable
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists() || !fileInfo.isReadable()) {
        qWarning() << "Cannot read .deb file:" << filePath;
        return false;
    }
    
    // Use dpkg-deb to extract control information
    QProcess process;
    process.start("dpkg-deb", QStringList() << "--info" << filePath);
    
    if (!process.waitForFinished(30000)) { // Increased to 30 second timeout
        qWarning() << "Timeout parsing .deb file:" << filePath;
        process.terminate();
        if (!process.waitForFinished(1000)) {
            process.kill();
        }
        return false;
    }
    
    if (process.exitCode() != 0) {
        QString errorOutput = process.readAllStandardError();
        qWarning() << "Failed to parse .deb file:" << filePath
                   << "Exit code:" << process.exitCode()
                   << "Error:" << errorOutput;
        return false;
    }
    
    QString controlData = process.readAllStandardOutput();
    parseControlFile(controlData, info);
    
    return !info.packageName.isEmpty();
}

void LocalPackageManager::addTemporaryPackage(const LocalPackageInfo &info)
{
    qDebug() << "Adding temporary local package:" << info.packageName << "from" << info.filename;
    
    // Add to local packages map
    QMutexLocker locker(&m_mutex);
    m_localPackages[info.packageName] = info;
    locker.unlock();
    
    // Emit signal to notify that local packages have changed
    emit localPackagesChanged();
}

void LocalPackageManager::parseControlFile(const QString &controlData, LocalPackageInfo &info)
{
    QStringList lines = controlData.split('\n');
    
    for (const QString &rawLine : lines) {
        QString line = rawLine.trimmed();
        if (line.startsWith("Package:")) {
            info.packageName = extractField(controlData, "Package");
        } else if (line.startsWith("Version:")) {
            info.version = extractField(controlData, "Version");
        } else if (line.startsWith("Architecture:")) {
            info.architecture = extractField(controlData, "Architecture");
        } else if (line.startsWith("Maintainer:")) {
            info.maintainer = extractField(controlData, "Maintainer");
        } else if (line.startsWith("Description:")) {
            info.description = extractField(controlData, "Description");
        } else if (line.startsWith("Section:")) {
            info.section = extractField(controlData, "Section");
        } else if (line.startsWith("Depends:")) {
            info.dependencies = extractField(controlData, "Depends").split(", ");
        } else if (line.startsWith("Conflicts:")) {
            info.conflicts = extractField(controlData, "Conflicts").split(", ");
        } else if (line.startsWith("Provides:")) {
            info.provides = extractField(controlData, "Provides").split(", ");
        } else if (line.startsWith("Replaces:")) {
            info.replaces = extractField(controlData, "Replaces").split(", ");
        } else if (line.startsWith("Suggests:")) {
            info.suggests = extractField(controlData, "Suggests").split(", ");
        } else if (line.startsWith("Recommends:")) {
            info.recommends = extractField(controlData, "Recommends").split(", ");
        } else if (line.startsWith("Enhances:")) {
            info.enhances = extractField(controlData, "Enhances").split(", ");
        } else if (line.startsWith("Pre-Depends:")) {
            info.preDepends = extractField(controlData, "Pre-Depends").split(", ");
        } else if (line.startsWith("Breaks:")) {
            info.breaks = extractField(controlData, "Breaks").split(", ");
        } else if (line.startsWith("Installed-Size:")) {
            info.installedSize = extractField(controlData, "Installed-Size");
        } else if (line.startsWith("Size:")) {
            info.downloadSize = extractField(controlData, "Size");
        } else if (line.startsWith("Homepage:")) {
            info.homepage = extractField(controlData, "Homepage");
        } else if (line.startsWith("Priority:")) {
            info.priority = extractField(controlData, "Priority");
        } else if (line.startsWith("Standards-Version:")) {
            info.standardsVersion = extractField(controlData, "Standards-Version");
        } else if (line.startsWith("Source:")) {
            info.source = extractField(controlData, "Source");
        }
    }
    
    info.origin = LOCAL_ORIGIN;
}

QString LocalPackageManager::extractField(const QString &data, const QString &field) const
{
    // dpkg-deb output is indented, so we allow leading whitespace
    QRegularExpression regex(QString("^\\s*%1:\\s*(.*)$").arg(field), QRegularExpression::MultilineOption);
    QRegularExpressionMatch match = regex.match(data);
    if (match.hasMatch()) {
        return match.captured(1).trimmed();
    }
    return QString();
}

bool LocalPackageManager::isPackageFromLocalInstall(const QString &packageName)
{
    // Check if package exists in any repository
    QProcess process;
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("LC_ALL", "C");
    process.setProcessEnvironment(env);
    process.start("apt-cache", QStringList() << "policy" << packageName);
    
    if (!process.waitForFinished(10000)) { // 10 second timeout
        return false;
    }
    
    if (process.exitCode() != 0) {
        return false;
    }
    
    QString output = process.readAllStandardOutput();
    QStringList lines = output.split('\n');
    QString installedVersion;
    
    // 1. Find installed version
    for (const QString &line : lines) {
        if (line.trimmed().startsWith("Installed:")) {
            installedVersion = line.section(':', 1).trimmed();
            break;
        }
    }
    
    if (installedVersion.isEmpty() || installedVersion == "(none)") {
        return false;
    }
    
    // 2. Check if the installed version comes from a repository
    bool inVersionTable = false;
    bool checkingInstalledVersion = false;
    bool hasRepoSource = false;
    
    for (const QString &line : lines) {
        if (line.trimmed().startsWith("Version table:")) {
            inVersionTable = true;
            continue;
        }
        
        if (inVersionTable) {
            QString trimmed = line.trimmed();
            
            // Check for version line
            if (trimmed.startsWith("***")) {
                // This is the currently installed version block
                QStringList parts = trimmed.split(QRegularExpression("\\s+"));
                if (parts.size() >= 2 && parts[1] == installedVersion) {
                    checkingInstalledVersion = true;
                    continue;
                } else {
                    checkingInstalledVersion = false;
                }
            } else if (!line.startsWith("      ")) { 
                // New version entry (less indented than sources)
                // Standard version lines have 5 spaces indent (or 1 space + ***)
                // Source lines have 8 spaces indent
                // We check for 6 spaces to be safe and robust against minor formatting differences
                // If we hit a new version line (indent < 6), stop checking the previous one
                checkingInstalledVersion = false;
            }
            
            if (checkingInstalledVersion) {
                // Check sources for this version
                // Check for any URI scheme (http, https, ftp, file, etc.)
                if (line.contains("://")) {
                    hasRepoSource = true;
                    break; // Found a repo source for installed version, so it's not local
                }
            }
        }
    }

    // If we never found the version table, we can't determine if it's local.
    // Assume not local to avoid showing everything in case of parse error.
    if (!inVersionTable) {
        qDebug() << "isPackageFromLocalInstall(" << packageName << "): No version table found";
        return false;
    }
    
    // If installed version has NO repo source, it is local
    if (!hasRepoSource) {
        qDebug() << "isPackageFromLocalInstall(" << packageName << "): Identified as LOCAL (Installed:" << installedVersion << ")";
    } else {
        // qDebug() << "isPackageFromLocalInstall(" << packageName << "): Identified as REPO (Installed:" << installedVersion << ")";
    }
    
    return !hasRepoSource;
}

QMap<QString, LocalPackageInfo> LocalPackageManager::localPackages() const
{
    QMutexLocker locker(&m_mutex);
    return m_localPackages;
}

QSet<QString> LocalPackageManager::localInstallPackages() const
{
    QMutexLocker locker(&m_mutex);
    return m_localInstallPackages;
}

bool LocalPackageManager::isLocalPackage(const QString &packageName) const
{
    QMutexLocker locker(&m_mutex);
    return m_localPackages.contains(packageName);
}

bool LocalPackageManager::isLocalInstallPackage(const QString &packageName) const
{
    QMutexLocker locker(&m_mutex);
    return m_localInstallPackages.contains(packageName);
}

bool LocalPackageManager::hasLocalFile(const QString &packageName) const
{
    QMutexLocker locker(&m_mutex);
    return m_localPackages.contains(packageName);
}

LocalPackageInfo LocalPackageManager::localPackageInfo(const QString &packageName) const
{
    QMutexLocker locker(&m_mutex);
    return m_localPackages.value(packageName);
}

QStringList LocalPackageManager::getLocalPackageFiles() const
{
    QMutexLocker locker(&m_mutex);
    QStringList files;
    for (const LocalPackageInfo &info : m_localPackages) {
        files << info.filename;
    }
    return files;
}

QList<LocalPackageInfo> LocalPackageManager::getVirtualPackages() const
{
    QList<LocalPackageInfo> virtualPackages;
    
    QMutexLocker locker(&m_mutex);
    if (!m_backend) {
        return virtualPackages;
    }
    
    // Iterate through all local packages and check if they exist in APT
    for (auto it = m_localPackages.constBegin(); it != m_localPackages.constEnd(); ++it) {
        const QString &packageName = it.key();
        
        // Check if this package exists in APT database
        QApt::Package *aptPackage = m_backend->package(packageName);
        
        if (!aptPackage) {
            // Package doesn't exist in APT - it's virtual
            virtualPackages.append(it.value());
            qDebug() << "Found virtual package:" << packageName << "from" << it.value().filename;
        }
    }
    
    return virtualPackages;
}

void LocalPackageManager::onDirectoryChanged(const QString &path)
{
    qDebug() << "Directory changed:" << path;
    scanLocalPackages();
}