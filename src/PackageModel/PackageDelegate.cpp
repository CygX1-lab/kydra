/***************************************************************************
 *   Copyright © 2010 Jonathan Thomas <echidnaman@kubuntu.org>             *
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

#include "PackageDelegate.h"
#include "PackageIconExtractor.h"

// Qt
#include <QApplication>
#include <QPainter>
#include <QPainterPath>

// KDE
#include <KColorScheme>
#include <KIconLoader>
#include <KLocalizedString>

// Own
#include "muonapt/MuonStrings.h"
#include "PackageModel.h"
#include "MuonSettings.h"

// Qt includes for color parsing
#include <QRegularExpression>
#include <QStringList>

PackageDelegate::PackageDelegate(QObject *parent)
    : QAbstractItemDelegate(parent)
    , m_supportedEmblem(QIcon::fromTheme("emblem-ok-symbolic").pixmap(QSize(12,12)))
    , m_lockedEmblem(QIcon::fromTheme("object-locked-symbolic").pixmap(QSize(12,12)))
{
    m_spacing  = 12; // Increased spacing for cleaner layout

    m_iconSize = KIconLoader::SizeSmallMedium;
}

void PackageDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid()) {
        return;
    }

    paintBackground(painter, option);

    switch (index.column()) {
    case 0:
        paintPackageName(painter, option, index);
        break;
    case 1: // Status
    case 2: // Action
    case 3: // InstalledSize
    case 4: // InstalledVersion
    case 5: // AvailableVersion
        paintText(painter, option, index);
        break;
    default:
        break;
    }
}

void PackageDelegate::paintBackground(QPainter *painter, const QStyleOptionViewItem &option) const
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    // Cleaner, flatter list design
    // No heavy shadows or rounded cards for every item, just a clean list item
    
    QColor bgColor = option.palette.color(QPalette::Base);
    
    if (option.state & QStyle::State_Selected) {
        // Modern selection style
        bgColor = option.palette.color(QPalette::Highlight).lighter(170);
        painter->fillRect(option.rect, bgColor);
        
        // Selection indicator on the left
        if (option.rect.left() == 0) { // Only draw for the first column or effectively the whole row
             painter->setPen(Qt::NoPen);
             painter->setBrush(option.palette.color(QPalette::Highlight));
             painter->drawRect(option.rect.left(), option.rect.top(), 3, option.rect.height());
        }
    } else if (option.state & QStyle::State_MouseOver) {
        // Subtle hover effect
        bgColor = option.palette.color(QPalette::Base).darker(102);
        painter->fillRect(option.rect, bgColor);
    } else {
        // Alternating row colors or just plain white/base
        // Keeping it simple with base color for now
        painter->fillRect(option.rect, bgColor);
    }
    
    // Bottom separator line
    painter->setPen(QPen(option.palette.color(QPalette::Midlight), 1));
    painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());
    
    painter->restore();
}

void PackageDelegate::paintPackageName(QPainter *painter, const QStyleOptionViewItem &option , const QModelIndex &index) const
{
    int left = option.rect.left();
    int top = option.rect.top();
    int width = option.rect.width();

    bool leftToRight = (painter->layoutDirection() == Qt::LeftToRight);

    QColor foregroundColor = option.palette.color(QPalette::Text);
    if (option.state & QStyle::State_Selected) {
        foregroundColor = option.palette.color(QPalette::HighlightedText);
        // If highlighted text color is too similar to background (common in some light themes with light selection), use text color
        if (foregroundColor == option.palette.color(QPalette::Highlight)) {
             foregroundColor = option.palette.color(QPalette::Text);
        }
    }

    const qreal dpr = painter->device()->devicePixelRatioF();

    // Pixmap that the text/icon goes in
    QPixmap pixmap(option.rect.size() * dpr);
    pixmap.setDevicePixelRatio(dpr);
    pixmap.fill(Qt::transparent);
    QPainter p(&pixmap);
    p.translate(-option.rect.topLeft());

    // Get package-specific icon
    QIcon packageIcon = index.data(PackageModel::IconRole).value<QIcon>();
    int iconX = leftToRight ? left + m_spacing : left + width - m_spacing - m_iconSize;
    int iconY = top + (option.rect.height() - m_iconSize) / 2; // Vertically centered
    
    packageIcon.paint(&p,
                      iconX,
                      iconY,
                      m_iconSize,
                      m_iconSize,
                      Qt::AlignCenter,
                      QIcon::Normal);

    int state = index.data(PackageModel::StatusRole).toInt();

    if (state & QApt::Package::IsPinned) {
        p.drawPixmap(iconX + m_iconSize - m_lockedEmblem.width()/2,
                     iconY + m_iconSize - m_lockedEmblem.height()/2,
                     m_lockedEmblem);
    } else if (index.data(PackageModel::SupportRole).toBool()) {
        p.drawPixmap(iconX + m_iconSize - m_supportedEmblem.width()/2,
                     iconY + m_iconSize - m_supportedEmblem.height()/2,
                     m_supportedEmblem);
    }

    // Text
    QStyleOptionViewItem name_item(option);
    QStyleOptionViewItem description_item(option);

    // Make name slightly larger/bolder
    name_item.font.setBold(true);
    name_item.font.setPointSize(name_item.font.pointSize() + 1);
    
    // Description slightly smaller and lighter
    description_item.font.setPointSize(name_item.font.pointSize() - 2);

    int textInner = 2 * m_spacing + m_iconSize;
    
    p.setPen(foregroundColor);
    
    // Draw "Local" tag if applicable
    int nameX = left + (leftToRight ? textInner : 0);
    int nameWidth = width - textInner;
    
    // Calculate vertical positions
    QFontMetrics nameFm(name_item.font);
    QFontMetrics descFm(description_item.font);
    
    int totalTextHeight = nameFm.height() + descFm.height() + 2; // 2px padding between lines
    int startY = top + (option.rect.height() - totalTextHeight) / 2;
    
    bool isLocal = index.data(PackageModel::IsLocalRole).toBool();
    QString nameText = index.data(PackageModel::NameRole).toString();
    
    int nameTextWidth = nameFm.width(nameText);
    
    if (isLocal) {
        // Tag settings
        QString tagText = i18n("Local");
        QFont tagFont = description_item.font;
        tagFont.setBold(true);
        QFontMetrics tagFm(tagFont);
        int tagWidth = tagFm.width(tagText) + 10;
        int tagHeight = tagFm.height() + 2;
        
        int tagX = nameX + nameTextWidth + 10;
        int tagY = startY + (nameFm.height() - tagHeight) / 2;
        
        // Draw tag background
        QColor tagColor = QColor(66, 133, 244); // Google Blue-ish
        QColor tagBgColor = tagColor.lighter(170);
        tagBgColor.setAlpha(200);
        
        p.setRenderHint(QPainter::Antialiasing);
        QPainterPath path;
        path.addRoundedRect(tagX, tagY, tagWidth, tagHeight, 4, 4);
        p.fillPath(path, tagBgColor);
        
        // Draw tag text
        p.setPen(tagColor.darker(120));
        p.setFont(tagFont);
        p.drawText(tagX, tagY, tagWidth, tagHeight, Qt::AlignCenter, tagText);
        
        // Reset painter for main text
        p.setPen(foregroundColor);
        p.setRenderHint(QPainter::Antialiasing, false);
    }

    // Draw Name
    p.setFont(name_item.font);
    p.drawText(nameX, startY + nameFm.ascent(), nameText);

    // Draw Description
    p.setFont(description_item.font);
    QColor descColor = foregroundColor;
    descColor.setAlpha(180); // Slightly transparent for secondary text
    p.setPen(descColor);
    
    QString descText = index.data(PackageModel::DescriptionRole).toString();
    p.drawText(nameX, startY + nameFm.height() + 2 + descFm.ascent(), descText);

    QLinearGradient gradient;

    // Gradient part of the background - fading of the text at the end
    if (leftToRight) {
        gradient = QLinearGradient(left + width - m_spacing - 16 /*fade length*/, 0,
                                   left + width - m_spacing, 0);
        gradient.setColorAt(0, Qt::white);
        gradient.setColorAt(1, Qt::transparent);
    } else {
        gradient = QLinearGradient(left + m_spacing, 0,
                                   left + m_spacing + 16, 0);
        gradient.setColorAt(0, Qt::transparent);
        gradient.setColorAt(1, Qt::white);
    }

    // Only apply fade if text might overflow (simplification: always apply for now as in original)
    // Actually, let's skip the fade for the cleaner look unless strictly necessary, 
    // but keeping it to match original logic is safer for now.
    QRect paintRect = option.rect;
    p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    p.fillRect(paintRect, gradient);

    p.end();

    painter->drawPixmap(option.rect.topLeft(), pixmap);
}

void PackageDelegate::paintText(QPainter *painter, const QStyleOptionViewItem &option , const QModelIndex &index) const
{
    int state;
    QString text;
    QPen pen;
    KColorScheme color(option.palette.currentColorGroup());

    QColor foregroundColor = (option.state.testFlag(QStyle::State_Selected)) ?
                             option.palette.color(QPalette::HighlightedText) : option.palette.color(QPalette::Text);

    switch (index.column()) {
    case 1:
        state = index.data(PackageModel::StatusRole).toInt();

        if (state & QApt::Package::NowBroken) {
            text = MuonStrings::global()->packageStateName(QApt::Package::NowBroken);
            pen.setBrush(getStatusColor(QApt::Package::NowBroken, color));
            break;
        }

        if (state & QApt::Package::Installed) {
            if (state & QApt::Package::Upgradeable) {
                text = MuonStrings::global()->packageStateName(QApt::Package::Upgradeable);
                pen.setBrush(getStatusColor(QApt::Package::Upgradeable, color));
            } else {
                text = MuonStrings::global()->packageStateName(QApt::Package::Installed);
                pen.setBrush(getStatusColor(QApt::Package::Installed, color));
            }
        } else {
            text = MuonStrings::global()->packageStateName(QApt::Package::NotInstalled);
            pen.setBrush(getStatusColor(QApt::Package::NotInstalled, color));
        }
        break;
    case 2:
        state = index.data(PackageModel::ActionRole).toInt();

        if (state & QApt::Package::ToKeep) {
            text = MuonStrings::global()->packageStateName(QApt::Package::ToKeep);
            pen.setBrush(getActionColor(QApt::Package::ToKeep, color));
            // No other "To" flag will be set if we are keeping
            break;
        }

        if (state & QApt::Package::ToInstall) {
            text = MuonStrings::global()->packageStateName(QApt::Package::ToInstall);
            pen.setBrush(getActionColor(QApt::Package::ToInstall, color));
        }

        if (state & QApt::Package::ToUpgrade) {
            text = MuonStrings::global()->packageStateName(QApt::Package::ToUpgrade);
            pen.setBrush(getActionColor(QApt::Package::ToUpgrade, color));
            break;
        }

        if (state & QApt::Package::ToRemove) {
            text = MuonStrings::global()->packageStateName(QApt::Package::ToRemove);
            pen.setBrush(getActionColor(QApt::Package::ToRemove, color));
        }

        if (state & QApt::Package::ToPurge) {
            text = MuonStrings::global()->packageStateName(QApt::Package::ToPurge);
            pen.setBrush(getActionColor(QApt::Package::ToPurge, color));
            break;
        }

        if (state & QApt::Package::ToReInstall) {
            text = MuonStrings::global()->packageStateName(QApt::Package::ToReInstall);
            pen.setBrush(getActionColor(QApt::Package::ToReInstall, color));
            break;
        }

        if (state & QApt::Package::ToDowngrade) {
            text = MuonStrings::global()->packageStateName(QApt::Package::ToDowngrade);
            pen.setBrush(getActionColor(QApt::Package::ToDowngrade, color));
            break;
        }
        break;
    case 3:
        text = index.data(PackageModel::InstalledSizeDisplayRole).toString();
        pen.setBrush(foregroundColor);
        break;
    case 4:
        text = index.data(PackageModel::InstalledVersionRole).toString();
        pen.setBrush(foregroundColor);
        break;
    case 5:
        text = index.data(PackageModel::AvailableVersionRole).toString();
        pen.setBrush(foregroundColor);
        break;
    }

    QFont font = option.font;
    QFontMetrics fontMetrics(font);

    int x = option.rect.x() + m_spacing;
    int y = option.rect.y() + calcItemHeight(option) / 4 + fontMetrics.height() -1;
    int width = option.rect.width();

    painter->setPen(pen);
    painter->drawText(x, y, fontMetrics.elidedText(text, option.textElideMode, width));
}

QSize PackageDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);

    QSize size;
    QFontMetrics metric = QFontMetrics(option.font);

    switch (index.column()) {
    case 0:
        size.setWidth(metric.width(index.data(PackageModel::DescriptionRole).toString()));
        break;
    case 1:
        size.setWidth(metric.width(index.data(PackageModel::StatusRole).toString()));
        break;
    case 2:
        size.setWidth(metric.width(index.data(PackageModel::ActionRole).toString()));
        break;
    case 3:
        size.setWidth(metric.width(index.data(PackageModel::InstalledSizeDisplayRole).toString()));
        break;
    case 4:
        size.setWidth(metric.width(index.data(PackageModel::InstalledVersionRole).toString()));
        break;
    case 5:
        size.setWidth(metric.width(index.data(PackageModel::AvailableVersionRole).toString()));
        break;
    default:
        break;
    }
    size.setHeight(option.fontMetrics.height() * 2 + m_spacing * 3); // Increased height for margins

    return size;
}

int PackageDelegate::calcItemHeight(const QStyleOptionViewItem &option) const
{
    // Painting main column
    QStyleOptionViewItem name_item(option);
    QStyleOptionViewItem description_item(option);

    description_item.font.setPointSize(name_item.font.pointSize() - 1);

    int textHeight = QFontInfo(name_item.font).pixelSize() + QFontInfo(description_item.font).pixelSize();
    return qMax(textHeight, m_iconSize) + 2 * m_spacing;
}

QBrush PackageDelegate::getStatusColor(int packageState, const KColorScheme &colorScheme) const
{
    // Check if custom colors are configured
    QHash<int, QColor> customColors = parseCustomColors();
    
    // For status column (column 1)
    if (packageState & QApt::Package::NowBroken) {
        if (customColors.contains(QApt::Package::NowBroken)) {
            return QBrush(customColors[QApt::Package::NowBroken]);
        }
        return colorScheme.foreground(KColorScheme::NegativeText);
    }
    
    if (packageState & QApt::Package::Installed) {
        if (packageState & QApt::Package::Upgradeable) {
            if (customColors.contains(QApt::Package::Upgradeable)) {
                return QBrush(customColors[QApt::Package::Upgradeable]);
            }
            return colorScheme.foreground(KColorScheme::LinkText);
        }
        if (customColors.contains(QApt::Package::Installed)) {
            return QBrush(customColors[QApt::Package::Installed]);
        }
        return colorScheme.foreground(KColorScheme::PositiveText);
    }
    
    if (customColors.contains(QApt::Package::NotInstalled)) {
        return QBrush(customColors[QApt::Package::NotInstalled]);
    }
    return colorScheme.foreground(KColorScheme::NeutralText);
}

QBrush PackageDelegate::getActionColor(int packageState, const KColorScheme &colorScheme) const
{
    // Check if custom colors are configured
    QHash<int, QColor> customColors = parseCustomColors();
    
    // For action column (column 2)
    if (packageState & QApt::Package::ToKeep) {
        if (customColors.contains(QApt::Package::ToKeep)) {
            return QBrush(customColors[QApt::Package::ToKeep]);
        }
        return colorScheme.foreground(KColorScheme::NeutralText);
    }
    
    if (packageState & QApt::Package::ToInstall) {
        if (customColors.contains(QApt::Package::ToInstall)) {
            return QBrush(customColors[QApt::Package::ToInstall]);
        }
        return colorScheme.foreground(KColorScheme::PositiveText);
    }
    
    if (packageState & QApt::Package::ToUpgrade) {
        if (customColors.contains(QApt::Package::ToUpgrade)) {
            return QBrush(customColors[QApt::Package::ToUpgrade]);
        }
        return colorScheme.foreground(KColorScheme::LinkText);
    }
    
    if (packageState & QApt::Package::ToRemove) {
        if (customColors.contains(QApt::Package::ToRemove)) {
            return QBrush(customColors[QApt::Package::ToRemove]);
        }
        return colorScheme.foreground(KColorScheme::NegativeText);
    }
    
    if (packageState & QApt::Package::ToPurge) {
        if (customColors.contains(QApt::Package::ToPurge)) {
            return QBrush(customColors[QApt::Package::ToPurge]);
        }
        return colorScheme.foreground(KColorScheme::NegativeText);
    }
    
    if (packageState & QApt::Package::ToReInstall) {
        if (customColors.contains(QApt::Package::ToReInstall)) {
            return QBrush(customColors[QApt::Package::ToReInstall]);
        }
        return colorScheme.foreground(KColorScheme::PositiveText);
    }
    
    if (packageState & QApt::Package::ToDowngrade) {
        if (customColors.contains(QApt::Package::ToDowngrade)) {
            return QBrush(customColors[QApt::Package::ToDowngrade]);
        }
        return colorScheme.foreground(KColorScheme::LinkText);
    }
    
    return colorScheme.foreground(KColorScheme::NeutralText);
}

QHash<int, QColor> PackageDelegate::parseCustomColors() const
{
    QHash<int, QColor> customColors;
    QString colorConfig = MuonSettings::self()->statusColumnColors();
    
    if (colorConfig.isEmpty()) {
        return customColors;
    }
    
    // Parse format: "status:color;status:color"
    // Example: "Installed:#00FF00;Upgradeable:#FFAA00;ToRemove:#FF0000"
    QRegularExpression regex("(\\w+):(#?[0-9A-Fa-f]{6})");
    QRegularExpressionMatchIterator i = regex.globalMatch(colorConfig);
    
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString statusName = match.captured(1);
        QString colorString = match.captured(2);
        
        // Map status names to QApt::Package enum values
        int statusFlag = 0;
        if (statusName == "NowBroken") statusFlag = QApt::Package::NowBroken;
        else if (statusName == "Installed") statusFlag = QApt::Package::Installed;
        else if (statusName == "Upgradeable") statusFlag = QApt::Package::Upgradeable;
        else if (statusName == "NotInstalled") statusFlag = QApt::Package::NotInstalled;
        else if (statusName == "ToKeep") statusFlag = QApt::Package::ToKeep;
        else if (statusName == "ToInstall") statusFlag = QApt::Package::ToInstall;
        else if (statusName == "ToUpgrade") statusFlag = QApt::Package::ToUpgrade;
        else if (statusName == "ToRemove") statusFlag = QApt::Package::ToRemove;
        else if (statusName == "ToPurge") statusFlag = QApt::Package::ToPurge;
        else if (statusName == "ToReInstall") statusFlag = QApt::Package::ToReInstall;
        else if (statusName == "ToDowngrade") statusFlag = QApt::Package::ToDowngrade;
        
        if (statusFlag != 0) {
            QColor color(colorString);
            if (color.isValid()) {
                customColors[statusFlag] = color;
            }
        }
    }
    
    return customColors;
}
