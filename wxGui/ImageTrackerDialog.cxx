/////////////////////////////////////////////////////////////////////////////
// Name:        ImageTrackerDialog.cxx
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#include "ImageTrackerDialog.h"
#include "FileSetDialog.h"
#include "ImageViewer.h"
#include "DialogLogger.h"
#include "GlobalRegistrationDialog.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// ImageTrackerDialog
//----------------------------------------------------------------------------

// WDR: event table for ImageTrackerDialog

BEGIN_EVENT_TABLE(ImageTrackerDialog,wxDialog)
END_EVENT_TABLE()

ImageTrackerDialog::ImageTrackerDialog( wxWindow *parent, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style ) :
    wxDialog( parent, id, title, position, size, style )
{
    // WDR: dialog function CreateImageTrackerApp for ImageTrackerDialog
    CreateImageTrackerApp( this, TRUE );

    new FileSetDialog(this->GetPanelFiles());
    new ImageViewer(this->GetPanelImage());
    new LoggerDialog(this->GetPanelLogger());
    new GlobalRegistrationDialog(this->GetPanelControl());
}

// WDR: handler implementations for ImageTrackerDialog




