/////////////////////////////////////////////////////////////////////////////
// Name:        LoggerPanel.h
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#ifndef __LoggerPanel_H__
#define __LoggerPanel_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "LoggerPanel.cxx"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wxCore_wdr.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// LoggerPanel
// Note: this class depends upon wxStreamToTextRedirector.  According to the
// wxWidgets documentation for that class:
// You also must have wxUSE_STD_IOSTREAM option on (i.e. set to 1) in your 
// wxWindows setup.h to be able to use it.  (Requires recompile of wx.)
//----------------------------------------------------------------------------
/**
 * A wxPanel containing a single text box to which all std::cout output is
 * redirected.  Any application that wants to view standard output messages
 * and uses wx can just create an instance of this panel somewhere in the 
 * application GUI.
 */
class LoggerPanel: public wxPanel
{
public:
    // constructors and destructors
    LoggerPanel( wxWindow *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER );
    virtual ~LoggerPanel();
    
    // WDR: method declarations for LoggerPanel
    wxTextCtrl* GetTextBox()  { return (wxTextCtrl*) FindWindow( ID_TEXT_BOX ); }
    
private:
    // WDR: member variable declarations for LoggerPanel

    wxStreamToTextRedirector* director;
    
private:
    // WDR: handler declarations for LoggerPanel

private:
    DECLARE_EVENT_TABLE()
};


#endif
