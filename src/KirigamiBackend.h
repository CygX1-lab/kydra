/***************************************************************************
 *   Copyright © 2025 Kydra Project                                        *
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

#ifndef KIRIGAMIBACKEND_H
#define KIRIGAMIBACKEND_H

#include <QObject>
#include <QAbstractListModel>
#include <QVector>

namespace QApt {
    class Backend;
    class Package;
}

class PackageListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

public:
    enum PackageRoles {
        NameRole = Qt::UserRole + 1,
        DescriptionRole,
        VersionRole,
        IconRole,
        IsInstalledRole,
        PackageIdRole,
        CurrentVersionRole,
        NewVersionRole
    };

    explicit PackageListModel(QObject *parent = nullptr);
    ~PackageListModel();

    void setBackend(QApt::Backend *backend);
    void setPackages(const QVector<QApt::Package*> &packages);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void refresh();
    Q_INVOKABLE void search(const QString &searchTerm);

signals:
    void countChanged();
    void packageToggled(const QString &packageId, bool installed);

private:
    QApt::Backend *m_backend;
    QVector<QApt::Package*> m_packages;
};

class KirigamiBackend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(PackageListModel *availablePackages READ availablePackages CONSTANT)
    Q_PROPERTY(PackageListModel *updates READ updatesPackages CONSTANT)
    Q_PROPERTY(PackageListModel *installedPackages READ installedPackages CONSTANT)
    Q_PROPERTY(bool kirigamiMode READ kirigamiMode WRITE setKirigamiMode NOTIFY kirigamiModeChanged)
    Q_PROPERTY(QObject* selectedPackage READ selectedPackage NOTIFY selectedPackageChanged)

public:
    explicit KirigamiBackend(QObject *parent = nullptr);
    ~KirigamiBackend();

    void setBackend(QApt::Backend *backend);

    PackageListModel *availablePackages() const;
    PackageListModel *updatesPackages() const;
    PackageListModel *installedPackages() const;

    bool kirigamiMode() const;
    void setKirigamiMode(bool enabled);

    QObject* selectedPackage() const;

    Q_INVOKABLE void togglePackage(const QString &packageId);
    Q_INVOKABLE void updatePackage(const QString &packageId);
    Q_INVOKABLE void installPackage(const QString &packageId);
    Q_INVOKABLE void removePackage(const QString &packageId);
    Q_INVOKABLE void searchPackages(const QString &searchTerm);
    Q_INVOKABLE void filterByCategory(const QString &category);
    Q_INVOKABLE void previewChanges();
    Q_INVOKABLE void applyChanges();
    Q_INVOKABLE void checkForUpdates();
    Q_INVOKABLE void selectPackage(const QString &packageId);

signals:
    void kirigamiModeChanged();
    void selectedPackageChanged();
    void showPreviewDialog();
    void showTransactionDialog();
    void showSettingsDialog();
    void packageToggled(const QString &packageId, bool installed);

private:
    void refreshModels();
    QString categorizePackage(const QString &section) const;
    
    QApt::Backend *m_backend;
    PackageListModel *m_availablePackages;
    PackageListModel *m_updates;
    PackageListModel *m_installedPackages;
    bool m_kirigamiMode;
    QApt::Package *m_selectedPackage;
};

#endif // KIRIGAMIBACKEND_H