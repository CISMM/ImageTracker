/////////////////////////////////////////////////////////////////////////////
// Name:        wxTestDesigns.h
// Author:      XX
// Created:     XX/XX/XX
// Copyright:
/////////////////////////////////////////////////////////////////////////////

#include <wx/wxprec.h>
#include "wxTestDesigns_wdr.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// TestCaseFrame
//----------------------------------------------------------------------------

class TestCaseFrame: public wxFrame
{
public:
    // constructors and destructors
    TestCaseFrame( wxWindow *parent, wxWindowID id, const wxString &title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_FRAME_STYLE );

private:
    // WDR: method declarations for TestCaseFrame
    void CreateMyMenuBar();

private:
    // WDR: member variable declarations for TestCaseFrame

private:
    // WDR: handler declarations for TestCaseFrame
    void OnRunTests( wxCommandEvent &event );
    void OnQuit( wxCommandEvent &event );
    void OnCloseWindow( wxCloseEvent &event );

private:
    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
// TestCaseApp
//----------------------------------------------------------------------------

class TestCaseApp: public wxApp
{
public:
    TestCaseApp();

    virtual bool OnInit();
    virtual int OnExit();
};

