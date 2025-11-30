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

#ifndef FLATPAKMANAGER_H
#define FLATPAKMANAGER_H

#include <QObject>
#include <QHash>
#include <QList>
#include <QMap>

struct FlatpakPackage {
    QString id;
    QString name;
    QString description;
    QString version;
    QString arch;
    QString branch;
    QString remote;
    bool isInstalled;
    
    // For sorting/display
    QString section; // e.g. "Flatpak"
};

class FlatpakManager : public QObject
{
    Q_OBJECT

public:
    static FlatpakManager *instance();

    void init();
    
    QList<FlatpakPackage> listPackages() const;
    FlatpakPackage getPackage(const QString &id) const;
    
    bool isFlatpak(const QString &id) const;

public slots:
    void installPackage(const QString &id, const QString &remote = "flathub");
    void removePackage(const QString &id);
    void updateAll();
    void refresh();

signals:
    void packagesChanged();
    void operationStarted(const QString &op);
    void operationFinished(const QString &op, bool success);
    void outputAvailable(const QString &output);

private:
    explicit FlatpakManager(QObject *parent = nullptr);
    ~FlatpakManager();

    void parseInstalled();
    void parseRemotes(); // To list available apps (simplified for now)

    static FlatpakManager *s_instance;
    QMap<QString, FlatpakPackage> m_packages;
};

#endif // FLATPAKMANAGER_H
