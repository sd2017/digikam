/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-04-27
 * Description : a view for date albums.
 * 
 * Copyright (C) 2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2006-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Qt includes.

#include <Q3ListView>
#include <QDateTime>
#include <QFont>
#include <QPainter>
#include <QStyle>
#include <QFileInfo>

// KDE includes.

#include <kapplication.h>
#include <klocale.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kconfig.h>
#include <kdeversion.h>
#include <kcalendarsystem.h>
#include <kconfiggroup.h>

// Local includes.

#include "ddebug.h"
#include "album.h"
#include "albumdb.h"
#include "albumsettings.h"
#include "monthwidget.h"
#include "folderview.h"
#include "datefolderview.h"
#include "datefolderview.moc"

namespace Digikam
{

DateFolderItem::DateFolderItem(Q3ListView* parent, const QString& name)
              : FolderItem(parent, name, true)
{
    m_count = 0;
    m_name  = name;
    m_album = 0;
}

DateFolderItem::DateFolderItem(Q3ListViewItem* parent, const QString& name, DAlbum* album)
              : FolderItem(parent, name)
{
    m_count = 0;
    m_name  = name;
    m_album = album;
}

DateFolderItem::~DateFolderItem()
{
}

void DateFolderItem::refresh()
{
    if (AlbumSettings::instance()->getShowFolderTreeViewItemsCount())
    {
        setText(0, QString("%1 (%2)").arg(m_name).arg(m_count));
    }
    else
    {
        setText(0, m_name);
    }
}

int DateFolderItem::compare(Q3ListViewItem* i, int , bool ) const
{
    if (!i)
        return 0;

    DateFolderItem* dItem = dynamic_cast<DateFolderItem*>(i);
    if (!dItem || !dItem->m_album)
    {
        return text(0).localeAwareCompare(i->text(0));
    }

    if (m_album->date() == dItem->m_album->date())
        return 0;
    else if (m_album->date() > dItem->m_album->date())
        return 1;
    else
        return -1;
}

QString DateFolderItem::date() const
{
    // If an album is set, return it's date, otherwise just the year
    return m_album ? m_album->date().toString() : m_name;
}

QString DateFolderItem::name() const
{
    return m_name;
}

DAlbum* DateFolderItem::album() const
{
    return m_album;
}

int DateFolderItem::count() const
{
    return m_count;
}

void DateFolderItem::setCount(int v)
{
    m_count = v;
    refresh();
}

// -----------------------------------------------------------------

class DateFolderViewPriv
{
public:

    DateFolderViewPriv()
    {
        active    = false;
        listview  = 0;
        monthview = 0;
    }

    bool         active;
    
    QString      selected;

    FolderView  *listview;
    MonthWidget *monthview;    
};

DateFolderView::DateFolderView(QWidget* parent)
              : KVBox(parent)
{
    d = new DateFolderViewPriv;
    d->listview  = new FolderView(this);
    d->monthview = new MonthWidget(this);

    d->listview->addColumn(i18n("My Calendar"));
    d->listview->setResizeMode(Q3ListView::LastColumn);
    d->listview->setRootIsDecorated(true);

    connect(AlbumManager::instance(), SIGNAL(signalAlbumAdded(Album*)),
            this, SLOT(slotAlbumAdded(Album*)));

    connect(AlbumManager::instance(), SIGNAL(signalAlbumDeleted(Album*)),
            this, SLOT(slotAlbumDeleted(Album*)));

    connect(AlbumManager::instance(), SIGNAL(signalAllDAlbumsLoaded()),
            this, SLOT(slotAllDAlbumsLoaded()));    

    connect(AlbumManager::instance(), SIGNAL(signalAlbumsCleared()),
            d->listview, SLOT(clear()));

    connect(AlbumManager::instance(), SIGNAL(signalDAlbumsDirty(const QMap<YearMonth, int>&)),
            this, SLOT(slotRefresh(const QMap<YearMonth, int>&)));

    connect(d->listview, SIGNAL(selectionChanged()),
            this, SLOT(slotSelectionChanged()));
}

DateFolderView::~DateFolderView()
{
    saveViewState();
    delete d;
}

void DateFolderView::setActive(bool val)
{
    if (d->active == val)
        return;
    
    d->active = val;
    if (d->active)
    {
        slotSelectionChanged();
    }
    else
    {
        d->monthview->setActive(false);
    }
}

void DateFolderView::slotAllDAlbumsLoaded()
{
    disconnect(AlbumManager::instance(), SIGNAL(signalAllDAlbumsLoaded()),
               this, SLOT(slotAllDAlbumsLoaded()));
    loadViewState();
}

void DateFolderView::slotAlbumAdded(Album* a)
{
    if (!a || a->type() != Album::DATE)
        return;

    DAlbum* album = (DAlbum*)a;
    
    QDate date = album->date();

    QString yr = QString::number(date.year());
    
    QString mo = KGlobal::locale()->calendar()->monthName(date, KCalendarSystem::LongName);
    Q3ListViewItem* parent = findRootItemByYear(yr);
    if (!parent)
    {
        parent = new DateFolderItem(d->listview, yr);
        parent->setPixmap(0, SmallIcon("go-jump-today", AlbumSettings::instance()->getDefaultTreeIconSize()));
    }

    DateFolderItem* item = new DateFolderItem(parent, mo, album);
    item->setPixmap(0, SmallIcon("go-jump-today", AlbumSettings::instance()->getDefaultTreeIconSize()));

    album->setExtraData(this, item);
}

void DateFolderView::slotAlbumDeleted(Album* a)
{
    if (!a || a->type() != Album::DATE)
        return;

    DAlbum* album = (DAlbum*)a;

    DateFolderItem* item = (DateFolderItem*) album->extraData(this);
    if (item)
    {
        delete item;
        album->removeExtraData(this);
    }
}

void DateFolderView::slotSelectionChanged()
{
    if (!d->active)
        return;
    
    Q3ListViewItem* selItem = 0;
    
    Q3ListViewItemIterator it( d->listview );
    while (it.current())
    {
        if (it.current()->isSelected())
        {
            selItem = it.current();
            break;
        }
        ++it;
    }

    d->monthview->setActive(false);
    
    if (!selItem)
    {
        AlbumManager::instance()->setCurrentAlbum(0);
        return;
    }

    DateFolderItem* dateItem = dynamic_cast<DateFolderItem*>(selItem);
    
    if (!dateItem || !dateItem->album())
    {
        AlbumManager::instance()->setCurrentAlbum(0);
        d->monthview->setActive(false);
    }
    else
    {
        AlbumManager::instance()->setCurrentAlbum(dateItem->album());

        QDate date = dateItem->album()->date();        
        d->monthview->setActive(true);
        d->monthview->setYearMonth(date.year(), date.month());
    }
}

void DateFolderView::loadViewState()
{
    KSharedConfig::Ptr config = KGlobal::config();
    KConfigGroup group = config->group(objectName());
    
    QString selected;
    if(group.hasKey("LastSelectedItem"))
    {
        selected = group.readEntry("LastSelectedItem", QString());
    }

    QStringList openFolders;
    if(group.hasKey("OpenFolders"))
    {
        openFolders = group.readEntry("OpenFolders", QStringList());
    }
    
    DateFolderItem *item;
    QString id;
    Q3ListViewItemIterator it(d->listview);
    for( ; it.current(); ++it)
    {        
        item = dynamic_cast<DateFolderItem*>(it.current());
        id = item->date();
        if(openFolders.contains(id))
            d->listview->setOpen(item, true);
        else
            d->listview->setOpen(item, false);
        
        if(id == selected)
            d->listview->setSelected(item, true);
    }    
}

void DateFolderView::gotoDate(const QDate& dt)
{
    DateFolderItem *item = 0;
    QDate           id;

    QDate date = QDate(dt.year(), dt.month(), 1);

    // Find that date in the side-bar list.
    Q3ListViewItemIterator it(d->listview);
    for( ; it.current(); ++it)
    {
        item = dynamic_cast<DateFolderItem*>(it.current());
        if (item->album())
        {
            id = item->album()->date();
            if(id == date)
            {
                d->listview->setSelected(item, true);
                d->listview->ensureItemVisible(item);
            }
        }
    }
}

void DateFolderView::saveViewState()
{
    KSharedConfig::Ptr config = KGlobal::config();
    KConfigGroup group = config->group(objectName());
   
    DateFolderItem *item = dynamic_cast<DateFolderItem*>(d->listview->selectedItem());
    if(item)
        group.writeEntry("LastSelectedItem", item->date());
    
    QStringList openFolders;
    Q3ListViewItemIterator it(d->listview);
    item = dynamic_cast<DateFolderItem*>(d->listview->firstChild());
    while(item)
    {
        // Storing the years only, a month cannot be open
        if(item && d->listview->isOpen(item))
            openFolders.push_back(item->date());
        item = dynamic_cast<DateFolderItem*>(item->nextSibling());
    }
    group.writeEntry("OpenFolders", openFolders);
}

void DateFolderView::setSelected(Q3ListViewItem *item)
{
    if(!item)
        return;
    
    d->listview->setSelected(item, true);
    d->listview->ensureItemVisible(item);
}

Q3ListViewItem *DateFolderView::findRootItemByYear(const QString& year)
{
    Q3ListViewItemIterator it(d->listview);
    
    while (it.current())
    {
        DateFolderItem* item = dynamic_cast<DateFolderItem*>(*it);
        if (item)
        {
            if (!item->album() && item->name() == year)
                return item;
        }
        ++it;
    }
    return 0;
}

void DateFolderView::refresh()
{
    Q3ListViewItemIterator it(d->listview);
    
    while (it.current())
    {
        DateFolderItem* item = dynamic_cast<DateFolderItem*>(*it);
        if (item)
            item->refresh();
        ++it;
    }
}

void DateFolderView::slotRefresh(const QMap<YearMonth, int>& yearMonthMap)
{
    Q3ListViewItemIterator it(d->listview);
    
    while (it.current())
    {
        DateFolderItem* item = dynamic_cast<DateFolderItem*>(*it);
        if (item)
        {
            if (item->album())
            {
                QDate date = item->album()->date();
                QMap<YearMonth, int>::const_iterator it2 = yearMonthMap.find(YearMonth(date.year(), date.month()));
                if ( it2 != yearMonthMap.end() )
                    item->setCount(it2.data());
            }
            else
            {
                QString year = item->date();
                int count    = 0;
                for ( QMap<YearMonth, int>::const_iterator it2 = yearMonthMap.begin();
                      it2 != yearMonthMap.end(); ++it2 )
                {
                    if (QString::number(it2.key().first) == year)
                        count += it2.data();
                }
                item->setCount(count);
            }   
        }
        ++it;
    }
}

}  // namespace Digikam
