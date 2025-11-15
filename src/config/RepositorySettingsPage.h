/***************************************************************************
 *   Copyright © 2025 Muon Project                                         *
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

#ifndef REPOSITORYSETTINGSPAGE_H
#define REPOSITORYSETTINGSPAGE_H

#include <QtWidgets/QWidget>

#include "../settings/SettingsPageBase.h"

class QPushButton;
class QListWidget;
class QLineEdit;
class QCheckBox;

namespace QApt {
    class Config;
}

class RepositorySettingsPage : public SettingsPageBase
{
    Q_OBJECT

public:
    RepositorySettingsPage(QWidget* parent, QApt::Config *aptConfig);
    virtual ~RepositorySettingsPage();

    void loadSettings();
    void applySettings() override;
    void restoreDefaults() override;

private Q_SLOTS:
    void addRepository();
    void removeRepository();
    void editRepository();
    void updateLocalDebFolder();
    void onRepositorySelectionChanged();

private:
    QApt::Config *m_aptConfig;
    QListWidget *m_repositoryList;
    QPushButton *m_addButton;
    QPushButton *m_removeButton;
    QPushButton *m_editButton;
    QLineEdit *m_localDebFolderEdit;
    QPushButton *m_browseButton;
    QCheckBox *m_enableLocalDebFolderCheckBox;

    void populateRepositoryList();
    QStringList getCurrentRepositories() const;
    void setRepositories(const QStringList &repositories);
};

#endif