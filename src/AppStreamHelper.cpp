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

#include "AppStreamHelper.h"
#include <QDebug>

#ifdef HAVE_APPSTREAM
#include <AppStreamQt5/screenshot.h>
#include <AppStreamQt5/image.h>
#include <AppStreamQt5/icon.h>
#endif

AppStreamHelper *AppStreamHelper::instance()
{
    static AppStreamHelper s_instance;
    return &s_instance;
}

AppStreamHelper::AppStreamHelper(QObject *parent)
    : QObject(parent)
    , m_initialized(false)
{
}

AppStreamHelper::~AppStreamHelper()
{
}

void AppStreamHelper::init()
{
    if (m_initialized) return;

#ifdef HAVE_APPSTREAM
    if (!m_pool.load()) {
        qWarning() << "Failed to load AppStream metadata pool";
    } else {
        qDebug() << "AppStream metadata loaded successfully";
    }
#endif
    m_initialized = true;
}

bool AppStreamHelper::isAvailable() const
{
#ifdef HAVE_APPSTREAM
    return true;
#else
    return false;
#endif
}

#ifdef HAVE_APPSTREAM
AppStream::Component AppStreamHelper::findComponent(const QString &packageName) const
{
    if (m_componentCache.contains(packageName)) {
        return m_componentCache.value(packageName);
    }

    // Search by package name
    // Note: This is a simplified search. Ideally we should match against the package name provided by the component.
    QList<AppStream::Component> components = m_pool.componentsByLaunchable(AppStream::Launchable::KindDesktopId, packageName + ".desktop").toList();
    
    if (components.isEmpty()) {
        // Try searching by package name directly if supported or iterate
        // For now, let's try a broader search or just return invalid
        // Some components might not have the .desktop suffix in their ID
         components = m_pool.componentsById(packageName + ".desktop").toList();
    }
    
    // Fallback: Iterate and check package names (expensive, maybe optimize later)
    if (components.isEmpty()) {
         const auto allComponents = m_pool.components();
         for (const auto &comp : allComponents) {
             if (comp.packageNames().contains(packageName)) {
                 m_componentCache.insert(packageName, comp);
                 return comp;
             }
         }
    }

    if (!components.isEmpty()) {
        m_componentCache.insert(packageName, components.first());
        return components.first();
    }

    return AppStream::Component();
}
#endif

QString AppStreamHelper::getGenericName(const QString &packageName) const
{
#ifdef HAVE_APPSTREAM
    auto comp = findComponent(packageName);
    if (comp.isValid()) {
        return comp.name(); // Or genericName() if preferred, but name() is usually better for display
    }
#endif
    return QString();
}

QString AppStreamHelper::getLongDescription(const QString &packageName) const
{
#ifdef HAVE_APPSTREAM
    auto comp = findComponent(packageName);
    if (comp.isValid()) {
        return comp.description();
    }
#endif
    return QString();
}

QString AppStreamHelper::getIconUrl(const QString &packageName) const
{
#ifdef HAVE_APPSTREAM
    auto comp = findComponent(packageName);
    if (comp.isValid()) {
        QList<AppStream::Icon> icons = comp.icons();
        for (const auto &icon : icons) {
            if (icon.kind() == AppStream::Icon::KindRemote) {
                return icon.url().toString();
            }
        }
    }
#endif
    return QString();
}

QString AppStreamHelper::getScreenshotUrl(const QString &packageName) const
{
#ifdef HAVE_APPSTREAM
    auto comp = findComponent(packageName);
    if (comp.isValid()) {
        QList<AppStream::Screenshot> screenshots = comp.screenshotsAll();
        if (!screenshots.isEmpty()) {
            // Get default screenshot
            auto screenshot = screenshots.first();
            // Prefer source image (highest res) or large thumbnail
            QList<AppStream::Image> images = screenshot.images();
            for (const auto &img : images) {
                if (img.kind() == AppStream::Image::KindSource) {
                    return img.url().toString();
                }
            }
            if (!images.isEmpty()) {
                 return images.first().url().toString();
            }
        }
    }
#endif
    return QString();
}

QColor AppStreamHelper::getBrandColor(const QString &packageName) const
{
#ifdef HAVE_APPSTREAM
    auto comp = findComponent(packageName);
    if (comp.isValid()) {
        // AppStreamQt5 might not expose branding colors directly in older versions
        // Checking available API... assuming standard metadata access
        // If not available, we return invalid color
        // Note: As of AppStreamQt 0.15+, branding might be available via custom keys or newer API
        // For now, let's return invalid to be safe unless we find a specific API
        return QColor(); 
    }
#endif
    return QColor();
}
