/*
 *  Flatpak Manager for Kydra Package Manager
 *  Copyright (C) 2025 Kydra Project
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "FlatpakManager.h"

#include <QProcess>
#include <QDebug>
#include <QtConcurrent>

FlatpakManager *FlatpakManager::s_instance = nullptr;

FlatpakManager *FlatpakManager::instance()
{
    if (!s_instance) {
        s_instance = new FlatpakManager();
    }
    return s_instance;
}

FlatpakManager::FlatpakManager(QObject *parent)
    : QObject(parent)
{
}

FlatpakManager::~FlatpakManager()
{
}

void FlatpakManager::init()
{
    refresh();
}

void FlatpakManager::refresh()
{
    QtConcurrent::run([this]() {
        parseInstalled();
        emit packagesChanged();
    });
}

void FlatpakManager::parseInstalled()
{
    QProcess process;
    process.start("flatpak", QStringList() << "list" << "--app" << "--columns=application,name,description,version,arch,branch,origin");
    process.waitForFinished();
    
    QString output = process.readAllStandardOutput();
    QStringList lines = output.split('\n', Qt::SkipEmptyParts);
    
    m_packages.clear();
    
    for (const QString &line : lines) {
        QStringList parts = line.split('\t');
        if (parts.count() < 7) continue;
        
        FlatpakPackage pkg;
        pkg.id = parts[0];
        pkg.name = parts[1];
        pkg.description = parts[2];
        pkg.version = parts[3];
        pkg.arch = parts[4];
        pkg.branch = parts[5];
        pkg.remote = parts[6];
        pkg.isInstalled = true;
        pkg.section = "Flatpak";
        
        m_packages.insert(pkg.id, pkg);
    }
    
    // TODO: Also list available packages from remote (e.g. flathub)
    // This is expensive with CLI "flatpak remote-ls". 
    // For now, we only show installed Flatpaks or rely on AppStream for discovery.
}

QList<FlatpakPackage> FlatpakManager::listPackages() const
{
    return m_packages.values();
}

FlatpakPackage FlatpakManager::getPackage(const QString &id) const
{
    return m_packages.value(id);
}

bool FlatpakManager::isFlatpak(const QString &id) const
{
    return m_packages.contains(id);
}

void FlatpakManager::installPackage(const QString &id, const QString &remote)
{
    emit operationStarted("install");
    
    // Run in background
    QtConcurrent::run([this, id, remote]() {
        QProcess process;
        // -y for non-interactive
        process.start("flatpak", QStringList() << "install" << "-y" << remote << id);
        
        connect(&process, &QProcess::readyReadStandardOutput, this, [this, &process]() {
             emit outputAvailable(process.readAllStandardOutput());
        });
        
        bool success = process.waitForFinished(-1); // Wait indefinitely
        
        if (success && process.exitCode() == 0) {
            parseInstalled(); // Refresh list
            emit packagesChanged();
            emit operationFinished("install", true);
        } else {
            emit operationFinished("install", false);
        }
    });
}

void FlatpakManager::removePackage(const QString &id)
{
    emit operationStarted("remove");
    
    QtConcurrent::run([this, id]() {
        QProcess process;
        process.start("flatpak", QStringList() << "uninstall" << "-y" << id);
        
        bool success = process.waitForFinished(-1);
        
        if (success && process.exitCode() == 0) {
            parseInstalled();
            emit packagesChanged();
            emit operationFinished("remove", true);
        } else {
            emit operationFinished("remove", false);
        }
    });
}

void FlatpakManager::updateAll()
{
    emit operationStarted("update");
    
    QtConcurrent::run([this]() {
        QProcess process;
        process.start("flatpak", QStringList() << "update" << "-y");
        
        bool success = process.waitForFinished(-1);
        
        if (success && process.exitCode() == 0) {
            parseInstalled();
            emit packagesChanged();
            emit operationFinished("update", true);
        } else {
            emit operationFinished("update", false);
        }
    });
}
