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

#include "DonateDialog.h"

// Qt includes
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QClipboard>
#include <QGuiApplication>

// KDE includes
#include <KLocalizedString>

DonateDialog::DonateDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(i18nc("@title:window", "Support Development"));
    setMinimumWidth(400);

    auto *layout = new QVBoxLayout(this);

    QLabel *intro = new QLabel(
        i18nc("@label", "<b>Support Development</b><br><br>"
              "This software is free and open-source (GPLv3). "
              "If you would like to support future development, "
              "you can donate using one of the addresses below.<br><br>"),
        this
    );
    intro->setWordWrap(true);

    QLabel *btc = new QLabel(i18nc("@label", "Bitcoin (BTC):<br><code>bc1q4z3d06unklcp868wgy6uy8t6y424r8hvy32uvw</code>"), this);
    btc->setTextFormat(Qt::RichText);
    btc->setTextInteractionFlags(Qt::TextSelectableByMouse);

    QLabel *eth = new QLabel(i18nc("@label", "Ethereum (ETH):<br><code>0xcAFdd35c1e00e6cc142F3df0c5DA4B0D428e6bf9</code>"), this);
    eth->setTextFormat(Qt::RichText);
    eth->setTextInteractionFlags(Qt::TextSelectableByMouse);

    QLabel *sol = new QLabel(i18nc("@label", "Solana (SOL):<br><code>89xBb3fXYm68WHJsi7hpNBv4hVcaN3GJVXsLNiuYVqv1</code>"), this);
    sol->setTextFormat(Qt::RichText);
    sol->setTextInteractionFlags(Qt::TextSelectableByMouse);

    layout->addWidget(intro);
    layout->addWidget(btc);
    layout->addWidget(eth);
    layout->addWidget(sol);

    auto *okButton = new QPushButton(i18nc("@action:button", "Close"), this);
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    layout->addWidget(okButton, 0, Qt::AlignRight);
}

DonateDialog::~DonateDialog()
{
}