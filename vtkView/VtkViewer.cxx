/////////////////////////////////////////////////////////////////////////////
// Name:        vtkViewer.cxx
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "vtkViewer.cxx"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "vtkViewer.h"

bool ViewerApp::OnInit()
{
    vtkViewer *frame = new vtkViewer((wxWindow *) NULL);
    frame->Show(TRUE);
    SetTopWindow(frame);
    return TRUE;
}

// WDR: class implementations

//----------------------------------------------------------------------------
// vtkViewer
//----------------------------------------------------------------------------

// WDR: event table for vtkViewer

BEGIN_EVENT_TABLE(vtkViewer,wxFrame)
    EVT_MENU( ID_FILE_OPEN, vtkViewer::OnFileOpen )
    EVT_MENU( ID_FILE_EXIT, vtkViewer::OnExit )
END_EVENT_TABLE()

vtkViewer::vtkViewer( wxWindow *parent, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style ) :
    wxFrame( parent, id, title, position, size, style )
{
    // WDR: dialog function VtkViewerDialog for vtkViewer
    CreateVtkViewer( this, TRUE );
    this->SetMenuBar(VtkViewerMenu());
}

vtkViewer::~vtkViewer()
{
}

// WDR: handler implementations for vtkViewer

void vtkViewer::OnExit( wxCommandEvent &event )
{
    this->Close(true);
}

void vtkViewer::OnFileOpen( wxCommandEvent &event )
{
    wxFileDialog open(this, _T("Open a file"));
    if (open.ShowModal() == wxID_OK)
    {
        this->GetCanvas()->SetFileName(open.GetPath().c_str());
    }
}
