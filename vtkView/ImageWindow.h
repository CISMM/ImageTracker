/////////////////////////////////////////////////////////////////////////////
// Name:        ImageWindow.h
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include <wx/wxprec.h>
#include <vector>

#include "VtkCanvas.h"
#include "../CommonTypes.h"
#include "vtkViewer_wdr.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// ImageWindow
//----------------------------------------------------------------------------

class ImageWindow : public wxDialog
{
public:
    // constructors and destructors
    ImageWindow( wxWindow *parent, wxWindowID id=-1, const wxString &title=_T("Image"),
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_FRAME_STYLE );
    virtual ~ImageWindow();
    
    // WDR: method declarations for ImageWindow
    VtkCanvas* GetCanvas()  { return (VtkCanvas*) FindWindow( ID_CANVAS ); }

    static void ImShow(CommonTypes::InternalImageType::Pointer image);
    static void ImShow(CommonTypes::InternalImageType::Pointer image, std::string title);
    static void Destroy();
    
private:
    // WDR: member variable declarations for ImageWindow
    
private:
    // WDR: handler declarations for ImageWindow

private:
    static int s_windowCount;
    static std::vector<ImageWindow*>* s_windowList;
    DECLARE_EVENT_TABLE()
};

