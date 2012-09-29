/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2012-08-21
 * Description : Overlays for the import interface
 *
 * Copyright (C) 2012 by Islam Wazery <wazery at ubuntu dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "importoverlays.moc"

// KDE includes

#include <kiconloader.h>
#include <klocale.h>
#include <kglobalsettings.h>

// Local includes

#include "importcategorizedview.h"
#include "importdelegate.h"
#include "camiteminfo.h"
#include "ratingwidget.h"

namespace Digikam
{

ImportOverlayWidget::ImportOverlayWidget(QWidget* const parent)
    : QAbstractButton(parent)
{
}

void ImportOverlayWidget::paintEvent(QPaintEvent*)
{
}

// -- Lock Overlay ------------------------------------------------------------------

ImportLockOverlay::ImportLockOverlay(QObject* const parent)
    : AbstractWidgetDelegateOverlay(parent)
{
}

ImportOverlayWidget* ImportLockOverlay::buttonWidget() const
{
    return static_cast<ImportOverlayWidget*>(m_widget);
}

QWidget* ImportLockOverlay::createWidget()
{
    QAbstractButton* button = new ImportOverlayWidget(parentWidget());
    //button->setCursor(Qt::PointingHandCursor);
    return button;
}

void ImportLockOverlay::setActive(bool active)
{
    AbstractWidgetDelegateOverlay::setActive(active);
}

void ImportLockOverlay::visualChange()
{
    if (m_widget && m_widget->isVisible())
    {
        updatePosition();
    }
}

void ImportLockOverlay::updatePosition()
{
    if (!m_index.isValid())
    {
        return;
    }

    QRect rect       = static_cast<ImportDelegate*>(delegate())->lockIndicatorRect();
    QRect visualRect = m_view->visualRect(m_index);
    rect.translate(visualRect.topLeft());

    m_widget->setFixedSize(rect.width() + 1, rect.height() + 1);
    m_widget->move(rect.topLeft());
}

bool ImportLockOverlay::checkIndex(const QModelIndex& index) const
{
    CamItemInfo info = ImportImageModel::retrieveCamItemInfo(index);

    if (info.writePermissions == 0)
    {
        m_widget->setToolTip(i18nc("@info:tooltip", "This item is locked."));
        return true;
    }

    if (info.writePermissions == 1)
    {
        m_widget->setToolTip(i18nc("@info:tooltip", "This item is not locked."));
        return true;
    }

    return false;
}

void ImportLockOverlay::slotEntered(const QModelIndex& index)
{
    AbstractWidgetDelegateOverlay::slotEntered(index);
    m_index = index;
    updatePosition();
}

// -- Download Overlay ------------------------------------------------------------------

ImportDownloadOverlay::ImportDownloadOverlay(QObject* const parent)
    : AbstractWidgetDelegateOverlay(parent)
{
}

ImportOverlayWidget* ImportDownloadOverlay::buttonWidget() const
{
    return static_cast<ImportOverlayWidget*>(m_widget);
}

QWidget* ImportDownloadOverlay::createWidget()
{
    QAbstractButton* button = new ImportOverlayWidget(parentWidget());
    //button->setCursor(Qt::PointingHandCursor);
    return button;
}

void ImportDownloadOverlay::setActive(bool active)
{
    AbstractWidgetDelegateOverlay::setActive(active);
}

void ImportDownloadOverlay::visualChange()
{
    if (m_widget && m_widget->isVisible())
    {
        updatePosition();
    }
}

void ImportDownloadOverlay::updatePosition()
{
    if (!m_index.isValid())
    {
        return;
    }

    QRect rect       = static_cast<ImportDelegate*>(delegate())->downloadIndicatorRect();
    QRect visualRect = m_view->visualRect(m_index);
    rect.translate(visualRect.topLeft());

    m_widget->setFixedSize(rect.width() + 1, rect.height() + 1);
    m_widget->move(rect.topLeft());
}

bool ImportDownloadOverlay::checkIndex(const QModelIndex& index) const
{
    CamItemInfo info = ImportImageModel::retrieveCamItemInfo(index);

    if (info.downloaded == CamItemInfo::DownloadUnknown)
    {
        m_widget->setToolTip(i18nc("@info:tooltip", "This item download status is unknown"));
        return true;
    }

    if (info.downloaded == CamItemInfo::NewPicture)
    {
        m_widget->setToolTip(i18nc("@info:tooltip", "This item download status is new"));
        return true;
    }

    if (info.downloaded == CamItemInfo::DownloadedYes)
    {
        m_widget->setToolTip(i18nc("@info:tooltip", "This item download status is downloaded"));
        return true;
    }

    return false;
}

void ImportDownloadOverlay::slotEntered(const QModelIndex& index)
{
    AbstractWidgetDelegateOverlay::slotEntered(index);
    m_index = index;
    updatePosition();
}

// -- Rating Overlay ------------------------------------------------------------------

ImportRatingOverlay::ImportRatingOverlay(QObject* parent)
    : AbstractWidgetDelegateOverlay(parent)
{
}

RatingWidget* ImportRatingOverlay::ratingWidget() const
{
    return static_cast<RatingWidget*>(m_widget);
}

QWidget* ImportRatingOverlay::createWidget()
{
    const bool animate = KGlobalSettings::graphicEffectsLevel() & KGlobalSettings::SimpleAnimationEffects;
    RatingWidget* w    = new RatingWidget(parentWidget());
    w->setFading(animate);
    w->setTracking(false);
    return w;
}

void ImportRatingOverlay::setActive(bool active)
{
    AbstractWidgetDelegateOverlay::setActive(active);

    if (active)
    {
        connect(ratingWidget(), SIGNAL(signalRatingChanged(int)),
                this, SLOT(slotRatingChanged(int)));

        if (view()->model())
            connect(view()->model(), SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                    this, SLOT(slotDataChanged(QModelIndex,QModelIndex)));
    }
    else
    {
        // widget is deleted

        if (view() && view()->model())
        {
            disconnect(view()->model(), 0, this, 0);
        }
    }
}

void ImportRatingOverlay::visualChange()
{
    if (m_widget && m_widget->isVisible())
    {
        updatePosition();
    }
}

void ImportRatingOverlay::widgetEnterEvent()
{
    widgetEnterNotifyMultiple(m_index);
}

void ImportRatingOverlay::widgetLeaveEvent()
{
    widgetLeaveNotifyMultiple();
}

void ImportRatingOverlay::hide()
{
    delegate()->setRatingEdited(QModelIndex());
    AbstractWidgetDelegateOverlay::hide();
}

void ImportRatingOverlay::updatePosition()
{
    if (!m_index.isValid())
    {
        return;
    }

    QRect rect = delegate()->ratingRect();

    if (rect.width() > ratingWidget()->maximumVisibleWidth())
    {
        int offset = (rect.width() - ratingWidget()->maximumVisibleWidth()) / 2;
        rect.adjust(offset, 0, -offset, 0);
    }

    QRect visualRect = m_view->visualRect(m_index);
    rect.translate(visualRect.topLeft());

    m_widget->setFixedSize(rect.width() + 1, rect.height() + 1);
    m_widget->move(rect.topLeft());
}

void ImportRatingOverlay::updateRating()
{
    if (!m_index.isValid())
    {
        return;
    }

    ImportImageModel* model = m_index.data(ImportImageModel::ImportImageModelPointerRole).value<ImportImageModel*>();
    ratingWidget()->setRating(model->camItemInfoRef(m_index).rating);
}

void ImportRatingOverlay::slotRatingChanged(int rating)
{
    if (m_widget && m_widget->isVisible() && m_index.isValid())
    {
        emit ratingEdited(affectedIndexes(m_index), rating);
    }
}

void ImportRatingOverlay::slotEntered(const QModelIndex& index)
{
    AbstractWidgetDelegateOverlay::slotEntered(index);

    // see bug 228810, this is a small workaround
    if (m_widget && m_widget->isVisible() && m_index.isValid() && index == m_index)
    {
        ratingWidget()->setVisibleImmediately();
    }

    m_index = index;

    updatePosition();
    updateRating();

    delegate()->setRatingEdited(m_index);
    view()->update(m_index);
}

void ImportRatingOverlay::slotDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
    if (m_widget && m_widget->isVisible() && QItemSelectionRange(topLeft, bottomRight).contains(m_index))
    {
        updateRating();
    }
}

// -- Rotate Overlay ----------------------------------------------------------------

ImportRotateOverlayButton::ImportRotateOverlayButton(ImportRotateOverlayDirection dir, QAbstractItemView* const parentView)
    : ItemViewHoverButton(parentView),
      m_direction(dir)
{
}

QSize ImportRotateOverlayButton::sizeHint() const
{
    return QSize(16, 16);
}

QPixmap ImportRotateOverlayButton::icon()
{
    if (m_direction == ImportRotateOverlayLeft)
    {
        return KIconLoader::global()->loadIcon("object-rotate-left", KIconLoader::NoGroup, KIconLoader::SizeSmall);
    }
    else
    {
        return KIconLoader::global()->loadIcon("object-rotate-right", KIconLoader::NoGroup, KIconLoader::SizeSmall);
    }
}

void ImportRotateOverlayButton::updateToolTip()
{
    if (m_direction == ImportRotateOverlayLeft)
    {
        setToolTip(i18nc("@info:tooltip", "Rotate Left"));
    }
    else
    {
        setToolTip(i18nc("@info:tooltip", "Rotate Right"));
    }
}

// --------------------------------------------------------------------

ImportRotateOverlay::ImportRotateOverlay(ImportRotateOverlayDirection dir, QObject* const parent)
    : HoverButtonDelegateOverlay(parent),
      m_direction(dir)
{
}

void ImportRotateOverlay::setActive(bool active)
{
    HoverButtonDelegateOverlay::setActive(active);

    if (active)
    {
        connect(button(), SIGNAL(clicked(bool)),
                this, SLOT(slotClicked()));
    }
}

ItemViewHoverButton* ImportRotateOverlay::createButton()
{
    return new ImportRotateOverlayButton(m_direction, view());
}

void ImportRotateOverlay::updateButton(const QModelIndex& index)
{
    const QRect rect = m_view->visualRect(index);
    const int gap    = 5;
    const int x      = rect.right() - (isLeft() ? (2*gap + 64) : (gap + 51));
    const int y      = rect.top() + gap;
    button()->move(QPoint(x, y));
}

void ImportRotateOverlay::slotClicked()
{
    QModelIndex index = button()->index();

    if (index.isValid())
    {
        emit signalRotate(affectedIndexes(index));
    }
}

bool ImportRotateOverlay::checkIndex(const QModelIndex& index) const
{
    CamItemInfo info = ImportImageModel::retrieveCamItemInfo(index);
    return (info.mime.contains("image/"));
}

void ImportRotateOverlay::widgetEnterEvent()
{
    widgetEnterNotifyMultiple(button()->index());
}

void ImportRotateOverlay::widgetLeaveEvent()
{
    widgetLeaveNotifyMultiple();
}

} // namespace Digikam