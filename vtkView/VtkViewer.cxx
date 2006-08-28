/////////////////////////////////////////////////////////////////////////////
// Name:        vtkViewer.cxx
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#include "VtkViewer.h"
#include "wxUtils.h"

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
        // You've got to be kidding me. Isn't there an easier way to get a 
        // std::string from a wxString?  Please.
        this->GetCanvas()->SetFileName(wx2std(open.GetPath()));
    }
}
