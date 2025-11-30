/*
 *  AppStream Helper for Kydra Package Manager
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

#ifndef APPSTREAMHELPER_H
#define APPSTREAMHELPER_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QColor>
#include <QSharedPointer>

#ifdef HAVE_APPSTREAM
#include <AppStreamQt5/pool.h>
#include <AppStreamQt5/component.h>
#endif

class AppStreamHelper : public QObject
{
    Q_OBJECT

public:
    static AppStreamHelper *instance();
    
    void init();
    bool isAvailable() const;
    
    // Metadata retrieval
    QString getGenericName(const QString &packageName) const;
    QString getLongDescription(const QString &packageName) const;
    QString getIconUrl(const QString &packageName) const;
    QString getScreenshotUrl(const QString &packageName) const;
    QColor getBrandColor(const QString &packageName) const;
    
private:
    explicit AppStreamHelper(QObject *parent = nullptr);
    ~AppStreamHelper();
    
#ifdef HAVE_APPSTREAM
    AppStream::Pool m_pool;
    mutable QMap<QString, AppStream::Component> m_componentCache;
    
    AppStream::Component findComponent(const QString &packageName) const;
#endif
    bool m_initialized;
};

#endif // APPSTREAMHELPER_H
