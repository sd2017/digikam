/* ============================================================
 * Author: Gilles Caulier <caulier dot gilles at free.fr>
 * Date  : 2006-01-10
 * Description : a widget to display CIE tongue from
 * an ICC profile.
 * 
 * Copyright 2006 by Gilles Caulier
 *
 * Any source code are inspired from lprof project and
 * Copyright (C) 1998-2001 Marti Maria
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

#ifndef CIETONGUEWIDGET_H
#define CIETONGUEWIDGET_H

#include <config.h>

// Qt includes.

#include <qwidget.h>
#include <qcolor.h>
#include <qpainter.h>
#include <qpixmap.h>

// Local includes

#include "digikam_export.h"

// Others

#include LCMS_HEADER

namespace Digikam
{

class CIETongueWidgetPriv;

class DIGIKAM_EXPORT CIETongueWidget : public QWidget
{
Q_OBJECT

public:

    CIETongueWidget(int w, int h, QWidget *parent=0, cmsHPROFILE hMonitor=0);
    ~CIETongueWidget();

    void setProfileData(QByteArray *profileData=0);

protected:
    
    double        m_gridside;
    int           m_xBias;
    int           m_yBias;
    int           m_pxcols;
    int           m_pxrows;
    
    QPainter      m_pnt;
    QPixmap       m_pix;
    
protected:

    int  Grids(double d) const { return (int) floor(d * m_gridside + .5); };

    void OutlineTongue();
    void FillTongue();
    void DrawTongueAxis();
    void DrawTongueGrid();
    void DrawLabels();

    QRgb ColorByCoord(double x, double y);  
    void DrawSmallElipse(LPcmsCIExyY xyY, BYTE r, BYTE g, BYTE b, int sz);

    void paintEvent( QPaintEvent * );

private:

    void DrawColorantTriangle(void);
    void DrawWhitePoint(void);
    void DrawPatches(void);
    
    void MapPoint(int& icx, int& icy, LPcmsCIExyY xyY);
    void BiasedLine(int x1, int y1, int x2, int y2);
    void CIETongueWidget::BiasedText(int x, int y, QString Txt);

    void Sweep_sRGB(void);

private :

    CIETongueWidgetPriv* d;

};

}  // namespace Digikam

#endif /* CIETONGUEWIDGET_H */
