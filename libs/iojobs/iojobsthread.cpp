/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2015-06-15
 * Description : IO Jobs thread for file system jobs
 *
 * Copyright (C) 2015 by Mohamed Anwer <m dot anwer at gmx dot com>
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

#include "iojobsthread.h"
#include "iojob.h"
#include "imageinfo.h"
#include "dnotificationwrapper.h"
#include "digikam_debug.h"
#include "digikamapp.h"

namespace Digikam
{

class IOJobsThread::Private
{

public:

    Private()
        : isCanceled(false),
          isRenameThread(false),
          keepErrors(true)
    {
    }

    bool           isCanceled;
    bool           isRenameThread;
    QUrl           oldUrl;

    bool           keepErrors;
    QList<QString> errorsList;
};

IOJobsThread::IOJobsThread(QObject* const parent)
    : RActionThreadBase(parent),
      d(new Private)
{
}

IOJobsThread::~IOJobsThread()
{
    delete d;
}

void IOJobsThread::copy(const QList<QUrl>& srcFiles, const QUrl destAlbum)
{
    RJobCollection collection;

    foreach (const QUrl& url, srcFiles)
    {
        CopyJob* const j = new CopyJob(url, destAlbum, false);

        connectOneJob(j);

        collection.insert(j, 0);
    }

    appendJobs(collection);
}

void IOJobsThread::move(const QList<QUrl>& srcFiles, const QUrl destAlbum)
{
    RJobCollection collection;

    foreach (const QUrl& url, srcFiles)
    {
        CopyJob* const j = new CopyJob(url, destAlbum, true);

        connectOneJob(j);

        collection.insert(j, 0);
    }

    appendJobs(collection);
}

void IOJobsThread::del(const QList<QUrl>& srcsToDelete, bool useTrash)
{
    RJobCollection collection;

    foreach (const QUrl& url, srcsToDelete)
    {
        DeleteJob* const j = new DeleteJob(url, useTrash);

        connectOneJob(j);

        collection.insert(j, 0);
    }

    appendJobs(collection);
}

void IOJobsThread::renameFile(const QUrl& srcToRename, const QUrl& newName)
{
    RJobCollection collection;
    RenameFileJob* const j = new RenameFileJob(srcToRename, newName);

    connect(j, SIGNAL(error(QString)),
            this, SLOT(error(QString)));

    // Connecting directly to signal finished
    // because it's only one job
    connect(j, SIGNAL(signalDone()),
            this, SIGNAL(finished()));

    connect(j, SIGNAL(signalRenamed(QUrl,QUrl)),
            this, SIGNAL(renamed(QUrl,QUrl)));

    d->isRenameThread = true;
    d->oldUrl         = srcToRename;

    collection.insert(j, 0);
    appendJobs(collection);
}

bool IOJobsThread::isRenameThread()
{
   return d->isRenameThread;
}

QUrl IOJobsThread::oldUrlToRename()
{
    return d->oldUrl;
}

void IOJobsThread::cancel()
{
    d->isCanceled = true;
    RActionThreadBase::cancel();
}

bool IOJobsThread::isCanceled()
{
    return d->isCanceled;
}

bool IOJobsThread::hasErrors()
{
    return !d->errorsList.isEmpty();
}

void IOJobsThread::setKeepErrors(bool keepErrors)
{
    d->keepErrors = keepErrors;
}

bool IOJobsThread::isKeepingErrors()
{
    return d->keepErrors;
}

QList<QString>& IOJobsThread::errorsList()
{
    return d->errorsList;
}

void IOJobsThread::connectOneJob(IOJob * const j)
{
    connect(j, SIGNAL(error(QString)),
            this, SLOT(error(QString)));

    connect(j, SIGNAL(signalDone()),
            this, SLOT(oneJobFinished()));
}

void IOJobsThread::oneJobFinished()
{
    if(isEmpty())
        emit finished();
}

void IOJobsThread::error(const QString& errString)
{
    d->errorsList.append(errString);
}

} // namespace Digikam