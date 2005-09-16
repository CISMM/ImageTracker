#pragma once
#include <wx/wxprec.h>
#include "RegistrationDesign_wdr.h"
#include "CLGOpticFlowDialog.h"
#include "DialogLogger.h"
#include "FileSetDialog.h"
#include "GlobalRegistrationDialog.h"
#include "HarrisFeatureDialog.h"
#include "../vtkView/VtkCanvas.h"


static const wxChar *TITLE = _T("ImageTracker");
static const wxChar *VERSION = _T("v 2.01");
static const wxChar *AUTHOR = _T("Brian Eastwood");
static const wxChar *INFO = _T("(c) 2005");

static const wxChar *FILETYPES = _T("PNG Files|*.png");
static const wxChar *ITK_FILETYPES = _T("All Files|*.*|"
                                        "TIFF Files|*.tif;*.tiff|"
                                        "PNG Files|*.png|"
                                        "JPG Files|*.jpg");


class TrackerApp : public wxApp
{
public:
    virtual bool OnInit();
};

// WDR: class declarations

//----------------------------------------------------------------------------
// ImageViewer
//----------------------------------------------------------------------------

/*
 * Image processing frame.
 */
class ImageViewer: public wxFrame
{
public:
    // constructors and destructors
    ImageViewer( wxWindow *parent, wxWindowID id = -1, 
        const wxString &title = TITLE,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_FRAME_STYLE );
    virtual ~ImageViewer();
    
    // WDR: method declarations for ImageViewer
    FileSetDialog* GetFileSetPanel()  { return (FileSetDialog*) FindWindow( IT_IMAGE_FILES ); }
    LoggerDialog* GetLoggerPanel()  { return (LoggerDialog*) FindWindow( IT_LOGGER ); }
    VtkCanvas* GetCanvas()  { return (VtkCanvas*) FindWindow( IT_CANVAS ); }
    
private:
    // WDR: member variable declarations for ImageViewer
    GlobalRegistrationDialog* registrationDialog;
    HarrisFeatureDialog* featureDialog;
    CLGOpticFlowDialog* clgFlowDialog;
    
private:
    // WDR: handler declarations for ImageViewer
    void OnCLGOpticFlow( wxCommandEvent &event );
    void OnTrack( wxCommandEvent &event );
    void OnImageScale( wxCommandEvent &event );
    void OnImageTrueSize( wxCommandEvent &event );
    void OnAbout( wxCommandEvent &event );
    void OnRegister( wxCommandEvent &event );
    void OnExit( wxCommandEvent &event );
    void OnOpenFiles( wxCommandEvent &event );

private:
    DECLARE_EVENT_TABLE()
};
