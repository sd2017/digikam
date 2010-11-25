/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2006-06-13
 * Description : A widget stack to embedded album content view
 *               or the current image preview.
 *
 * Copyright (C) 2006-2010 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef ALBUMWIDGETSTACK_H
#define ALBUMWIDGETSTACK_H

// Qt includes

#include <QByteArray>
#include <QMainWindow>
#include <QStackedWidget>
#include <QString>

// KDE includes

#include <kurl.h>

// Local includes

#include "digikam_export.h"
#include "imageinfo.h"
#include "thumbbardock.h"
//#include "mapwidgetview.h"

namespace Digikam
{

class DigikamImageView;
class ImagePreviewView;
class AlbumWidgetStackPriv;
class MapWidgetView;

class AlbumWidgetStack : public QStackedWidget
{
    Q_OBJECT

public:

    enum AlbumWidgetStackMode
    {
        PreviewAlbumMode=0,
        PreviewImageMode,
        WelcomePageMode,
        MediaPlayerMode,
        MapWidgetMode
    };

public:

    AlbumWidgetStack(QWidget* parent=0);
    ~AlbumWidgetStack();

    /* Attach the thumbnail dock widget to the specified QMainWindow. */
    void setDockArea(QMainWindow*);

    ThumbBarDock*     thumbBarDock();
    DigikamImageView* imageIconView();
    ImagePreviewView* imagePreviewView();
    MapWidgetView*      mapWidgetView();

    void setPreviewItem(const ImageInfo& info = ImageInfo(),
                        const ImageInfo& previous = ImageInfo(),
                        const ImageInfo& next = ImageInfo());
    int  previewMode();
    void setMapViewMode();
    void setIconViewMode();
    void setPreviewMode(int mode);
    void previewLoaded();

    void   increaseZoom();
    void   decreaseZoom();
    void   fitToWindow();
    void   toggleFitToWindowOr100();
    void   zoomTo100Percents();
    bool   maxZoom();
    bool   minZoom();
    void   setZoomFactor(double z);
    void   setZoomFactorSnapped(double z);
    double zoomFactor();
    double zoomMin();
    double zoomMax();

Q_SIGNALS:

    void signalNextItem();
    void signalPrevItem();
    void signalEditItem();
    void signalDeleteItem();
    void signalToggledToPreviewMode(bool);
    void signalBack2Album();
    void signalSlideShow();
    void signalZoomFactorChanged(double);
    void signalInsert2LightTable();
    void signalInsert2QueueMgr();
    void signalFindSimilar();
    void signalImageSelected(const ImageInfo& info);
    void signalAddToExistingQueue(int);

    void signalGotoAlbumAndItem(const ImageInfo&);
    void signalGotoDateAndItem(const ImageInfo&);
    void signalGotoTagAndItem(int);

public Q_SLOTS:

    void slotEscapePreview();

private Q_SLOTS:

    void slotZoomFactorChanged(double);

private:

    void readSettings();

private:

    AlbumWidgetStackPriv* const d;
};

}  // namespace Digikam

#endif /* ALBUMWIDGETSTACK_H */
