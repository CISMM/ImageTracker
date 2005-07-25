/////////////////////////////////////////////////////////////////////////////
// Name:        ImageTrackerDialog.h
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include <wx/wxprec.h>
#include "RegistrationDesign_wdr.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// ImageTrackerDialog
//----------------------------------------------------------------------------

class ImageTrackerDialog: public wxDialog
{
public:
    // constructors and destructors
    ImageTrackerDialog( wxWindow *parent, wxWindowID id, const wxString &title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE );
    
    // WDR: method declarations for ImageTrackerDialog
    wxPanel* GetPanelLogger()  { return (wxPanel*) FindWindow( ID_PANEL_LOGGER ); }
    wxPanel* GetPanelControl()  { return (wxPanel*) FindWindow( ID_PANEL_CONTROL ); }
    wxPanel* GetPanelImage()  { return (wxPanel*) FindWindow( ID_PANEL_IMAGE ); }
    wxPanel* GetPanelFiles()  { return (wxPanel*) FindWindow( ID_PANEL_FILES ); }
    
private:
    // WDR: member variable declarations for ImageTrackerDialog
    
private:
    // WDR: handler declarations for ImageTrackerDialog

private:
    DECLARE_EVENT_TABLE()
};
