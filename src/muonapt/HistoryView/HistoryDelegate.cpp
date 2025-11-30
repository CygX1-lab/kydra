/*
 *  Modern History Delegate for Kydra Package Manager
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

#include "HistoryDelegate.h"

#include <QPainter>
#include <QApplication>
#include <QDate>
#include <KIconLoader>

HistoryDelegate::HistoryDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void HistoryDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() == 0) {
        paintCard(painter, option, index);
    } else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QSize HistoryDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() == 0) {
        return QSize(option.rect.width(), 80); // Fixed height for card
    }
    return QStyledItemDelegate::sizeHint(option, index);
}

void HistoryDelegate::paintCard(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    // Card dimensions with margins
    int margin = 6;
    QRect cardRect = option.rect.adjusted(margin, margin/2, -margin, -margin/2);
    
    // Draw shadow
    QColor shadowColor(0, 0, 0, 20);
    painter->setPen(Qt::NoPen);
    painter->setBrush(shadowColor);
    painter->drawRoundedRect(cardRect.adjusted(2, 2, 2, 2), 6, 6);

    // Draw card background
    QColor bgColor = option.palette.color(QPalette::Base);
    if (option.state & QStyle::State_Selected) {
        bgColor = option.palette.color(QPalette::Highlight).lighter(160);
    } else if (option.state & QStyle::State_MouseOver) {
        bgColor = option.palette.color(QPalette::Base).darker(105);
    }
    
    painter->setBrush(bgColor);
    
    // Border
    if (option.state & QStyle::State_Selected) {
        painter->setPen(QPen(option.palette.color(QPalette::Highlight), 1.5));
    } else {
        painter->setPen(QPen(option.palette.color(QPalette::Mid), 1));
    }
    
    painter->drawRoundedRect(cardRect, 6, 6);
    
    // Content
    // Assuming model columns: 0=Package, 1=Date, 2=State/Action
    // But this is a TreeView, so we might need to check how data is structured.
    // Usually column 0 has the main text.
    
    QString mainText = index.data(Qt::DisplayRole).toString();
    QIcon icon = index.data(Qt::DecorationRole).value<QIcon>();
    
    // Icon
    QRect iconRect(cardRect.left() + 10, cardRect.top() + 10, 48, 48);
    if (!icon.isNull()) {
        icon.paint(painter, iconRect);
    } else {
        // Fallback icon
        QIcon::fromTheme("system-software-install").paint(painter, iconRect);
    }
    
    // Text
    painter->setPen(option.palette.color(QPalette::Text));
    QFont titleFont = option.font;
    titleFont.setBold(true);
    titleFont.setPointSize(titleFont.pointSize() + 1);
    painter->setFont(titleFont);
    
    QRect textRect = cardRect.adjusted(70, 10, -10, -10);
    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignTop, mainText);
    
    // Subtext (Date/Details)
    // We might need to access sibling indices if it's a flat list, 
    // or just rely on what's in this index if it's a tree node.
    // For now, let's just draw the main text nicely.
    
    painter->restore();
}
