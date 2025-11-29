/***************************************************************************
 *   Copyright © 2010, 2011 Jonathan Thomas <echidnaman@kubuntu.org>       *
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

#include "CategoryFilter.h"

// Qt includes
#include <QtCore/QSet>

// KDE includes
#include <KLocalizedString>

// QApt includes
#include <QApt/Backend>

// Own includes
#include "muonapt/MuonStrings.h"

CategoryFilter::CategoryFilter(QObject *parent, QApt::Backend *backend)
    : FilterModel(parent)
    , m_backend(backend)
{
}

QString getIconForGroup(const QString &group)
{
    if (group == "Administration") return "applications-system";
    if (group == "Base System") return "system-help";
    if (group == "Communication") return "applications-internet";
    if (group == "Development") return "applications-development";
    if (group == "Documentation") return "help-contents";
    if (group == "Editors") return "accessories-text-editor";
    if (group == "Electronics") return "applications-engineering";
    if (group == "Embedded Devices") return "applications-system";
    if (group == "Games") return "applications-games";
    if (group == "GNOME Desktop") return "preferences-desktop";
    if (group == "Graphics") return "applications-graphics";
    if (group == "Ham Radio") return "network-wireless";
    if (group == "Interpreters") return "applications-development";
    if (group == "KDE Desktop") return "kde";
    if (group == "Libraries") return "applications-other";
    if (group == "Mail") return "internet-mail";
    if (group == "Mathematics") return "applications-education";
    if (group == "Miscellaneous") return "applications-other";
    if (group == "Multimedia") return "applications-multimedia";
    if (group == "Networking") return "applications-internet";
    if (group == "News") return "internet-news-reader";
    if (group == "Old Libraries") return "applications-other";
    if (group == "Other") return "applications-other";
    if (group == "Perl") return "applications-development";
    if (group == "Python") return "applications-development";
    if (group == "Science") return "applications-science";
    if (group == "Shells") return "utilities-terminal";
    if (group == "Sound") return "audio-x-generic";
    if (group == "TeX") return "applications-office";
    if (group == "Text Processing") return "applications-office";
    if (group == "Utilities") return "applications-utilities";
    if (group == "Video") return "video-x-generic";
    if (group == "Web Software") return "applications-internet";
    if (group == "X11") return "preferences-desktop-display";
    
    return "applications-other";
}

void CategoryFilter::populate()
{
    QApt::GroupList groups = m_backend->availableGroups();
    QSet<QString> groupSet;

    foreach(const QApt::Group &group, groups) {
        QString groupName = MuonStrings::global()->groupName(group);

        if (!groupName.isEmpty()) {
            groupSet << groupName;
        }
    }

    QStandardItem *defaultItem = new QStandardItem;
    defaultItem->setEditable(false);
    defaultItem->setIcon(QIcon::fromTheme("bookmark-new-list"));
    defaultItem->setText(i18nc("@item:inlistbox Item that resets the filter to \"all\"", "All"));
    appendRow(defaultItem);

    QStringList groupList = groupSet.toList();
    qSort(groupList);

    foreach(const QString &group, groupList) {
        QStandardItem *groupItem = new QStandardItem;
        groupItem->setEditable(false);
        groupItem->setText(group);
        groupItem->setIcon(QIcon::fromTheme(getIconForGroup(group)));
        appendRow(groupItem);
    }
}
