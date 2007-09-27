// -*- C++ -*- generated by wxGlade 0.4cvs on Mon Oct  2 16:06:25 2006

#include "ImageTracker.h"

#include <string>

#include "vtkInteractorStyleSwitch.h"

#include "FilePattern.h"
#include "FileSet.h"
#include "ImageUtils.h"
#include "Logger.h"
#include "wxUtils.h"

static const std::string APP_NAME("ImageTracker");
static const std::string APP_VERSION("v 2.05");
static const std::string APP_AUTHOR("Brian Eastwood");
static const std::string APP_COPYRIGHT("(c) 2007");
static const std::string APP_WEBSITE("http://www.cs.unc.edu/research/Nano/ImageTracker");

bool ITApp::OnInit()
{
    ImageTracker *it = new ImageTracker((wxWindow *) NULL, -1, std2wx(APP_NAME + " " + APP_VERSION));
    it->Show(true);
    SetTopWindow(it);
    return true;
}

BEGIN_EVENT_TABLE(ImageTracker, wxFrame)
    EVT_IDLE(           ImageTracker::OnIdle)
    EVT_MENU(MENU_OPEN, ImageTracker::OnOpen)
    EVT_MENU(MENU_SAVE_IMAGES, ImageTracker::OnSaveViewImages)
    EVT_MENU(MENU_EXIT, ImageTracker::OnExit)
    EVT_MENU(MENU_ABOUT, ImageTracker::OnAbout)
    EVT_MENU(MENU_OCCLUSIONS, ImageTracker::OnOcclusions)
    EVT_MENU(MENU_STABILIZE, ImageTracker::OnStabilize)
    EVT_MENU(MENU_APPLY_TRANSFORM, ImageTracker::OnApplyTransform)
    EVT_MENU(MENU_CLG_OPTIC_FLOW, ImageTracker::OnCLGOpticFlow)
    EVT_MENU(MENU_HORN_OPTICAL_FLOW, ImageTracker::OnHornOpticalFlow)
    EVT_MENU(MENU_IMAGE_INFO, ImageTracker::OnImageInfo)
    EVT_MENU(MENU_LOG_ERROR, ImageTracker::OnLoggingMenu)
    EVT_MENU(MENU_LOG_WARN, ImageTracker::OnLoggingMenu)
    EVT_MENU(MENU_LOG_INFO, ImageTracker::OnLoggingMenu)
    EVT_MENU(MENU_LOG_DEBUG, ImageTracker::OnLoggingMenu)
    EVT_MENU(MENU_LOG_VERBOSE, ImageTracker::OnLoggingMenu)
    // begin wxGlade: ImageTracker::event_table
    EVT_LISTBOX_DCLICK(LBX_DATASOURCES, ImageTracker::OnEditDataSource)
    EVT_LISTBOX(LBX_DATASOURCES, ImageTracker::OnSelectDataSource)
    EVT_BUTTON(BTN_ADD_DATASOURCE, ImageTracker::OnAddDataSource)
    EVT_BUTTON(BTN_REMOVE_DATASOURCE, ImageTracker::OnRemoveDataSource)
    EVT_COMMAND_SCROLL(SLD_IMAGE_INDEX, ImageTracker::OnImageIndexScroll)
    EVT_BUTTON(BTN_FIRST, ImageTracker::OnFirstFrame)
    EVT_BUTTON(BTN_REWIND, ImageTracker::OnRewind)
    EVT_BUTTON(BTN_PAUSE, ImageTracker::OnPause)
    EVT_BUTTON(BTN_PLAY, ImageTracker::OnPlay)
    EVT_BUTTON(BTN_LAST, ImageTracker::OnLastFrame)
    // end wxGlade
END_EVENT_TABLE();

void ImageTracker::OnIdle(wxIdleEvent &event)
{
    this->UpdatePlayState();
    if (ImageTrackerController::Instance()->IsControllerChanged())
    {
        ImageTrackerController::Instance()->SetIsControllerChanged(false);
        ImageTrackerController::Instance()->UpdateView();
        this->UpdateDataSources();
    }
}

void ImageTracker::OnOpen(wxCommandEvent &event)
{
    // Pass this off to another event handler
    this->OnAddDataSource(event);
}

void ImageTracker::OnSaveViewImages(wxCommandEvent &event)
{
    Logger::info << "Please do not put anything on top of the image window while the visualizations are being saved!" << std::endl;
    this->SetPlayState(AboutToRecord);
}

void ImageTracker::OnExit(wxCommandEvent &event)
{
    // It seems the contents of the visualization panel sizer
    // may not get deleted properly, because the app started crashing
    // on exit on Windows when this was added.
    // wxSizer* sizer = this->panelVisualization->GetSizer();
    // sizer->Clear(true);
    this->Destroy();
}

void ImageTracker::OnAbout(wxCommandEvent &event)
{
    this->theStatusBar->SetStatusText(wxT("About..."));
    this->dlgAbout->Show(true);
}

void ImageTracker::OnImageInfo(wxCommandEvent &event)
{
    std::string function("ImageTracker::OnImageInfo");
    // Make sure we at least have a data source to work with
    if (this->lbxSources->GetCount() == 0)
    {
        Logger::warning << function << "There are no images open for which to provide information.  Please load some data." << std::endl;
        return;
    }
    
    int dsIdx = this->lbxSources->GetSelection();
    dsIdx = (dsIdx == wxNOT_FOUND) ? 0 : dsIdx;
    int imgIdx = this->sldImageIndex->GetValue();
    
    if (dynamic_cast<ImageTypeF2*>(ImageTrackerController::Instance()->GetDataSource(dsIdx)->GetImage(imgIdx)) != 0)
    {    
        PrintImageInfo(dynamic_cast<ImageTypeF2*>(ImageTrackerController::Instance()->GetDataSource(dsIdx)->GetImage(imgIdx)), "", Logger::info);
    }
    else if (dynamic_cast<ImageTypeV2F2*>(ImageTrackerController::Instance()->GetDataSource(dsIdx)->GetImage(imgIdx)) != 0)
    {
        PrintImageInfo(dynamic_cast<ImageTypeV2F2*>(ImageTrackerController::Instance()->GetDataSource(dsIdx)->GetImage(imgIdx)), "", Logger::info);
    }
    else
    {
        Logger::info << function << "Unknown image type." << std::endl;
    }
}

void ImageTracker::OnLoggingMenu(wxCommandEvent &event)
{
    switch (event.GetId())
    {
    case MENU_LOG_ERROR:
        Logger::setLevel(Error);
        break;
    case MENU_LOG_WARN:
        Logger::setLevel(Warning);
        break;
    case MENU_LOG_INFO:
        Logger::setLevel(Info);
        break;
    case MENU_LOG_DEBUG:
        Logger::setLevel(Debug);
        break;
    case MENU_LOG_VERBOSE:
        Logger::setLevel(Verbose);
        break;
    default:
        Logger::setLevel(Verbose);
    }
}

void ImageTracker::OnOcclusions(wxCommandEvent &event)
{
    // Make sure we at least have a data source to work with
    if (this->lbxSources->GetCount() == 0)
    {
        wxMessageDialog alert(this, wxT("Please add a data source on which to operate first."), 
                              wxT("No data sources."), wxOK);
        alert.ShowModal();
        return;
    }
    
    this->theStatusBar->SetStatusText(wxT("Removing occlusions..."));
    
    // Find the user's selected DataSource
    int idx = this->lbxSources->GetSelection();
    idx = (idx == wxNOT_FOUND) ? 0 : idx;
    this->dlgRemoveOcclusions->SetInput(ImageTrackerController::Instance()->GetDataSource(idx));
    this->dlgRemoveOcclusions->Show(true);
}

void ImageTracker::OnStabilize(wxCommandEvent &event)
{
    // Make sure we at least have a data source to work with
    if (this->lbxSources->GetCount() == 0)
    {
        wxMessageDialog alert(this, wxT("Please add a data source on which to operate first."), 
                              wxT("No data sources."), wxOK);
        alert.ShowModal();
        return;
    }
    
    this->theStatusBar->SetStatusText(wxT("Stabilizing..."));
    
    // Find the user's selected DataSource
    int idx = this->lbxSources->GetSelection();
    idx = (idx == wxNOT_FOUND) ? 0 : idx;
    if (ImageTrackerController::Instance()->GetDataSource(idx)->size() < 2) // Need at least two images to register.
    {
        wxMessageDialog alert(this, wxT("This operation requires at least two images in the data source."),
                              wxT("Not enough images."), wxOK);
        alert.ShowModal();
        return;
    }
    this->dlgRegistration->SetInput(ImageTrackerController::Instance()->GetDataSource(idx));
    this->dlgRegistration->Show(true);
}

void ImageTracker::OnApplyTransform(wxCommandEvent &event)
{
    // Make sure we at least have a data source to work with
    if (this->lbxSources->GetCount() == 0)
    {
        wxMessageDialog alert(this, wxT("Please add a data source on which to operate first."), 
                              wxT("No data sources."), wxOK);
        alert.ShowModal();
        return;
    }
    
    this->theStatusBar->SetStatusText(wxT("Applying transform..."));
    
    // Find the user's selected DataSource
    int idx = this->lbxSources->GetSelection();
    idx = (idx == wxNOT_FOUND) ? 0 : idx;
    this->dlgApplyTransform->SetInput(ImageTrackerController::Instance()->GetDataSource(idx));
    this->dlgApplyTransform->Show(true);
}

void ImageTracker::OnCLGOpticFlow(wxCommandEvent &event)
{
    // Make sure we at least have a data source to work with
    if (this->lbxSources->GetCount() == 0)
    {
        wxMessageDialog alert(this, wxT("Please add a data source on which to operate first."), 
                              wxT("No data sources."), wxOK);
        alert.ShowModal();
        return;
    }
    
    this->theStatusBar->SetStatusText(wxT("Computing CLG optical flow..."));
    
    // Find the user's selected DataSource.
    // Find the user's selected DataSource
    int idx = this->lbxSources->GetSelection();
    idx = (idx == wxNOT_FOUND) ? 0 : idx;
    if (ImageTrackerController::Instance()->GetDataSource(idx)->size() < 2) // Need at least two images to register.
    {
        wxMessageDialog alert(this, wxT("This operation requires at least two images in the data source."),
                              wxT("Not enough images."), wxOK);
        alert.ShowModal();
        return;
    }
    
    this->dlgCLGOpticFlow->SetInput(ImageTrackerController::Instance()->GetDataSource(idx));
    this->dlgCLGOpticFlow->Show(true);
}

void ImageTracker::OnHornOpticalFlow(wxCommandEvent &event)
{
    // Make sure we at least have a data source to work with
    if (this->lbxSources->GetCount() == 0)
    {
        wxMessageDialog alert(this, wxT("Please add a data source on which to operate first."), 
                              wxT("No data sources."), wxOK);
        alert.ShowModal();
        return;
    }
    
    this->theStatusBar->SetStatusText(wxT("Computing H&S optical flow..."));
    
    // Find the user's selected DataSource.
    // Find the user's selected DataSource
    int idx = this->lbxSources->GetSelection();
    idx = (idx == wxNOT_FOUND) ? 0 : idx;
    if (ImageTrackerController::Instance()->GetDataSource(idx)->size() < 2) // Need at least two images to register.
    {
        wxMessageDialog alert(this, wxT("This operation requires at least two images in the data source."),
                              wxT("Not enough images."), wxOK);
        alert.ShowModal();
        return;
    }
    
    this->dlgHornOpticalFlow->SetInput(ImageTrackerController::Instance()->GetDataSource(idx));
    this->dlgHornOpticalFlow->Show(true);
}

void ImageTracker::OnRemoveDataSource(wxCommandEvent &event)
{
    this->theStatusBar->SetStatusText(wxT("Removing data source..."),0);
    
    int idx = this->lbxSources->GetSelection();
    if (idx != wxNOT_FOUND)
    {
        ImageTrackerController::Instance()->RemoveDataSource(idx);
        this->theStatusBar->SetStatusText(wxT("Data source removed"),0);
    }
    else
    {
        this->theStatusBar->SetStatusText(wxT("No data source selected."),0);
    }
}

void ImageTracker::OnAddDataSource(wxCommandEvent &event)
{
    this->theStatusBar->SetStatusText(wxT("Adding data source..."),0);
    this->dlgDataSource->SetNewSource(true);
    this->dlgDataSource->ClearDataSource();
    
    if (this->dlgDataSource->ShowModal() == wxID_OK)
    {
        ImageTrackerController::Instance()->AddDataSource(this->dlgDataSource->GetDataSource());
        this->theStatusBar->SetStatusText(wxT("Added data source"));
    }
    else
    {
        this->theStatusBar->SetStatusText(wxT("No new data source"));
    }
}

void ImageTracker::OnSelectDataSource(wxCommandEvent &event)
{
    int idx = this->lbxSources->GetSelection();
    wxWindow* panel;
    
    if (idx == wxNOT_FOUND) // No data source is selected
    {
        panel = new wxPanel(this->panelVisualization, -1);
    }
    else
    {
        // Create an appropriate visualization control panel for the selected data source
        panel = ImageTrackerController::Instance()->GetVisualization(idx)->CreateWxControl(this->panelVisualization);
    }
    
    // Get the sizer for the visualization panel
    wxSizer* sizer = this->panelVisualization->GetSizer();
    
    // The visualization controls will always be placed in the first position of 
    // the visualization panel's sizer.
    // But Windows prefers Clear() over Remove(idx)...why, we don't know.
    sizer->Clear(true);
    // sizer->Remove(0);
    sizer->Add(panel, 1, wxADJUST_MINSIZE|wxEXPAND, 0);
    
    // Make sure control panel changes are seen
    this->Layout();
    
    // We want to skip the select event on the data source list box because a double click
    // event means we want to edit the data source.
    event.Skip();
}

void ImageTracker::OnEditDataSource(wxCommandEvent &event)
{
    this->theStatusBar->SetStatusText(wxT("Editing data source..."),0);
    
    int idx = this->lbxSources->GetSelection();
    if (idx != wxNOT_FOUND)
    {
        this->dlgDataSource->SetNewSource(false);
        this->dlgDataSource->SetDataSource(ImageTrackerController::Instance()->GetDataSource(idx));
        if (this->dlgDataSource->ShowModal() == wxID_OK)
        {
            ImageTrackerController::Instance()->SetIsControllerChanged(true);
            this->theStatusBar->SetStatusText(wxT("Edited data source"));
        }
        else
        {
            this->theStatusBar->SetStatusText(wxT("Edit cancelled"));
        }
    }
}

void ImageTracker::OnImageIndexScroll(wxScrollEvent &event)
{
    Logger::verbose << "ImageTracker::OnImageIndexScroll: " << this->sldImageIndex->GetValue() << std::endl;
    ImageTrackerController::Instance()->SetFrameIndex(this->sldImageIndex->GetValue());
}

void ImageTracker::OnFirstFrame(wxCommandEvent &event)
{
    Logger::verbose << "ImageTracker::OnFirstFrame" << std::endl;
    this->SetPlayState(SkipFirst);
}

void ImageTracker::OnRewind(wxCommandEvent &event)
{
    Logger::verbose << "ImageTracker::OnRewind" << std::endl;
    this->SetPlayState(Rewind);
}

void ImageTracker::OnPause(wxCommandEvent &event)
{
    Logger::verbose << "ImageTracker::OnPause" << std::endl;
    this->SetPlayState(Pause);
}

void ImageTracker::OnPlay(wxCommandEvent &event)
{
    Logger::verbose << "ImageTracker::OnPlay" << std::endl;
    this->SetPlayState(Play);
}

void ImageTracker::OnLastFrame(wxCommandEvent &event)
{
    Logger::verbose << "ImageTracker::OnLastFrame" << std::endl;
    this->SetPlayState(SkipLast);
}

// wxGlade: add ImageTracker event handlers

void ImageTracker::UpdatePlayState()
{
    // Get the current and maximum index
    int idx = this->sldImageIndex->GetValue();
    int maxIdx = this->sldImageIndex->GetMax();
    
    // Determine the next index and next play state
    // based on the current state; like a state machine.
    switch (this->GetPlayState())
    {
    case AboutToRecord:
        if (maxIdx > 0) // when no data is loaded, max idx is negative
        {
            this->sldImageIndex->SetValue(0);
            ImageTrackerController::Instance()->SetFrameIndex(0);
            this->SetPlayState(Record);
        }
        else
        {
            this->SetPlayState(Pause);
        }
        break;
    case Record:
        // Save the current view image
        char fileName[80];
        sprintf(fileName, saveFormat.c_str(), ImageTrackerController::Instance()->GetFrameIndex());
        ImageTrackerController::Instance()->SaveViewImage(std::string(fileName));
        if (idx < maxIdx)
        {
            this->sldImageIndex->SetValue(idx+1);
            ImageTrackerController::Instance()->SetFrameIndex(idx+1);
        }
        else // done recording
        {
            Logger::info << "Finished saving view images" << std::endl;
            this->SetPlayState(Pause);
        }
        break;
    case Play:      // advance one frame
        if (idx < maxIdx)
        {
            this->sldImageIndex->SetValue(idx+1);
            ImageTrackerController::Instance()->SetFrameIndex(idx+1);
        }
        else if (this->loopPlay)
        {
            this->sldImageIndex->SetValue(0);
            ImageTrackerController::Instance()->SetFrameIndex(0);
        }
        else
        {
            this->SetPlayState(Pause);
        }
        break;
    case Rewind:    // retreat one frame
        if (idx > 0)
        {
            this->sldImageIndex->SetValue(idx-1);
            ImageTrackerController::Instance()->SetFrameIndex(idx-1);
        }
        else if (this->loopPlay)
        {
            this->sldImageIndex->SetValue(maxIdx);
            ImageTrackerController::Instance()->SetFrameIndex(maxIdx);
        }
        else
        {
            this->SetPlayState(Pause);
        }
        break;
    case SkipFirst: // go to the first frame
        if (maxIdx > 0) // when no data is loaded, max idx is negative
        {
            this->sldImageIndex->SetValue(0);
            ImageTrackerController::Instance()->SetFrameIndex(0);
        }
        this->SetPlayState(Pause);
        break;
    case SkipLast:  // go to the last frame
        if (maxIdx > 0) // when no data is loaded, max idx is negative
        {
            this->sldImageIndex->SetValue(maxIdx);
            ImageTrackerController::Instance()->SetFrameIndex(maxIdx);
        }
        this->SetPlayState(Pause);
        break;
    case Pause:     // pause...does nothing--stable, default state
    default:
        break;
    }
}

void ImageTracker::UpdateDataSources()
{
    int idx = this->lbxSources->GetSelection();
    wxArrayString names;
    ImageTrackerController::Instance()->GetDataSourceNames(names);
    this->lbxSources->Set(names);
    this->lbxSources->SetSelection(std::min(idx, ((int)this->lbxSources->GetCount())-1));
    this->sldImageIndex->SetRange(0, ImageTrackerController::Instance()->GetMaxSize()-1);
    
    // Fake a data source selection event, so that the correct visualization control
    // is displayed in the visualization control panel.
    if (this->lbxSources->GetSelection() != idx)
    {
        wxCommandEvent evt(wxEVT_COMMAND_LISTBOX_SELECTED);
        this->OnSelectDataSource(evt);
    }
}

ImageTracker::ImageTracker(wxWindow* parent, int id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
    wxFrame(parent, id, title, pos, size, wxDEFAULT_FRAME_STYLE)
{
    // begin wxGlade: ImageTracker::ImageTracker
    hsplitDataLogger = new wxSplitterWindow(this, -1, wxDefaultPosition, wxDefaultSize, wxSP_3D|wxSP_BORDER);
    panelMainLower = new wxPanel(hsplitDataLogger, -1);
    panelMainUpper = new wxPanel(hsplitDataLogger, -1);
    vsplitDataView = new wxSplitterWindow(panelMainUpper, -1, wxDefaultPosition, wxDefaultSize, wxSP_3D|wxSP_BORDER);
    panelRight = new wxPanel(vsplitDataView, -1);
    panelLeft = new wxPanel(vsplitDataView, -1);
    hsplitSourceVisControl = new wxSplitterWindow(panelLeft, -1, wxDefaultPosition, wxDefaultSize, wxSP_3D|wxSP_BORDER);
    panelLeftLower = new wxPanel(hsplitSourceVisControl, -1);
    panelVisualization = new wxPanel(panelLeftLower, -1);
    panelLeftUpper = new wxPanel(hsplitSourceVisControl, -1);
    sizerStaticVisualization_staticbox = new wxStaticBox(panelLeftLower, -1, wxT("Visualization"));
    sizer_4_staticbox = new wxStaticBox(panelLeftUpper, -1, wxT("Data Sources"));
    itMenuBar = new wxMenuBar();
    SetMenuBar(itMenuBar);
    wxMenu* menuFile = new wxMenu();
    menuFile->Append(MENU_OPEN, wxT("&Open"), wxT("Open a set of image files"), wxITEM_NORMAL);
    menuFile->Append(MENU_SAVE_IMAGES, wxT("&Save View Images"), wxT("Save data visualizations as a set of images"), wxITEM_NORMAL);
    menuFile->Append(MENU_EXIT, wxT("E&xit"), wxT("Quit ImageTracker"), wxITEM_NORMAL);
    itMenuBar->Append(menuFile, wxT("&File"));
    wxMenu* menuEnhance = new wxMenu();
    menuEnhance->Append(MENU_OCCLUSIONS, wxT("&Remove Occlusions"), wxT("Detect and remove static partial occlusions from bright-field microscopy images"), wxITEM_NORMAL);
    menuEnhance->Append(MENU_STABILIZE, wxT("&Stabilize"), wxT("Remove global drift to keep an object stationary"), wxITEM_NORMAL);
    menuEnhance->Append(MENU_APPLY_TRANSFORM, wxT("Apply &Transform"), wxT("Apply a transforms from one stabilization to another data source"), wxITEM_NORMAL);
    itMenuBar->Append(menuEnhance, wxT("&Enhance"));
    wxMenu* menuCompute = new wxMenu();
    menuCompute->Append(MENU_CLG_OPTIC_FLOW, wxT("CLG &Optical Flow"), wxT("Combined local and global optical flow computation"), wxITEM_NORMAL);
    menuCompute->Append(MENU_HORN_OPTICAL_FLOW, wxT("Horn O&ptical Flow"), wxT("Horn and Schunck global optical flow computation"), wxITEM_NORMAL);
    itMenuBar->Append(menuCompute, wxT("&Compute"));
    wxMenu* menuHelp = new wxMenu();
    menuHelp->Append(MENU_IMAGE_INFO, wxT("&Image Info"), wxT("Print information about the current image"), wxITEM_NORMAL);
    wxMenu* menuLogging = new wxMenu();
    menuLogging->Append(MENU_LOG_ERROR, wxT("Error"), wxT("Display only error logging messages"), wxITEM_RADIO);
    menuLogging->Append(MENU_LOG_WARN, wxT("Warn"), wxT("Display warning and higher logging messages"), wxITEM_RADIO);
    menuLogging->Append(MENU_LOG_INFO, wxT("Info"), wxT("Display information and higher logging messages"), wxITEM_RADIO);
    menuLogging->Append(MENU_LOG_DEBUG, wxT("Debug"), wxT("Display debug and higher logging messages"), wxITEM_RADIO);
    menuLogging->Append(MENU_LOG_VERBOSE, wxT("Verbose"), wxT("Display verbose and higer logging messages (all messages)"), wxITEM_RADIO);
    menuHelp->Append(wxNewId(), wxT("Logging"), menuLogging, wxT(""));
    menuHelp->Append(MENU_ABOUT, wxT("&About"), wxT("About this application"), wxITEM_NORMAL);
    itMenuBar->Append(menuHelp, wxT("&Help"));
    theStatusBar = CreateStatusBar(1, 0);
    const wxString lbxSources_choices[] = {
        wxT("")
    };
    lbxSources = new wxListBox(panelLeftUpper, LBX_DATASOURCES, wxDefaultPosition, wxDefaultSize, 1, lbxSources_choices, wxLB_SINGLE);
    btnAddDataSource = new wxButton(panelLeftUpper, BTN_ADD_DATASOURCE, wxT("+"));
    btnRemoveDataSource = new wxButton(panelLeftUpper, BTN_REMOVE_DATASOURCE, wxT("-"));
    rwiView = new wxVTKRenderWindowInteractor(panelRight, -1);
    sldImageIndex = new wxSlider(panelRight, SLD_IMAGE_INDEX, 0, 0, 0, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL|wxSL_LABELS);
    btnFirst = new wxButton(panelRight, BTN_FIRST, wxT("|<"));
    btnRewind = new wxButton(panelRight, BTN_REWIND, wxT("<"));
    btnPause = new wxButton(panelRight, BTN_PAUSE, wxT("||"));
    btnPlay = new wxButton(panelRight, BTN_PLAY, wxT(">"));
    btnLast = new wxButton(panelRight, BTN_LAST, wxT(">|"));
    txtLogger = new wxTextCtrl(panelMainLower, -1, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);

    set_properties();
    do_layout();
    // end wxGlade
    
    // ----- Custom code! ----- //
    // Initialize play state
    this->SetPlayState(Pause);
    this->loopPlay = true;
    this->saveFormat = "visualization-%04d.tif";

    // Select log level, and redirect output to log panel
    menuHelp->Check(MENU_LOG_INFO, true);
    Logger::setLevel(Info);
    this->coutRedirect = new wxStreamToTextRedirector(this->txtLogger);
    
    // Pre-create dialogs
    this->dlgDataSource = new DataSourceDialog(this, -1, wxT("Edit DataSource"));
    this->dlgRemoveOcclusions = new RemoveOcclusionsDialog(this, -1, wxT("Remove Partial Occlusions"));
    this->dlgRegistration = new MultiResolutionRegistrationDialog(this, -1, wxT("Stabilize"));
    this->dlgApplyTransform = new ApplyTransformDialog(this, -1, wxT("Apply Transform"));
    this->dlgCLGOpticFlow = new CLGOpticFlowDialog(this, -1, wxT("CLG Optical Flow"));
    this->dlgHornOpticalFlow = new HornOpticalFlowDialog(this, -1, wxT("Horn and Schunck Optical Flow"));
    this->dlgAbout = new AboutDialog(this, -1, wxT("About"));
	this->dlgAbout->SetMessage(APP_NAME + " " + APP_VERSION + "\n" + APP_AUTHOR + " " + APP_COPYRIGHT); // + "\n" + APP_WEBSITE);
    
    // Set up the interactor style...we like the trackball camera
    vtkInteractorStyleSwitch* inter = vtkInteractorStyleSwitch::New();
    inter->SetCurrentStyleToTrackballCamera();
    this->rwiView->SetInteractorStyle(inter);
    
    // Set up the ImageTracker controller
    ImageTrackerController::Instance()->SetParent(this);
    ImageTrackerController::Instance()->SetRenderWindow(this->rwiView->GetRenderWindow());
    
    // This is pretty messed up right here.  Windows complains if the choices array for a
    // list box is empty.  If you pass an empty wxString as the only element, everything
    // is ok.  But, then you have a list item by default, which is bad.  So, we have to 
    // clear it here.  Wow.
    this->lbxSources->Clear();
}

void ImageTracker::set_properties()
{
    // begin wxGlade: ImageTracker::set_properties
    SetSize(wxSize(1000, 791));
    int theStatusBar_widths[] = { -1 };
    theStatusBar->SetStatusWidths(1, theStatusBar_widths);
    const wxString theStatusBar_fields[] = {
        wxT("Welcome!")
    };
    for(int i = 0; i < theStatusBar->GetFieldsCount(); ++i) {
        theStatusBar->SetStatusText(theStatusBar_fields[i], i);
    }
    lbxSources->SetSelection(0);
    btnFirst->SetMinSize(wxSize(40, 35));
    btnRewind->SetMinSize(wxSize(40, 35));
    btnPause->SetMinSize(wxSize(40, 35));
    btnPlay->SetMinSize(wxSize(40, 35));
    btnLast->SetMinSize(wxSize(40, 35));
    // end wxGlade
}


void ImageTracker::do_layout()
{
    // begin wxGlade: ImageTracker::do_layout
    wxBoxSizer* sizer_1 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_41 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_9 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_10 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_32 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_33 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_3 = new wxBoxSizer(wxVERTICAL);
    wxStaticBoxSizer* sizerStaticVisualization = new wxStaticBoxSizer(sizerStaticVisualization_staticbox, wxVERTICAL);
    wxBoxSizer* sizerVisualization = new wxBoxSizer(wxHORIZONTAL);
    wxStaticBoxSizer* sizer_4 = new wxStaticBoxSizer(sizer_4_staticbox, wxVERTICAL);
    wxBoxSizer* sizer_6 = new wxBoxSizer(wxHORIZONTAL);
    sizer_4->Add(lbxSources, 1, wxEXPAND|wxADJUST_MINSIZE, 0);
    sizer_6->Add(btnAddDataSource, 1, wxALIGN_CENTER_HORIZONTAL|wxADJUST_MINSIZE, 0);
    sizer_6->Add(btnRemoveDataSource, 1, wxALIGN_CENTER_HORIZONTAL, 0);
    sizer_4->Add(sizer_6, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL, 1);
    panelLeftUpper->SetAutoLayout(true);
    panelLeftUpper->SetSizer(sizer_4);
    sizer_4->Fit(panelLeftUpper);
    sizer_4->SetSizeHints(panelLeftUpper);
    sizerVisualization->Add(20, 20, 0, wxADJUST_MINSIZE, 0);
    panelVisualization->SetAutoLayout(true);
    panelVisualization->SetSizer(sizerVisualization);
    sizerVisualization->Fit(panelVisualization);
    sizerVisualization->SetSizeHints(panelVisualization);
    sizerStaticVisualization->Add(panelVisualization, 1, wxEXPAND, 0);
    panelLeftLower->SetAutoLayout(true);
    panelLeftLower->SetSizer(sizerStaticVisualization);
    sizerStaticVisualization->Fit(panelLeftLower);
    sizerStaticVisualization->SetSizeHints(panelLeftLower);
    hsplitSourceVisControl->SplitHorizontally(panelLeftUpper, panelLeftLower);
    sizer_3->Add(hsplitSourceVisControl, 1, wxEXPAND, 0);
    panelLeft->SetAutoLayout(true);
    panelLeft->SetSizer(sizer_3);
    sizer_3->Fit(panelLeft);
    sizer_3->SetSizeHints(panelLeft);
    sizer_10->Add(rwiView, 8, wxALL|wxEXPAND, 0);
    sizer_32->Add(sldImageIndex, 3, wxEXPAND|wxADJUST_MINSIZE, 0);
    sizer_33->Add(btnFirst, 0, wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 0);
    sizer_33->Add(btnRewind, 0, wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 0);
    sizer_33->Add(btnPause, 0, wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 0);
    sizer_33->Add(btnPlay, 0, wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 0);
    sizer_33->Add(btnLast, 0, wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 0);
    sizer_32->Add(sizer_33, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    sizer_10->Add(sizer_32, 1, wxEXPAND, 0);
    panelRight->SetAutoLayout(true);
    panelRight->SetSizer(sizer_10);
    sizer_10->Fit(panelRight);
    sizer_10->SetSizeHints(panelRight);
    vsplitDataView->SplitVertically(panelLeft, panelRight, 10);
    sizer_9->Add(vsplitDataView, 2, wxEXPAND, 0);
    panelMainUpper->SetAutoLayout(true);
    panelMainUpper->SetSizer(sizer_9);
    sizer_9->Fit(panelMainUpper);
    sizer_9->SetSizeHints(panelMainUpper);
    sizer_41->Add(txtLogger, 1, wxEXPAND|wxADJUST_MINSIZE, 0);
    panelMainLower->SetAutoLayout(true);
    panelMainLower->SetSizer(sizer_41);
    sizer_41->Fit(panelMainLower);
    sizer_41->SetSizeHints(panelMainLower);
    hsplitDataLogger->SplitHorizontally(panelMainUpper, panelMainLower, 600);
    sizer_1->Add(hsplitDataLogger, 2, wxEXPAND, 0);
    SetAutoLayout(true);
    SetSizer(sizer_1);
    Layout();
    // end wxGlade
}

