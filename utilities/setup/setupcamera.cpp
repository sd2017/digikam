/* ============================================================
 * File  : setupcamera.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-02-10
 * Description : 
 * 
 * Copyright 2003 by Renchi Raju

 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published bythe Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */

#include <qgroupbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qlistview.h>

#include <klocale.h>
#include <kmessagebox.h>

#include "setupcamera.h"
#include "cameraselection.h"
#include "cameralist.h"
#include "cameratype.h"
#include "gpiface.h"

SetupCamera::SetupCamera( QWidget* parent )
    : QWidget( parent )
{
    QVBoxLayout* vbox = new QVBoxLayout( this, 5, 5 ); 

    QGroupBox* groupBox = new QGroupBox( this, "groupBox" );
    groupBox->setColumnLayout(0, Qt::Vertical );
    groupBox->layout()->setSpacing( 5 );
    groupBox->layout()->setMargin( 5 );
    QGridLayout* groupBoxLayout = new QGridLayout( groupBox->layout() );
    groupBoxLayout->setAlignment( Qt::AlignTop );

    listView_ = new QListView( groupBox );
    listView_->addColumn( i18n("Title") );
    listView_->addColumn( i18n("Model") );
    listView_->addColumn( i18n("Port") );
    listView_->addColumn( i18n("Path") );
    listView_->setAllColumnsShowFocus(true); 
    groupBoxLayout->addMultiCellWidget( listView_, 0, 4, 0, 0 );

    addButton_ = new QPushButton( groupBox );
    groupBoxLayout->addWidget( addButton_, 0, 1 );

    removeButton_ = new QPushButton( groupBox );
    groupBoxLayout->addWidget( removeButton_, 1, 1 );

    editButton_ = new QPushButton( groupBox );
    groupBoxLayout->addWidget( editButton_, 2, 1 );

    autoDetectButton_ = new QPushButton( groupBox );
    groupBoxLayout->addWidget( autoDetectButton_, 3, 1 );

    addButton_->setText( i18n( "Add..." ) );
    removeButton_->setText( i18n( "Remove" ) );
    editButton_->setText( i18n( "Edit..." ) );
    autoDetectButton_->setText( i18n( "Auto-Detect" ) );

    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum,
                                           QSizePolicy::Expanding );
    groupBoxLayout->addItem( spacer, 4, 1 );
    vbox->addWidget( groupBox );

    // Initialize buttons

    removeButton_->setEnabled(false);
    editButton_->setEnabled(false);

    // connections

    connect(listView_, SIGNAL(selectionChanged()),
            this,      SLOT(slotSelectionChanged()));
    
    connect(addButton_, SIGNAL(clicked()),
            this, SLOT(slotAddCamera()));
    connect(removeButton_, SIGNAL(clicked()),
            this, SLOT(slotRemoveCamera()));
    connect(editButton_, SIGNAL(clicked()),
            this, SLOT(slotEditCamera()));
    connect(autoDetectButton_, SIGNAL(clicked()),
            this, SLOT(slotAutoDetectCamera()));

    // Add cameras --------------------------------------

    CameraList* clist = CameraList::instance();
    if (clist) {

        QPtrList<CameraType>* cl = clist->cameraList();
        for (CameraType *ctype = cl->first(); ctype;
             ctype = cl->next()) {
            new QListViewItem(listView_, ctype->title(), ctype->model(),
                              ctype->port(), ctype->path());
        }
    }
}

SetupCamera::~SetupCamera()
{
}

void SetupCamera::slotSelectionChanged()
{
    QListViewItem *item = listView_->selectedItem();
    
    if (!item) {
        removeButton_->setEnabled(false);
        editButton_->setEnabled(false);
        return;
    }

    removeButton_->setEnabled(true);
    editButton_->setEnabled(true);
}

void SetupCamera::slotAddCamera()
{
    CameraSelection *select = new CameraSelection;

    connect(select, SIGNAL(signalOkClicked(const QString&,
                                           const QString&,
                                           const QString&,
                                           const QString&)),
            this,   SLOT(slotAddedCamera(const QString&,
                                         const QString&,
                                         const QString&,
                                         const QString&)));
    select->show();
}

void SetupCamera::slotRemoveCamera()
{
    QListViewItem *item = listView_->currentItem();
    if (!item) return;

    delete item;
}

void SetupCamera::slotEditCamera()
{
    QListViewItem *item = listView_->currentItem();
    if (!item) return;
    
    CameraSelection *select = new CameraSelection;
    select->setCamera(item->text(0), item->text(1),
                      item->text(2), item->text(3));
    connect(select, SIGNAL(signalOkClicked(const QString&,
                                           const QString&,
                                           const QString&,
                                           const QString&)),
            this,   SLOT(slotEditedCamera(const QString&,
                                          const QString&,
                                          const QString&,
                                          const QString&)));
    select->show();
}

void SetupCamera::slotAutoDetectCamera()
{
    QString model, port;

    if (GPIface::autoDetect(model, port) != 0) {
        KMessageBox::error(this,i18n("Failed to auto-detect camera!\n"
                                     "Please retry or try setting manually."));
        return;
    }

    bool found = false;
    CameraList* clist = CameraList::instance();
    if (clist) {
        if (clist->find(model))
            found = true;
    }

    if (found) {
        KMessageBox::information(this, i18n("Already added camera: ")
                                 + model + " (" + port + ")");
    }
    else {
        KMessageBox::information(this, i18n("Found camera: ") + model
                                 + " (" + port + ")");
        new QListViewItem(listView_, model, model, port, "/");
    }
}



void SetupCamera::slotAddedCamera(const QString& title,
                                  const QString& model,
                                  const QString& port,
                                  const QString& path)
{
    new QListViewItem(listView_, title, model, port, path);
}

void SetupCamera::slotEditedCamera(const QString& title,
                                  const QString& model,
                                  const QString& port,
                                  const QString& path)
{
    QListViewItem *item = listView_->currentItem();
    if (!item) return;

    item->setText(0, title);
    item->setText(1, model);
    item->setText(2, port);
    item->setText(3, path);
}

void SetupCamera::applySettings()
{
    CameraList* clist = CameraList::instance();
    if (clist) {
        clist->clear();

        QListViewItemIterator it(listView_);
        for ( ; it.current(); ++it ) {
            QListViewItem *item = it.current();
            CameraType *ctype = new CameraType(item->text(0),
                                               item->text(1),
                                               item->text(2),
                                               item->text(3));
            clist->insert(ctype);
        }
    }
}
