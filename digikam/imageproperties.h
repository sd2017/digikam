/* ============================================================
 * Author: Caulier Gilles <caulier dot gilles at free.fr>
 * Date  : 2004-11-17
 * Description :
 *
 * Copyright 2004 by Gilles Caulier
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
 
#ifndef IMAGEPROPERTIES_H
#define IMAGEPROPERTIES_H

// KDE includes.

#include <kdialogbase.h>
#include <kurl.h>

class AlbumIconView;
class AlbumIconItem;
class ImagePropertiesGeneral;
class ImagePropertiesEXIF;
class ImagePropertiesHistogram;

class ImageProperties : public KDialogBase
{
    Q_OBJECT

public:

    enum Mode
    {
        SINGLE,
        MULTI
    };

    // Multiple file mode (to be called from albumiconview)
    ImageProperties(AlbumIconView* view, AlbumIconItem* currItem);

    // Single file mode (to be called from elsewhere)
    ImageProperties(QWidget* parent, const KURL& url, QRect* selectionArea=0);
                    
    ~ImageProperties();

private:

    AlbumIconView                *m_view;
    AlbumIconItem                *m_currItem;
    Mode                          m_mode;
    KURL                          m_currURL;
    QRect*                        m_selectionArea;

    ImagePropertiesGeneral*       m_generalPage;
    ImagePropertiesEXIF*          m_exifPage;
    ImagePropertiesHistogram*     m_histogramPage;
    
    void setupGui(void);

private slots:

    void slotItemChanged();
    void slotUser1();
    void slotUser2();
};

#endif  // IMAGEPROPERTIES_H
