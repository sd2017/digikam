/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2004-08-26
 * Description : a digiKam image editor plugin for add film
 *               grain on an image.
 *
 * Copyright (C) 2004-2016 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#ifndef FILMGRAINTOOL_H
#define FILMGRAINTOOL_H

// Local includes

#include "editortool.h"

namespace Digikam
{

class FilmGrainTool : public EditorToolThreaded
{
    Q_OBJECT

public:

    explicit FilmGrainTool(QObject* const parent);
    ~FilmGrainTool();

private Q_SLOTS:

    void slotResetSettings();

private:

    void readSettings();
    void writeSettings();
    void preparePreview();
    void prepareFinal();
    void setPreviewImage();
    void setFinalImage();
    void renderingFinished();

private:

    class Private;
    Private* const d;
};

}  // namespace Digikam

#endif /* FILMGRAINTOOL_H */
