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

#ifndef PACKAGEICONEXTRACTOR_H
#define PACKAGEICONEXTRACTOR_H

#include <QObject>
#include <QIcon>
#include <QCache>
#include <QMutex>

#include <QApt/Package>

class PackageIconExtractor : public QObject
{
    Q_OBJECT

public:
    static PackageIconExtractor* instance();
    
    QIcon getPackageIcon(QApt::Package* package);
    void clearCache();

private:
    explicit PackageIconExtractor(QObject* parent = nullptr);
    ~PackageIconExtractor() override;
    
    QIcon extractIconFromPackage(QApt::Package* package);
    QIcon extractIconFromDebFile(const QString& debFilePath);
    QIcon extractIconFromControlFile(const QString& controlData);
    QIcon getIconFromDesktopFile(const QString& desktopFileContent);
    QString findIconPath(const QString& iconName);
    
    QCache<QString, QIcon> m_iconCache;
    QMutex m_cacheMutex;
    
    static PackageIconExtractor* s_instance;
};

#endif // PACKAGEICONEXTRACTOR_H