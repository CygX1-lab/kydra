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

#include "PackageIconExtractor.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <QTemporaryDir>
#include <QIcon>
#include <QPixmap>
#include <QRegularExpression>

#include <QApt/Package>
#include <QApt/DebFile>

PackageIconExtractor* PackageIconExtractor::s_instance = nullptr;

PackageIconExtractor* PackageIconExtractor::instance()
{
    if (!s_instance) {
        s_instance = new PackageIconExtractor();
    }
    return s_instance;
}

PackageIconExtractor::PackageIconExtractor(QObject* parent)
    : QObject(parent)
    , m_iconCache(100) // Cache up to 100 icons
{
}

PackageIconExtractor::~PackageIconExtractor()
{
    clearCache();
}

QIcon PackageIconExtractor::getPackageIcon(QApt::Package* package)
{
    if (!package) {
        return QIcon::fromTheme("package-x-generic");
    }

    const QString packageName = package->name();
    
    // Check cache first
    {
        QMutexLocker locker(&m_cacheMutex);
        if (m_iconCache.contains(packageName)) {
            return *m_iconCache.object(packageName);
        }
    }

    // Try to extract icon from package
    QIcon packageIcon = extractIconFromPackage(package);
    
    // If no specific icon found, fall back to appropriate KDE icons based on package type
    if (packageIcon.isNull()) {
        // Use different icons based on package category/section
        QString section = QString(package->section()).toLower();
        
        if (section.contains("games") || section.contains("game")) {
            packageIcon = QIcon::fromTheme("applications-games");
        } else if (section.contains("devel") || section.contains("development")) {
            packageIcon = QIcon::fromTheme("applications-development");
        } else if (section.contains("graphics") || section.contains("image")) {
            packageIcon = QIcon::fromTheme("applications-graphics");
        } else if (section.contains("multimedia") || section.contains("sound") || section.contains("video")) {
            packageIcon = QIcon::fromTheme("applications-multimedia");
        } else if (section.contains("network") || section.contains("web") || section.contains("internet")) {
            packageIcon = QIcon::fromTheme("applications-internet");
        } else if (section.contains("office") || section.contains("text")) {
            packageIcon = QIcon::fromTheme("applications-office");
        } else if (section.contains("system") || section.contains("admin")) {
            packageIcon = QIcon::fromTheme("applications-system");
        } else if (section.contains("education") || section.contains("science")) {
            packageIcon = QIcon::fromTheme("applications-science");
        } else if (section.contains("utilities") || section.contains("tools")) {
            packageIcon = QIcon::fromTheme("applications-utilities");
        } else {
            // Default to generic package icon
            packageIcon = QIcon::fromTheme("package-x-generic");
        }
    }

    // Cache the result
    {
        QMutexLocker locker(&m_cacheMutex);
        m_iconCache.insert(packageName, new QIcon(packageIcon));
    }

    return packageIcon;
}

void PackageIconExtractor::clearCache()
{
    QMutexLocker locker(&m_cacheMutex);
    m_iconCache.clear();
}

QIcon PackageIconExtractor::extractIconFromPackage(QApt::Package* package)
{
    // First, try to get the icon from the package's desktop files
    QStringList installedFiles = package->installedFilesList();
    
    // Look for .desktop files in the installed files
    for (const QString& filePath : installedFiles) {
        if (filePath.endsWith(".desktop") && filePath.contains("/applications/")) {
            QFile desktopFile(filePath);
            if (desktopFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QString desktopContent = desktopFile.readAll();
                desktopFile.close();
                
                QIcon icon = getIconFromDesktopFile(desktopContent);
                if (!icon.isNull()) {
                    return icon;
                }
            }
        }
    }

    // If no desktop file found, try to extract from the deb file if available
    // This would require access to the original deb file, which might not be available
    // for already installed packages
    
    return QIcon();
}

QIcon PackageIconExtractor::extractIconFromDebFile(const QString& debFilePath)
{
    // Create a temporary directory for extraction
    QTemporaryDir tempDir;
    if (!tempDir.isValid()) {
        return QIcon();
    }

    // Extract the deb file using ar command
    QProcess arProcess;
    arProcess.start("ar", QStringList() << "x" << debFilePath << tempDir.path());
    if (!arProcess.waitForFinished(30000)) { // 30 second timeout
        return QIcon();
    }

    // Extract the control.tar.gz or control.tar.xz
    QDir tempDirPath(tempDir.path());
    QStringList tarFiles = tempDirPath.entryList(QStringList() << "control.tar.*", QDir::Files);
    
    if (tarFiles.isEmpty()) {
        return QIcon();
    }

    QString tarFile = tempDirPath.absoluteFilePath(tarFiles.first());
    
    // Extract the tar file
    QProcess tarProcess;
    tarProcess.setWorkingDirectory(tempDir.path());
    tarProcess.start("tar", QStringList() << "xf" << tarFile);
    if (!tarProcess.waitForFinished(30000)) {
        return QIcon();
    }

    // Look for desktop files in the extracted control directory
    QDir controlDir(tempDir.path());
    QStringList desktopFiles = controlDir.entryList(QStringList() << "*.desktop", QDir::Files | QDir::AllDirs);
    
    for (const QString& desktopFile : desktopFiles) {
        QFile file(controlDir.absoluteFilePath(desktopFile));
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString desktopContent = file.readAll();
            file.close();
            
            QIcon icon = getIconFromDesktopFile(desktopContent);
            if (!icon.isNull()) {
                return icon;
            }
        }
    }

    return QIcon();
}

QIcon PackageIconExtractor::getIconFromDesktopFile(const QString& desktopFileContent)
{
    // Parse the desktop file to find the Icon field
    QRegularExpression iconRegex("^Icon\\s*=\\s*(.+)$", QRegularExpression::MultilineOption);
    QRegularExpressionMatch match = iconRegex.match(desktopFileContent);
    
    if (!match.hasMatch()) {
        return QIcon();
    }

    QString iconName = match.captured(1).trimmed();
    
    // If it's an absolute path, try to load it directly
    if (iconName.startsWith('/')) {
        if (QFile::exists(iconName)) {
            return QIcon(iconName);
        }
    } else {
        // Try to find the icon in the system icon theme
        QIcon themedIcon = QIcon::fromTheme(iconName);
        if (!themedIcon.isNull()) {
            return themedIcon;
        }
        
        // If not found in theme, try common icon directories
        QString foundPath = findIconPath(iconName);
        if (!foundPath.isEmpty()) {
            return QIcon(foundPath);
        }
    }

    return QIcon();
}

QString PackageIconExtractor::findIconPath(const QString& iconName)
{
    // Common icon directories to search
    QStringList iconDirs = {
        "/usr/share/pixmaps",
        "/usr/share/icons/hicolor/48x48/apps",
        "/usr/share/icons/hicolor/64x64/apps",
        "/usr/share/icons/hicolor/128x128/apps",
        "/usr/share/icons/hicolor/256x256/apps",
        "/usr/share/icons/hicolor/scalable/apps"
    };

    QStringList extensions = {".png", ".svg", ".xpm", ".jpg", ".jpeg"};

    for (const QString& dir : iconDirs) {
        for (const QString& ext : extensions) {
            QString fullPath = dir + "/" + iconName + ext;
            if (QFile::exists(fullPath)) {
                return fullPath;
            }
        }
    }

    return QString();
}