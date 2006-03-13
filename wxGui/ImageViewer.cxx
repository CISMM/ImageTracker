#include "./ImageViewer.h"

bool TrackerApp::OnInit()
{
    //ImageTrackerDialog *dlg = new ImageTrackerDialog((wxWindow* ) NULL, -1, _("Harris"));
    //dlg->Show(true);
    //SetTopWindow(dlg);
    //return true;

    ImageViewer *frame = new ImageViewer((wxWindow *) NULL);
    frame->Show(TRUE);
    SetTopWindow(frame);
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
    EVT_MENU( IV_TRACK, ImageViewer::OnTrack )
    EVT_MENU( IV_CLGOpticFlow, ImageViewer::OnCLGOpticFlow )
    EVT_MENU( IV_MULTI_RES, ImageViewer::OnRegisterMultiResolution )
    EVT_MENU( IV_LOG_ERROR, ImageViewer::OnLogging )
	EVT_MENU( IV_LOG_WARN, ImageViewer::OnLogging )
	EVT_MENU( IV_LOG_INFO, ImageViewer::OnLogging )
	EVT_MENU( IV_LOG_DEBUG, ImageViewer::OnLogging )
	EVT_MENU( IV_LOG_VERBOSE, ImageViewer::OnLogging )
END_EVENT_TABLE()

ImageViewer::ImageViewer( wxWindow *parent, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style ) :
    wxFrame( parent, id, title, position, size, style )
{
    // WDR: dialog function CreateViewerDialog for ImageViewer
    CreateImageTrackerApp( this, TRUE );
    this->SetMenuBar(CreateViewerMenu());
    this->CreateStatusBar(2);

    // Lazily instantiated
    this->registrationDialog = 0;
    this->featureDialog = 0;
    this->clgFlowDialog = 0;
    this->mrRegistrationDialog = 0;

    this->GetFileSetPanel()->SetCanvas(this->GetCanvas());

	// Choose an initial log level
	Logger::setLevel(Info);
	wxMenuItem* item = this->GetMenuBar()->FindItem(IV_LOG_INFO);
	if (item)
		item->Check(true);
}

ImageViewer::~ImageViewer()
{
}

// WDR: handler implementations for ImageViewer

void ImageViewer::OnLogging( wxCommandEvent &event )
{
	// This is pretty verbose, but seems to be the only way we
	// can get at menu radio items.
	wxMenuItem* item;
	item = this->GetMenuBar()->FindItem(IV_LOG_ERROR);
	if (item && item->IsChecked())
		Logger::setLevel(Error);
	item = this->GetMenuBar()->FindItem(IV_LOG_WARN);
	if (item && item->IsChecked())
		Logger::setLevel(Warning);
	item = this->GetMenuBar()->FindItem(IV_LOG_INFO);
	if (item && item->IsChecked())
		Logger::setLevel(Info);
	item = this->GetMenuBar()->FindItem(IV_LOG_DEBUG);
	if (item && item->IsChecked())
		Logger::setLevel(Debug);
	item = this->GetMenuBar()->FindItem(IV_LOG_VERBOSE);
	if (item && item->IsChecked())
		Logger::setLevel(Verbose);

	Logger::debug << "Log level => " << Logger::getLevel() << std::endl;
}

void ImageViewer::OnRegisterMultiResolution( wxCommandEvent &event )
{
    if (this->GetFileSetPanel()->GetFileSet()->GetFileNames()->size() == 0)
    {
        Logger::logWarn("Please open image files first.");
        this->OnOpenFiles(event);
        return;
    }

    if (!this->mrRegistrationDialog)
    {
        this->mrRegistrationDialog = new MultiResolutionRegistrationDialog(this);
        this->mrRegistrationDialog->SetCanvas(this->GetCanvas());
    }

    Logger::logVerbose("Showing multi-resolution registration dialog.");
    this->mrRegistrationDialog->SetInputFiles(*(this->GetFileSetPanel()->GetFileSet()));
    this->mrRegistrationDialog->Show(true);
}

void ImageViewer::OnCLGOpticFlow( wxCommandEvent &event )
{
    // Ensure we have image files to use
    if (this->GetFileSetPanel()->GetFileSet()->GetFileNames()->size() == 0)
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
    this->clgFlowDialog->SetInput(this->GetFileSetPanel()->GetFileSet());
    this->clgFlowDialog->Show(true);    
}

void ImageViewer::OnAbout( wxCommandEvent &event )
{
    wxString ret(_T("\n"));
    wxString msg(TITLE);
    msg.append(_T(" ")).append(VERSION).append(ret).append(AUTHOR).append(ret).append(INFO).append(ret).append(ret);
    msg.append(_T("A user guide is provided with the installation.  See:")).append(ret);
    msg.append(_T("Start -> Program Files -> NSRG -> ")).append(ret);
    msg.append(_T("ImageTracker ")).append(VERSION).append(_T(" -> ImageTracker Documentation"));
    wxMessageDialog about(this, msg, wxString(_T("About ")).append(TITLE), wxOK);
    about.ShowModal();
}

void ImageViewer::OnRegister( wxCommandEvent &event )
{
    if (this->GetFileSetPanel()->GetFileSet()->GetFileNames()->size() == 0)
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
    this->registrationDialog->GetPipeline()->SetInput(this->GetFileSetPanel()->GetFileSet());
    this->registrationDialog->InitializeOutput();
    this->registrationDialog->Show(true);
}

void ImageViewer::OnTrack( wxCommandEvent &event )
{
    if (this->GetFileSetPanel()->GetFileSet()->GetFileNames()->size() == 0)
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
    this->featureDialog->SetInput(this->GetFileSetPanel()->GetFileSet());
    this->featureDialog->Show(true);
}

void ImageViewer::OnExit( wxCommandEvent &event )
{
    this->Close(true);
}

void ImageViewer::OnOpenFiles( wxCommandEvent &event )
{
    this->GetFileSetPanel()->OnAdd(event);
}
