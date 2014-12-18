/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2009-02-04
 * Description : a command line tool to load PGF data and convert to QImage
 *
 * Copyright (C) 2011-2014 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// Qt includes

#include <QFile>
#include <QIODevice>
#include <QImage>

// Local includes

#include "digikam_debug.h"
#include "pgfutils.h"

using namespace Digikam;

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "loadpgfdata - Load PGF data and save to PNG";
        qCDebug(DIGIKAM_GENERAL_LOG) << "Usage: <pgffile>";
        return -1;
    }

    qCDebug(DIGIKAM_GENERAL_LOG) << "Using LibPGF version: " << PGFUtils::libPGFVersion();
    QImage img;

    // Write PGF file.

    QString fname(argv[1]);
    QFile   file(fname);

    if ( !file.open(QIODevice::ReadOnly) )
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Cannot open PGF file to read...";
        return -1;
    }

    QByteArray data(file.size(), '\x00');
    QDataStream stream(&file);
    stream.readRawData(data.data(), data.size());

    // PGF => QImage conversion

    if (!PGFUtils::readPGFImageData(data, img))
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "loadPGFScaled failed...";
        return -1;
    }

    img.save(file.fileName() + QString("-converted.png"), "PNG");

    return 0;
}
