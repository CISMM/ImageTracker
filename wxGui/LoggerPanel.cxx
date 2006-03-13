/////////////////////////////////////////////////////////////////////////////
// Name:        LoggerPanel.cxx
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "LoggerPanel.cxx"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "LoggerPanel.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// LoggerPanel
//----------------------------------------------------------------------------

// WDR: event table for LoggerPanel

BEGIN_EVENT_TABLE(LoggerPanel,wxPanel)
END_EVENT_TABLE()

LoggerPanel::LoggerPanel( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxPanel( parent, id, position, size, style )
{
    this->director = NULL;
    // WDR: dialog function CreateLogPanel for LoggerPanel
    CreateLogPanel( this, true );
    this->director = new wxStreamToTextRedirector(this->GetTextBox());
}

LoggerPanel::~LoggerPanel()
{
    if (this->director)
        delete this->director;
}

// WDR: handler implementations for LoggerPanel




