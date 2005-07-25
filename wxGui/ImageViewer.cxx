#include "./ImageViewer.h"
#include "./DialogLogger.h"

bool ViewerApp::OnInit()
{
    //ImageTrackerDialog *dlg = new ImageTrackerDialog((wxWindow* ) NULL, -1, _("Harris"));
    //dlg->Show(true);
    //SetTopWindow(dlg);
    //return true;

    ImageViewer *frame = new ImageViewer((wxWindow *) NULL);
    frame->Show(TRUE);
    SetTopWindow(frame);
    Logger::SetLogger(new DialogLogger());
    return TRUE;
}

// WDR: class implementations

//----------------------------------------------------------------------------
// ImageViewer
//----------------------------------------------------------------------------

// WDR: event table for ImageViewer

BEGIN_EVENT_TABLE(ImageViewer,wxFrame)
    EVT_MENU( IV_FILES, ImageViewer::OnOpenFiles )
    EVT_MENU( IV_EXIT, ImageViewer::OnExit )
    EVT_MENU( IV_REGISTER, ImageViewer::OnRegister )
    EVT_MENU( IV_ABOUT, ImageViewer::OnAbout )
    EVT_MENU( IV_MENU_LOGGER, ImageViewer::OnViewLogger )
    EVT_MENU( IV_TRACK, ImageViewer::OnTrack )
    EVT_MENU( IV_CLGOpticFlow, ImageViewer::OnCLGOpticFlow )
END_EVENT_TABLE()

ImageViewer::ImageViewer( wxWindow *parent, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style ) :
    wxFrame( parent, id, title, position, size, style )
{
    // WDR: dialog function CreateViewerDialog for ImageViewer
    CreateViewerDialog( this, TRUE );
    this->SetMenuBar(CreateViewerMenu());
    this->CreateStatusBar(2);

    this->fileSetDialog = (FileSetDialog *) NULL;
    this->registrationDialog = (GlobalRegistrationDialog *) NULL;
    this->featureDialog = (HarrisFeatureDialog *) NULL;
    this->clgFlowDialog = (CLGOpticFlowDialog *) NULL;
}

ImageViewer::~ImageViewer()
{
    Logger::Destroy();
}

// WDR: handler implementations for ImageViewer

void ImageViewer::OnCLGOpticFlow( wxCommandEvent &event )
{
    // Ensure we have image files to use
    if (!this->fileSetDialog || this->fileSetDialog->GetFileSet()->GetFileNames()->size() == 0)
    {
        Logger::logWarn("Please open image files first.");
        this->OnOpenFiles(event);
        return;
    }

    // Create the dialog, if it doesn't exist
    if (!this->clgFlowDialog)
    {
        this->clgFlowDialog = new CLGOpticFlowDialog(this);
    }

    Logger::logVerbose("Showing CLG optic flow dialog.");
    this->clgFlowDialog->SetInput(this->fileSetDialog->GetFileSet());
    this->clgFlowDialog->Show(true);    
}

void ImageViewer::OnViewLogger( wxCommandEvent &event )
{
    Logger::setVisible(true);
    Logger::logVerbose("Setting logger visible.");
}

void ImageViewer::OnAbout( wxCommandEvent &event )
{
    wxString ret(_T("\n"));
    wxString msg(TITLE);
    msg.append(_T(" ")).append(VERSION).append(ret).append(AUTHOR).append(ret).append(INFO);
    wxMessageDialog about(this, msg, wxString(_T("About ")).append(TITLE), wxOK);
    about.ShowModal();
}

void ImageViewer::OnRegister( wxCommandEvent &event )
{
    if (!this->fileSetDialog || this->fileSetDialog->GetFileSet()->GetFileNames()->size() == 0)
    {
        Logger::logWarn("Please open image files first.");
        this->OnOpenFiles(event);
        return;
    }

    if (!this->registrationDialog)
    {
        this->registrationDialog = new GlobalRegistrationDialog(this);
        this->registrationDialog->SetCanvas(this->GetCanvas());
    }

    Logger::logVerbose("Showing registration dialog.");
    this->registrationDialog->GetPipeline()->SetInput(this->fileSetDialog->GetFileSet());
    this->registrationDialog->Show(true);
}

void ImageViewer::OnTrack( wxCommandEvent &event )
{
    if (!this->fileSetDialog || this->fileSetDialog->GetFileSet()->GetFileNames()->size() == 0)
    {
        Logger::logWarn("Please open image files first.");
        this->OnOpenFiles(event);
        return;
    }

    if (!this->featureDialog)
    {
        this->featureDialog = new HarrisFeatureDialog(this);
        this->featureDialog->SetCanvas(this->GetCanvas());
    }

    Logger::logVerbose("Showing tracker dialog.");
    this->featureDialog->SetInput(this->fileSetDialog->GetFileSet());
    this->featureDialog->Show(true);
}

void ImageViewer::OnExit( wxCommandEvent &event )
{
    this->Close(true);
}

void ImageViewer::OnOpenFiles( wxCommandEvent &event )
{
    if (!this->fileSetDialog)
    {
        this->fileSetDialog = new FileSetDialog(this);
        this->fileSetDialog->SetCanvas(this->GetCanvas());
    }

    this->fileSetDialog->Show(true);
}
