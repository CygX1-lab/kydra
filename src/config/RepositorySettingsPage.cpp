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

#include "RepositorySettingsPage.h"

#include <QPushButton>
#include <QListWidget>
#include <QLineEdit>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>

#include <KLocalizedString>

#include <QApt/Config>

#include "MuonSettings.h"

RepositorySettingsPage::RepositorySettingsPage(QWidget* parent, QApt::Config *aptConfig)
        : SettingsPageBase(parent)
        , m_aptConfig(aptConfig)
        , m_repositoryList(new QListWidget(this))
        , m_addButton(new QPushButton(this))
        , m_removeButton(new QPushButton(this))
        , m_editButton(new QPushButton(this))
        , m_localDebFolderEdit(new QLineEdit(this))
        , m_browseButton(new QPushButton(this))
        , m_enableLocalDebFolderCheckBox(new QCheckBox(this))
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(0);
    setLayout(mainLayout);

    // Repository list group
    QGroupBox *repositoryGroup = new QGroupBox(i18n("Package Repositories"), this);
    QVBoxLayout *repositoryLayout = new QVBoxLayout(repositoryGroup);

    // Repository list
    m_repositoryList->setSelectionMode(QAbstractItemView::SingleSelection);
    repositoryLayout->addWidget(m_repositoryList);

    // Button layout for repository management
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    m_addButton->setText(i18n("Add"));
    m_addButton->setIcon(QIcon::fromTheme("list-add"));
    buttonLayout->addWidget(m_addButton);

    m_removeButton->setText(i18n("Remove"));
    m_removeButton->setIcon(QIcon::fromTheme("list-remove"));
    m_removeButton->setEnabled(false);
    buttonLayout->addWidget(m_removeButton);

    m_editButton->setText(i18n("Edit"));
    m_editButton->setIcon(QIcon::fromTheme("document-edit"));
    m_editButton->setEnabled(false);
    buttonLayout->addWidget(m_editButton);

    repositoryLayout->addLayout(buttonLayout);

    // Local .deb folder group
    QGroupBox *localDebGroup = new QGroupBox(i18n("Local .deb Packages"), this);
    QFormLayout *localDebLayout = new QFormLayout(localDebGroup);

    m_enableLocalDebFolderCheckBox->setText(i18n("Enable local .deb folder"));
    localDebLayout->addRow(m_enableLocalDebFolderCheckBox);

    QHBoxLayout *folderLayout = new QHBoxLayout();
    m_localDebFolderEdit->setPlaceholderText(i18n("Path to local .deb folder"));
    m_localDebFolderEdit->setEnabled(false);
    folderLayout->addWidget(m_localDebFolderEdit);

    m_browseButton->setText(i18n("Browse..."));
    m_browseButton->setEnabled(false);
    folderLayout->addWidget(m_browseButton);

    localDebLayout->addRow(i18n("Local .deb folder path:"), folderLayout);

    // Add groups to main layout
    mainLayout->addWidget(repositoryGroup);
    mainLayout->addWidget(localDebGroup);
    mainLayout->addStretch();

    // Connect signals
    connect(m_addButton, SIGNAL(clicked()), this, SLOT(addRepository()));
    connect(m_removeButton, SIGNAL(clicked()), this, SLOT(removeRepository()));
    connect(m_editButton, SIGNAL(clicked()), this, SLOT(editRepository()));
    connect(m_browseButton, SIGNAL(clicked()), this, SLOT(updateLocalDebFolder()));
    connect(m_enableLocalDebFolderCheckBox, SIGNAL(clicked()), this, SLOT(onRepositorySelectionChanged()));
    connect(m_repositoryList, SIGNAL(itemSelectionChanged()), this, SLOT(onRepositorySelectionChanged()));

    // Connect changed signals
    connect(m_addButton, SIGNAL(clicked()), this, SIGNAL(changed()));
    connect(m_removeButton, SIGNAL(clicked()), this, SIGNAL(changed()));
    connect(m_editButton, SIGNAL(clicked()), this, SIGNAL(changed()));
    connect(m_browseButton, SIGNAL(clicked()), this, SIGNAL(changed()));
    connect(m_enableLocalDebFolderCheckBox, SIGNAL(clicked()), this, SIGNAL(changed()));
    connect(m_localDebFolderEdit, SIGNAL(textChanged(QString)), this, SIGNAL(changed()));

    loadSettings();
}

RepositorySettingsPage::~RepositorySettingsPage()
{
}

void RepositorySettingsPage::loadSettings()
{
    populateRepositoryList();
    
    // Load local .deb folder settings
    QString localDebFolder = MuonSettings::self()->localDebFolder();
    bool enableLocalDeb = !localDebFolder.isEmpty();
    
    m_enableLocalDebFolderCheckBox->setChecked(enableLocalDeb);
    m_localDebFolderEdit->setText(localDebFolder);
    m_localDebFolderEdit->setEnabled(enableLocalDeb);
    m_browseButton->setEnabled(enableLocalDeb);
}

void RepositorySettingsPage::applySettings()
{
    // Save repository list to MuonSettings
    QStringList repositories;
    for (int i = 0; i < m_repositoryList->count(); ++i) {
        repositories.append(m_repositoryList->item(i)->text());
    }
    
    MuonSettings::self()->setRepositories(repositories);
    
    // Save local .deb folder settings
    QString localDebFolder;
    if (m_enableLocalDebFolderCheckBox->isChecked()) {
        localDebFolder = m_localDebFolderEdit->text().trimmed();
    }
    
    MuonSettings::self()->setLocalDebFolder(localDebFolder);
    MuonSettings::self()->save();
}

void RepositorySettingsPage::restoreDefaults()
{
    // Clear all repositories
    m_repositoryList->clear();
    
    // Add default Ubuntu/Debian repositories
    QStringList defaultRepos;
    defaultRepos << "deb http://archive.ubuntu.com/ubuntu focal main restricted universe multiverse"
                 << "deb http://archive.ubuntu.com/ubuntu focal-updates main restricted universe multiverse"
                 << "deb http://archive.ubuntu.com/ubuntu focal-security main restricted universe multiverse";
    
    m_repositoryList->addItems(defaultRepos);
    
    // Reset local .deb folder
    m_enableLocalDebFolderCheckBox->setChecked(false);
    m_localDebFolderEdit->clear();
    m_localDebFolderEdit->setEnabled(false);
    m_browseButton->setEnabled(false);
    
    emit changed();
}

void RepositorySettingsPage::populateRepositoryList()
{
    m_repositoryList->clear();
    
    // Load repositories from MuonSettings
    QStringList repositories = MuonSettings::self()->repositories();
    if (repositories.isEmpty()) {
        // Load default repositories from system
        repositories = getCurrentRepositories();
    }
    m_repositoryList->addItems(repositories);
}

QStringList RepositorySettingsPage::getCurrentRepositories() const
{
    QStringList repos;
    
    // Get repositories from apt sources.list and sources.list.d
    if (m_aptConfig) {
        // This is a simplified version - in a real implementation,
        // you would parse /etc/apt/sources.list and files in /etc/apt/sources.list.d/
        // For now, we'll return some common default repositories
        repos << "deb http://archive.ubuntu.com/ubuntu focal main restricted universe multiverse"
              << "deb http://archive.ubuntu.com/ubuntu focal-updates main restricted universe multiverse"
              << "deb http://archive.ubuntu.com/ubuntu focal-security main restricted universe multiverse";
    }
    
    return repos;
}

void RepositorySettingsPage::setRepositories(const QStringList &repositories)
{
    m_repositoryList->clear();
    m_repositoryList->addItems(repositories);
}

void RepositorySettingsPage::addRepository()
{
    bool ok;
    QString repoUrl = QInputDialog::getText(this,
                                           i18nc("@title:window", "Add Repository"),
                                           i18nc("@label", "Enter repository URL (e.g., deb http://archive.ubuntu.com/ubuntu focal main):"),
                                           QLineEdit::Normal, "", &ok);
    
    if (ok && !repoUrl.trimmed().isEmpty()) {
        m_repositoryList->addItem(repoUrl.trimmed());
        emit changed();
    }
}

void RepositorySettingsPage::removeRepository()
{
    QListWidgetItem *currentItem = m_repositoryList->currentItem();
    if (currentItem) {
        int ret = QMessageBox::question(this,
                                       i18nc("@title:window", "Remove Repository"),
                                       i18nc("@label", "Are you sure you want to remove this repository?"),
                                       QMessageBox::Yes | QMessageBox::No,
                                       QMessageBox::No);
        
        if (ret == QMessageBox::Yes) {
            delete currentItem;
            emit changed();
        }
    }
}

void RepositorySettingsPage::editRepository()
{
    QListWidgetItem *currentItem = m_repositoryList->currentItem();
    if (currentItem) {
        bool ok;
        QString currentText = currentItem->text();
        QString newText = QInputDialog::getText(this,
                                               i18nc("@title:window", "Edit Repository"),
                                               i18nc("@label", "Edit repository URL:"),
                                               QLineEdit::Normal, currentText, &ok);
        
        if (ok && !newText.trimmed().isEmpty() && newText != currentText) {
            currentItem->setText(newText.trimmed());
            emit changed();
        }
    }
}

void RepositorySettingsPage::updateLocalDebFolder()
{
    QString currentPath = m_localDebFolderEdit->text();
    QString newPath = QFileDialog::getExistingDirectory(this,
                                                       i18nc("@title:window", "Select Local .deb Folder"),
                                                       currentPath.isEmpty() ? QDir::homePath() : currentPath);
    
    if (!newPath.isEmpty()) {
        m_localDebFolderEdit->setText(newPath);
        emit changed();
    }
}

void RepositorySettingsPage::onRepositorySelectionChanged()
{
    bool hasSelection = !m_repositoryList->selectedItems().isEmpty();
    m_removeButton->setEnabled(hasSelection);
    m_editButton->setEnabled(hasSelection);
    
    bool localDebEnabled = m_enableLocalDebFolderCheckBox->isChecked();
    m_localDebFolderEdit->setEnabled(localDebEnabled);
    m_browseButton->setEnabled(localDebEnabled);
}
