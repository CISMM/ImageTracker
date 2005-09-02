/////////////////////////////////////////////////////////////////////////////
// Name:        vtkViewer.h
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#ifndef __vtkViewer_H__
#define __vtkViewer_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "vtkViewer.cxx"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "vtkViewer_wdr.h"
#include "VtkCanvas.h"
#include "wxVTKRenderWindowInteractor.h"

class ViewerApp : public wxApp
{
public:
    virtual bool OnInit();
};

// WDR: class declarations

//----------------------------------------------------------------------------
// vtkViewer
//----------------------------------------------------------------------------

class vtkViewer: public wxFrame
{
public:
    // constructors and destructors
    vtkViewer( wxWindow *parent, wxWindowID id = -1, const wxString &title = _T("Viewer"),
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_FRAME_STYLE );
    virtual ~vtkViewer();
    
    // WDR: method declarations for vtkViewer
    VtkCanvas* GetCanvas()  { return (VtkCanvas*) FindWindow( ID_CANVAS ); }
    
private:
    // WDR: member variable declarations for vtkViewer
    
private:
    // WDR: handler declarations for vtkViewer
    void OnExit( wxCommandEvent &event );
    void OnFileOpen( wxCommandEvent &event );

private:
    DECLARE_EVENT_TABLE()
};

#endif
