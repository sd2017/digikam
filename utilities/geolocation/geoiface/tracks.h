/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2006-09-19
 * @brief  Track file loading and managing
 *
 * @author Copyright (C) 2006-2016 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
 * @author Copyright (C) 2010-2014 by Michael G. Hansen
 *         <a href="mailto:mike at mghansen dot de">mike at mghansen dot de</a>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef TRACKS_H
#define TRACKS_H

// Qt includes

#include <QColor>
#include <QDateTime>
#include <QUrl>

// local includes

#include "geoiface_types.h"
#include "geocoordinates.h"
#include "digikam_export.h"

class TestGPXParsing;

namespace GeoIface
{

class DIGIKAM_EXPORT TrackManager : public QObject
{
    Q_OBJECT

public:

    class  TrackPoint
    {
    public:

        TrackPoint()
          : dateTime(),
            coordinates(),
            nSatellites(-1),
            hDop(-1),
            pDop(-1),
            fixType(-1),
            speed(-1)
        {
        }

        static bool EarlierThan(const TrackPoint& a, const TrackPoint& b);

    public:

        QDateTime                 dateTime;
        GeoCoordinates            coordinates;
        int                       nSatellites;
        qreal                     hDop;
        qreal                     pDop;
        int                       fixType;
        qreal                     speed;

        typedef QList<TrackPoint> List;
    };

    // -------------------------------------

    // We assume here that we will never load more than uint32_max tracks.
    typedef quint32 Id;

    class Track
    {
    public:

        enum Flags
        {
            FlagVisible = 1,
            FlagDefault = FlagVisible
        };

    public:

        Track()
          : url(),
            points(),
            id(0),
            color(Qt::red),
            flags(FlagDefault)
        {
            qRegisterMetaType<TrackChanges>("TrackChanges");
        }

        QUrl                 url;
        QList<TrackPoint>    points;
        /// 0 means no track id assigned yet
        Id                   id;
        QColor               color;
        Flags                flags;

        typedef QList<Track> List;
    };

    enum ChangeFlag
    {
        ChangeTrackPoints = 1,
        ChangeMetadata    = 2,

        ChangeRemoved     = 4,
        ChangeAdd         = ChangeTrackPoints | ChangeMetadata
    };

    typedef QPair<Id, ChangeFlag> TrackChanges;

public:

    explicit TrackManager(QObject* const parent = 0);
    virtual ~TrackManager();

    void loadTrackFiles(const QList<QUrl>& urls);
    QList<QPair<QUrl, QString> > readLoadErrors();
    void clear();

    const Track& getTrack(const int index) const;
    Track::List getTrackList() const;
    int trackCount() const;

    quint64 getNextFreeTrackId();
    Track   getTrackById(const quint64 trackId) const;
    QColor  getNextFreeTrackColor();

    void setVisibility(const bool value);
    bool getVisibility() const;

Q_SIGNALS:

    void signalTrackFilesReadyAt(const int startIndex, const int endIndex);
    void signalAllTrackFilesReady();
    void signalTracksChanged(const QList<TrackManager::TrackChanges> trackChanges);
    void signalVisibilityChanged(const bool newValue);

private Q_SLOTS:

    void slotTrackFilesReadyAt(int beginIndex, int endIndex);
    void slotTrackFilesFinished();

private:

    class Private;
    const QScopedPointer<Private> d;
};

} // namespace GeoIface

#endif // TRACKS_H
