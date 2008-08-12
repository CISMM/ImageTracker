// -*- C++ -*- generated by wxGlade 0.6.3 on Fri Jul 25 12:17:19 2008

#include "ImageTracker.h"

// begin wxGlade: ::extracode

// end wxGlade


#include <algorithm>
#include <iostream>
#include <string>

#include <wx/apptrait.h>
#include <wx/stdpaths.h>

#include "vtkInteractorStyleSwitch.h"

#include "FileUtils.h"
#include "GaussianFilterPanel.h"
#include "ImageTrackerController.h"
#include "ImageUtils.h"
#include "ItkVtkPipeline.h"
#include "Logger.h"
#include "ScalarImageVisualization.h"
#include "ThresholdPanel.h"
#include "VectorGlyphVisualization.h"
#include "VectorHeightMapVisualization.h"
#include "wxUtils.h"

static const std::string APP_NAME("ImageTracker");
static const std::string APP_VERSION("v 2.09");
static const std::string APP_AUTHOR("Brian Eastwood");
static const std::string APP_COPYRIGHT("(c) 2004 - 2008");
static const std::string APP_WEBSITE("http://www.cs.unc.edu/Research/nano/cismm/download/imagetracker");

static const std::string VECTOR_FILE_FILTER(
        "All Files|*.*|MHA|*.mha|VTK|*.vtk|NRRD|*.nrrd");

bool ITApp::OnInit()
{
    ImageTracker *it = new ImageTracker((wxWindow *) NULL, -1, nano::std2wx(APP_NAME + " " + APP_VERSION));
    it->Show(true);
    SetTopWindow(it);
    return true;
}

ImageTracker::ImageTracker(wxWindow* parent, int id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
    wxFrame(parent, id, title, pos, size, wxDEFAULT_FRAME_STYLE)
{
    // begin wxGlade: ImageTracker::ImageTracker
    panelMainUpper = new wxPanel(this, wxID_ANY);
    vsplitDataView = new wxSplitterWindow(panelMainUpper, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D|wxSP_BORDER|wxSP_PERMIT_UNSPLIT);
    panelRight = new wxPanel(vsplitDataView, wxID_ANY);
    panelLeft = new wxPanel(vsplitDataView, wxID_ANY);
    notebookData = new wxNotebook(panelLeft, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
    notebook_1_pane_2 = new wxPanel(notebookData, wxID_ANY);
    panelVectorControl = new wxPanel(notebook_1_pane_2, wxID_ANY);
    notebook_1_pane_1 = new wxPanel(notebookData, wxID_ANY);
    sizer_41_staticbox = new wxStaticBox(notebook_1_pane_2, -1, wxT("Visualization"));
    panelFilterControl = new wxPanel(notebook_1_pane_1, wxID_ANY);
    itMenuBar = new wxMenuBar();
    wxMenu* menuFile = new wxMenu();
    menuFile->Append(MENU_SAVE_IMAGES, wxT("&Save View Images"), wxT("Save data visualizations as a set of images"), wxITEM_NORMAL);
    menuFile->Append(MENU_EXIT, wxT("E&xit"), wxT("Quit ImageTracker"), wxITEM_NORMAL);
    itMenuBar->Append(menuFile, wxT("&File"));
    wxMenu* menuFilter = new wxMenu();
    menuFilter->Append(MENU_THRESHOLD, wxT("&Threshold"), wxT("Apply a threshold"), wxITEM_NORMAL);
    menuFilter->Append(MENU_GAUSSIAN, wxT("&Gaussian"), wxT("Apply 2D Gaussian smoothing"), wxITEM_NORMAL);
    itMenuBar->Append(menuFilter, wxT("Fil&ter"));
    wxMenu* menuProcess = new wxMenu();
    menuProcess->Append(MENU_OCCLUSIONS, wxT("&Remove Occlusions"), wxT("Detect and remove static partial occlusions from bright-field microscopy images"), wxITEM_NORMAL);
    menuProcess->Append(MENU_STABILIZE, wxT("&Stabilize"), wxT("Remove global drift to keep an object stationary"), wxITEM_NORMAL);
    menuProcess->Append(MENU_APPLY_TRANSFORM, wxT("Apply &Transform"), wxT("Apply a transforms from one stabilization to another data source"), wxITEM_NORMAL);
    menuProcess->Append(MENU_CLG_OPTICAL_FLOW, wxT("CLG &Optical Flow"), wxT("Combined local and global optical flow computation"), wxITEM_NORMAL);
    menuProcess->Append(MENU_INTEGRATE_FLOW, wxT("&Integrate Flow"), wxT("Integrate flow fields into displacement fields"), wxITEM_NORMAL);
    menuProcess->Append(MENU_HORN_OPTICAL_FLOW, wxT("&Horn Optical Flow"), wxT("Horn and Schunck global optical flow computation"), wxITEM_NORMAL);
    itMenuBar->Append(menuProcess, wxT("&Process"));
    wxMenu* menuView = new wxMenu();
    menuView->Append(MENU_WINDOW_CONTRAST, wxT("&Window/Contrast"), wxT("Adjust display contrast levels"), wxITEM_NORMAL);
    menuView->Append(MENU_LOGGER, wxT("&Logger"), wxT("Show log file"), wxITEM_NORMAL);
    menuView->Append(MENU_IMAGE_INFO, wxT("&Image Info"), wxT("Print information about the current image"), wxITEM_NORMAL);
    menuView->Append(MENU_LOOP_PLAY, wxT("Loop &play"), wxT("Specify whether image set playback should loop at the ends"), wxITEM_CHECK);
    itMenuBar->Append(menuView, wxT("&View"));
    wxMenu* menuHelp = new wxMenu();
    menuHelp->Append(MENU_ABOUT, wxT("&About"), wxT("About this application"), wxITEM_NORMAL);
    itMenuBar->Append(menuHelp, wxT("&Help"));
    SetMenuBar(itMenuBar);
    theStatusBar = CreateStatusBar(1, 0);
    const wxString *listFilters_choices = NULL;
    listFilters = new wxListBox(notebook_1_pane_1, LIST_FILTERS, wxDefaultPosition, wxDefaultSize, 0, listFilters_choices, wxLB_SINGLE);
    buttonRemoveFilter = new wxButton(notebook_1_pane_1, BUTTON_REMOVE_FILTER, wxT("Remove"));
    buttonClearFilters = new wxButton(notebook_1_pane_1, BUTTON_CLEAR_FILTERS, wxT("Clear"));
    panelVectorFiles = new FileSetPanel(notebook_1_pane_2, wxID_ANY);
    buttonApplyVector = new wxButton(notebook_1_pane_2, BUTTON_APPLY_VECTOR, wxT("Apply"));
    label_10 = new wxStaticText(notebook_1_pane_2, wxID_ANY, wxT("Method"));
    const wxString comboVectorMethod_choices[] = {
        wxT("Glyph"),
        wxT("Height Map")
    };
    comboVectorMethod = new wxComboBox(notebook_1_pane_2, COMBO_VECTOR_METHOD, wxT(""), wxDefaultPosition, wxDefaultSize, 2, comboVectorMethod_choices, wxCB_DROPDOWN|wxCB_READONLY);
    rwiView = new wxVTKRenderWindowInteractor(panelRight, wxID_ANY);
    slideImageIndex = new wxIntSlider(panelRight, SLIDE_IMAGE_INDEX);
    buttonFirst = new wxButton(panelRight, BUTTON_FIRST, wxT("[<"));
    buttonPrevious = new wxButton(panelRight, BUTTON_PREVIOUS, wxT("|<"));
    buttonRewind = new wxButton(panelRight, BUTTON_REWIND, wxT("<"));
    buttonPause = new wxButton(panelRight, BUTTON_PAUSE, wxT("||"));
    buttonPlay = new wxButton(panelRight, BUTTON_PLAY, wxT(">"));
    buttonNext = new wxButton(panelRight, BUTTON_NEXT, wxT(">|"));
    buttonLast = new wxButton(panelRight, BUTTON_LAST, wxT(">]"));

    set_properties();
    do_layout();
    // end wxGlade
    
    // Custom code ! //
    
    // Set up logging
    // Log to a file in our application directory
    std::string logFileName(
        nano::wx2std(wxGetApp().GetTraits()->GetStandardPaths().GetExecutablePath()));
    logFileName = DirectoryPart(logFileName);
    logFileName.append("itlog.txt");
    Logger::setLevel(All);
    this->logfile = new std::ofstream(logFileName.c_str());
    Logger::SetStream((*this->logfile));
//     std::cout << "Logging to file: " << logFileName << std::endl;
    Logger::info << "Logging to file: " << logFileName << std::endl;
    
    // Set up the frame slider
    this->slideImageIndex->SetRange(0,1);
    this->slideImageIndex->SetValue(0);
    this->slideImageIndex->SetEnforceRange(true);
    
    // Set up the interactor style
    vtkInteractorStyleSwitch* interact = vtkInteractorStyleSwitch::New();
    interact->SetCurrentStyleToTrackballCamera();
    this->rwiView->SetInteractorStyle(interact);
    
    // Set up the ImageTracker controller
    ImageTrackerController::Instance()->SetRenderWindow(this->rwiView->GetRenderWindow());
    ImageTrackerController::Instance()->SetFilterParent(this->panelFilterControl);
    ImageTrackerController::Instance()->ClearFilters();
    
    // Initialize the filter list
    this->UpdateFilterList();
    this->SetPlayState(Pause);
    this->loopPlay = true;
    menuView->Check(MENU_LOOP_PLAY, this->loopPlay);
    this->panelVectorFiles->SetFilter(VECTOR_FILE_FILTER);
    
    // Initialize control dialogs
    this->dlgAbout = new AboutDialog(this, -1, wxT("About ImageTracker"));
    this->dlgAbout->SetMessage(
        APP_NAME + " " + APP_VERSION + "\n" + 
        APP_AUTHOR + " " + APP_COPYRIGHT + "\n" + 
        APP_WEBSITE);
    
    this->dlgLogger = new LoggerDialog(this, -1, wxT("Logger"));
    this->dlgLogger->SetFileName(logFileName);
    
    this->dlgSaveVisualization = new SaveVisualizationDialog(this, -1, wxT("Save View Images"));
    this->dlgScalarImageControl = new ScalarImageControlDialog(this, -1, wxT("Window/Contrast"));
    
    this->dlgApplyTransform = new ApplyTransformDialog(this, -1, wxT("Apply Transfrom"));
    this->dlgRegistration = new MultiResolutionRegistrationDialog(this, -1, wxT("Register Images"));
    this->dlgRemoveOcclusions = new RemoveOcclusionsDialog(this, -1, wxT("Remove Occlusions"));
    this->dlgCLGOpticalFlow = new CLGOpticalFlowDialog(this, -1, wxT("CLG Optical Flow"));
    this->dlgHornOpticalFlow = new HornOpticalFlowDialog(this, -1, wxT("Horn & Schunck Optical Flow"));
    this->dlgIntegrateFlow = new IntegrateFlowDialog(this, -1, wxT("Integrate Flow"));
    
    // Initialize recording states
    this->saveFormat = "visualization-%04d.tif";
    this->saveFromIdx = 0;
    this->saveToIdx = 0;
}

ImageTracker::~ImageTracker()
{
    if (this->rwiView)
        this->rwiView->Delete();
    
    if (this->logfile)
        this->logfile->close();
}


BEGIN_EVENT_TABLE(ImageTracker, wxFrame)
    EVT_IDLE(ImageTracker::OnIdle)
    EVT_SLIDER(SLIDE_IMAGE_INDEX, ImageTracker::OnImageIndex)
    // begin wxGlade: ImageTracker::event_table
    EVT_MENU(MENU_SAVE_IMAGES, ImageTracker::OnSaveViewImages)
    EVT_MENU(MENU_EXIT, ImageTracker::OnExit)
    EVT_MENU(MENU_THRESHOLD, ImageTracker::OnThreshold)
    EVT_MENU(MENU_GAUSSIAN, ImageTracker::OnGaussian)
    EVT_MENU(MENU_OCCLUSIONS, ImageTracker::OnOcclusions)
    EVT_MENU(MENU_STABILIZE, ImageTracker::OnStabilize)
    EVT_MENU(MENU_APPLY_TRANSFORM, ImageTracker::OnApplyTransform)
    EVT_MENU(MENU_CLG_OPTICAL_FLOW, ImageTracker::OnCLGOpticalFlow)
    EVT_MENU(MENU_INTEGRATE_FLOW, ImageTracker::OnIntegrateFlow)
    EVT_MENU(MENU_HORN_OPTICAL_FLOW, ImageTracker::OnHornOpticalFlow)
    EVT_MENU(MENU_WINDOW_CONTRAST, ImageTracker::OnWindowContrast)
    EVT_MENU(MENU_LOGGER, ImageTracker::OnLogger)
    EVT_MENU(MENU_IMAGE_INFO, ImageTracker::OnImageInfo)
    EVT_MENU(MENU_LOOP_PLAY, ImageTracker::OnLoopPlay)
    EVT_MENU(MENU_ABOUT, ImageTracker::OnAbout)
    EVT_LISTBOX_DCLICK(LIST_FILTERS, ImageTracker::OnSelectFilter)
    EVT_LISTBOX(LIST_FILTERS, ImageTracker::OnSelectFilter)
    EVT_BUTTON(BUTTON_REMOVE_FILTER, ImageTracker::OnRemoveFilter)
    EVT_BUTTON(BUTTON_CLEAR_FILTERS, ImageTracker::OnClearFilters)
    EVT_BUTTON(BUTTON_APPLY_VECTOR, ImageTracker::OnApplyVector)
    EVT_COMBOBOX(COMBO_VECTOR_METHOD, ImageTracker::OnVectorMethod)
    EVT_BUTTON(BUTTON_FIRST, ImageTracker::OnFirstFrame)
    EVT_BUTTON(BUTTON_PREVIOUS, ImageTracker::OnPrevious)
    EVT_BUTTON(BUTTON_REWIND, ImageTracker::OnRewind)
    EVT_BUTTON(BUTTON_PAUSE, ImageTracker::OnPause)
    EVT_BUTTON(BUTTON_PLAY, ImageTracker::OnPlay)
    EVT_BUTTON(BUTTON_NEXT, ImageTracker::OnNext)
    EVT_BUTTON(BUTTON_LAST, ImageTracker::OnLastFrame)
    // end wxGlade
END_EVENT_TABLE();

void ImageTracker::OnIdle(wxIdleEvent &event)
{
	std::string function("ImageTracker::OnIdle");
	// Check for data changes
	if (ImageTrackerController::Instance()->CheckForUpdates())
	{
		ImageTrackerController::Instance()->SetIsDataChanged(false);
		this->UpdateFilterList();
    }
        
    // Update view
    this->UpdatePlayState();
    ImageTrackerController::Instance()->UpdateView();
}

void ImageTracker::OnOpen(wxCommandEvent &event)
{
    event.Skip();
    wxLogDebug(wxT("Event handler (ImageTracker::OnOpen) not implemented yet")); //notify the user that he hasn't implemented the event handler yet
}


void ImageTracker::OnSaveViewImages(wxCommandEvent &event)
{
    Logger::warning << "Please do not put anything on top of the image window while the visualizations are being saved!" << std::endl;
    if (this->dlgSaveVisualization->ShowModal() == wxID_OK)
    {
        this->saveFormat = this->dlgSaveVisualization->GetFileFormat();
        this->saveFromIdx = this->dlgSaveVisualization->GetIndexStart();
        this->saveToIdx = this->dlgSaveVisualization->GetIndexEnd();
        this->SetPlayState(AboutToRecord);
    }
}


void ImageTracker::OnExit(wxCommandEvent &event)
{
    this->Destroy();
}

void ImageTracker::OnThreshold(wxCommandEvent &event)
{
    if (ImageTrackerController::Instance()->GetImageCount() < 1)
    {
        wxMessageDialog alert(this, wxT("Please load some image files first."), wxT("No data"), wxOK);
        alert.ShowModal();
        return;
    }
    
    ThresholdPanel* threshold = new ThresholdPanel(this->panelFilterControl, -1);
    ImageTrackerController::Instance()->AddFilter(threshold);
}

void ImageTracker::OnGaussian(wxCommandEvent &event)
{
    if (ImageTrackerController::Instance()->GetImageCount() < 1)
    {
        wxMessageDialog alert(this, wxT("Please load some image files first."), wxT("No data"), wxOK);
        alert.ShowModal();
        return;
    }
    
    GaussianFilterPanel* gaussian = new GaussianFilterPanel(this->panelFilterControl, -1);
    ImageTrackerController::Instance()->AddFilter(gaussian);
}


void ImageTracker::OnOcclusions(wxCommandEvent &event)
{
    if (ImageTrackerController::Instance()->GetImageCount() < 2)
    {
        wxMessageDialog alert(this, wxT("Please load at least two image files first."), wxT("No data"), wxOK);
        alert.ShowModal();
        return;
    }
    
    this->theStatusBar->SetStatusText(wxT("Removing occlusions..."));
    this->SetPlayState(Pause);  // pause play, at least initially
    
    this->dlgRemoveOcclusions->SetInput(ImageTrackerController::Instance()->GetImageFileSet());
    this->dlgRemoveOcclusions->Show(true);
}


void ImageTracker::OnStabilize(wxCommandEvent &event)
{
    if (ImageTrackerController::Instance()->GetImageCount() < 2)
    {
        wxMessageDialog alert(this, wxT("Please load at least two image files first."), wxT("No data"), wxOK);
        alert.ShowModal();
        return;
    }
    
    this->theStatusBar->SetStatusText(wxT("Stabilizing..."));
    this->SetPlayState(Pause);  // pause play, at least initially
    
    this->dlgRegistration->SetInput(ImageTrackerController::Instance()->GetImageFileSet());
    this->dlgRegistration->Show(true);
}


void ImageTracker::OnApplyTransform(wxCommandEvent &event)
{
    if (ImageTrackerController::Instance()->GetImageCount() == 0)
    {
        wxMessageDialog alert(this, wxT("Please load some files first."), wxT("No data"), wxOK);
        alert.ShowModal();
        return;
    }
    
    this->theStatusBar->SetStatusText(wxT("Applying transform..."));
    this->SetPlayState(Pause);  // pause play, at least initially
    
    this->dlgApplyTransform->SetInput(ImageTrackerController::Instance()->GetImageFileSet());
    this->dlgApplyTransform->Show(true);
}


void ImageTracker::OnCLGOpticalFlow(wxCommandEvent &event)
{
    if (ImageTrackerController::Instance()->GetImageCount() < 2)
    {
        wxMessageDialog alert(this, wxT("Please load at least two image files."), wxT("No data"), wxOK);
        alert.ShowModal();
        return;
    }
    
    this->theStatusBar->SetStatusText(wxT("Computing optical flow..."));
    this->SetPlayState(Pause);  // pause play, at least initially
    
    this->dlgCLGOpticalFlow->SetInput(ImageTrackerController::Instance()->GetImageFileSet());
    this->dlgCLGOpticalFlow->Show(true);
}


void ImageTracker::OnHornOpticalFlow(wxCommandEvent &event)
{
    if (ImageTrackerController::Instance()->GetImageCount() < 2)
    {
        wxMessageDialog alert(this, wxT("Please load at least two image files."), wxT("No data"), wxOK);
        alert.ShowModal();
        return;
    }
    
    this->theStatusBar->SetStatusText(wxT("Computing optical flow..."));
    this->SetPlayState(Pause);  // pause play, at least initially
    
    this->dlgHornOpticalFlow->SetInput(ImageTrackerController::Instance()->GetImageFileSet());
    this->dlgHornOpticalFlow->Show(true);
}


void ImageTracker::OnIntegrateFlow(wxCommandEvent &event)
{
    if (ImageTrackerController::Instance()->GetVectorCount() < 2)
    {
        wxMessageDialog alert(this, wxT("Please load at least two vector files."), wxT("No data"), wxOK);
        alert.ShowModal();
        return;
    }
    
    this->theStatusBar->SetStatusText(wxT("Integrating flow..."));
    this->SetPlayState(Pause);  // pause play, at least initially
    
    this->dlgIntegrateFlow->SetInput(ImageTrackerController::Instance()->GetVectorFileSet());
    this->dlgIntegrateFlow->Show(true);
}


void ImageTracker::OnWindowContrast(wxCommandEvent &event)
{
    this->dlgScalarImageControl->Show(true);
}

void ImageTracker::OnImageInfo(wxCommandEvent &event)
{
    std::string function("ImageTracker::OnImageInfo");
    
    if (ImageTrackerController::Instance()->GetImageCount() < 1)
    {
        Logger::warning << function << ": There are no images open for which to provide information." << std::endl;
    }
    else
    {
        PrintImageInfo(ImageTrackerController::Instance()->GetImageFileSet()->GetOutput(), "", Logger::info);
    }
    this->OnLogger(event);
}


void ImageTracker::OnLogger(wxCommandEvent &event)
{
    this->dlgLogger->Show(true);
}


void ImageTracker::OnLoopPlay(wxCommandEvent &event)
{
    this->loopPlay = event.IsChecked();
}


void ImageTracker::OnAbout(wxCommandEvent &event)
{
    this->theStatusBar->SetStatusText(wxT("About..."));
    this->dlgAbout->Show(true);
}


void ImageTracker::OnSelectFilter(wxCommandEvent &event)
{
    this->theStatusBar->SetStatusText(wxT("Editing filter"), 0);
    
    int idx = this->listFilters->GetSelection();
	if (idx != wxNOT_FOUND)
	{
		wxWindow* panel = ImageTrackerController::Instance()->GetFilterPanel(idx);
    
	    wxSizer* sizer = this->panelFilterControl->GetSizer();

		// TODO: Not sure this is necessary...Clear(false) seems to well on Windows....
		// But there seem to be some problems on Linux....
		// Actually, this seems not to work in release mode on Windows.
		wxSizerItem* item = sizer->GetItem((size_t)0);
		if (item)
		{
			wxWindow* oldPanel = item->GetWindow();
			if (oldPanel)
				oldPanel->Show(false);
		}
	    
		sizer->Clear(false);
		sizer->Add(panel, 3, wxEXPAND, 0);
		panel->Show(true);
		sizer->Layout();
	}
    
	// this lets double clicks also register events
	event.Skip();
}

void ImageTracker::OnRemoveFilter(wxCommandEvent &event)
{
    int idx = this->listFilters->GetSelection();
    if (idx != wxNOT_FOUND)
    {
        ImageTrackerController::Instance()->RemoveFilter(idx);
    }
}

void ImageTracker::OnClearFilters(wxCommandEvent &event)
{
    ImageTrackerController::Instance()->ClearFilters();
}


void ImageTracker::OnImageIndex(wxCommandEvent &event)
{
    ImageTrackerController::Instance()->SetImageIndex(this->slideImageIndex->GetValue());
}

void ImageTracker::OnFirstFrame(wxCommandEvent &event)
{
    this->SetPlayState(SkipFirst);
}


void ImageTracker::OnPrevious(wxCommandEvent &event)
{
    this->SetPlayState(StepBackward);
}


void ImageTracker::OnRewind(wxCommandEvent &event)
{
    this->SetPlayState(Rewind);
}


void ImageTracker::OnPause(wxCommandEvent &event)
{
    this->SetPlayState(Pause);
}


void ImageTracker::OnPlay(wxCommandEvent &event)
{
    this->SetPlayState(Play);
}


void ImageTracker::OnNext(wxCommandEvent &event)
{
    this->SetPlayState(StepForward);
}


void ImageTracker::OnLastFrame(wxCommandEvent &event)
{
    this->SetPlayState(SkipLast);
}


void ImageTracker::OnApplyVector(wxCommandEvent &event)
{
    ImageTrackerController::Instance()->SetVectorFiles(this->panelVectorFiles->GetFileSet());
    this->OnVectorMethod(event);
}


void ImageTracker::OnVectorMethod(wxCommandEvent &event)
{
    int method = this->comboVectorMethod->GetSelection();
    if (method == wxNOT_FOUND) // not likely to happen
        return;
    
    ItkVtkPipeline::Pointer vis;
    
    if (method == 0)
    {
        vis = VectorGlyphVisualization::New();
    }
    else
    {
        vis = VectorHeightMapVisualization::New();
    }
    
    ImageTrackerController::Instance()->SetVectorVisualization(vis);
    wxWindow* panel = vis->CreateWxControl(this->panelVectorControl);
    
    wxSizer* sizer = this->panelVectorControl->GetSizer();
    wxSizerItem* item = sizer->GetItem((size_t)0);
    if (item)
    {
        wxWindow* oldPanel = item->GetWindow();
        if (oldPanel)
        {
            oldPanel->Show(false);
            delete (oldPanel);
        }
    }
    
    sizer->Clear(false);
    sizer->Add(panel, 1, wxEXPAND, 0);
    panel->Show(true);
    sizer->Layout();
}


// wxGlade: add ImageTracker event handlers


void ImageTracker::set_properties()
{
    // begin wxGlade: ImageTracker::set_properties
    SetSize(wxSize(960, 700));
    int theStatusBar_widths[] = { -1 };
    theStatusBar->SetStatusWidths(1, theStatusBar_widths);
    const wxString theStatusBar_fields[] = {
        wxT("Welcome!")
    };
    for(int i = 0; i < theStatusBar->GetFieldsCount(); ++i) {
        theStatusBar->SetStatusText(theStatusBar_fields[i], i);
    }
    listFilters->SetToolTip(wxT("The list of active filters"));
    buttonRemoveFilter->SetToolTip(wxT("Remove the selected filter from the filter list"));
    buttonClearFilters->SetToolTip(wxT("Clear the existing file set and filters"));
    buttonApplyVector->SetToolTip(wxT("Load the selected vector images into ImageTracker"));
    comboVectorMethod->SetSelection(0);
    slideImageIndex->SetToolTip(wxT("Frame index"));
    slideImageIndex->Enable(false);
    buttonFirst->SetMinSize(wxSize(30, 30));
    buttonFirst->SetToolTip(wxT("Skip to beginning"));
    buttonPrevious->SetMinSize(wxSize(30, 30));
    buttonPrevious->SetToolTip(wxT("Back one frame"));
    buttonRewind->SetMinSize(wxSize(30, 30));
    buttonRewind->SetToolTip(wxT("Rewind"));
    buttonPause->SetMinSize(wxSize(30, 30));
    buttonPause->SetToolTip(wxT("Pause"));
    buttonPlay->SetMinSize(wxSize(30, 30));
    buttonPlay->SetToolTip(wxT("Play"));
    buttonNext->SetMinSize(wxSize(30, 30));
    buttonNext->SetToolTip(wxT("Forward one frame"));
    buttonLast->SetMinSize(wxSize(30, 30));
    buttonLast->SetToolTip(wxT("Skip to end"));
    // end wxGlade
}


void ImageTracker::do_layout()
{
    // begin wxGlade: ImageTracker::do_layout
    wxBoxSizer* sizer_1 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_9 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_10 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_32 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_33 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_3 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_6 = new wxBoxSizer(wxVERTICAL);
    wxStaticBoxSizer* sizer_41 = new wxStaticBoxSizer(sizer_41_staticbox, wxVERTICAL);
    wxBoxSizer* sizer_65 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_62 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_22 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_46 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_4 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_47 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_61 = new wxBoxSizer(wxHORIZONTAL);
    sizer_47->Add(listFilters, 1, wxEXPAND, 0);
    sizer_61->Add(buttonRemoveFilter, 0, 0, 0);
    sizer_61->Add(buttonClearFilters, 0, 0, 0);
    sizer_47->Add(sizer_61, 0, wxALIGN_CENTER_HORIZONTAL, 0);
    sizer_46->Add(sizer_47, 1, wxEXPAND, 0);
    sizer_4->Add(20, 20, 0, 0, 0);
    panelFilterControl->SetSizer(sizer_4);
    sizer_46->Add(panelFilterControl, 3, wxEXPAND, 0);
    notebook_1_pane_1->SetSizer(sizer_46);
    sizer_22->Add(panelVectorFiles, 1, wxEXPAND, 0);
    sizer_22->Add(buttonApplyVector, 0, wxALIGN_CENTER_HORIZONTAL, 0);
    sizer_6->Add(sizer_22, 2, wxEXPAND, 0);
    sizer_62->Add(label_10, 0, 0, 0);
    sizer_62->Add(comboVectorMethod, 0, 0, 0);
    sizer_41->Add(sizer_62, 0, wxEXPAND, 0);
    sizer_65->Add(20, 20, 0, 0, 0);
    panelVectorControl->SetSizer(sizer_65);
    sizer_41->Add(panelVectorControl, 1, wxEXPAND, 0);
    sizer_6->Add(sizer_41, 3, wxEXPAND, 0);
    notebook_1_pane_2->SetSizer(sizer_6);
    notebookData->AddPage(notebook_1_pane_1, wxT("Images"));
    notebookData->AddPage(notebook_1_pane_2, wxT("Vectors"));
    sizer_3->Add(notebookData, 1, wxEXPAND, 0);
    panelLeft->SetSizer(sizer_3);
    sizer_10->Add(rwiView, 1, wxALL|wxEXPAND, 0);
    sizer_32->Add(slideImageIndex, 1, wxEXPAND|wxALIGN_BOTTOM, 0);
    sizer_33->Add(buttonFirst, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    sizer_33->Add(buttonPrevious, 0, 0, 0);
    sizer_33->Add(buttonRewind, 0, wxALIGN_CENTER_VERTICAL, 0);
    sizer_33->Add(buttonPause, 0, wxALIGN_CENTER_VERTICAL, 0);
    sizer_33->Add(buttonPlay, 0, wxALIGN_CENTER_VERTICAL, 0);
    sizer_33->Add(buttonNext, 0, 0, 0);
    sizer_33->Add(buttonLast, 0, wxALIGN_CENTER_VERTICAL, 0);
    sizer_32->Add(sizer_33, 0, wxALIGN_CENTER_HORIZONTAL, 0);
    sizer_10->Add(sizer_32, 0, wxEXPAND, 0);
    panelRight->SetSizer(sizer_10);
    vsplitDataView->SplitVertically(panelLeft, panelRight, 350);
    sizer_9->Add(vsplitDataView, 1, wxEXPAND, 0);
    panelMainUpper->SetSizer(sizer_9);
    sizer_1->Add(panelMainUpper, 1, wxEXPAND, 0);
    SetSizer(sizer_1);
    Layout();
    // end wxGlade
}


bool ImageTracker::Destroy()
{
    // Delete the ImageTrackerController instance first because it
    // is a static object that holds application resources.
    ImageTrackerController::DeleteInstance();
    wxFrame::Destroy();
    return true;
}

void ImageTracker::UpdateFilterList()
{
	std::string function("ImageTracker::UpdateFilterList");
	
    // Update the list of filters
    int oldIdx = this->listFilters->GetSelection();
    Logger::verbose << function << ": previous selection => " << oldIdx << std::endl;
	wxArrayString names;
    ImageTrackerController::Instance()->GetFilterNames(names);
    this->listFilters->Set(names);
    
    // Update the image index slider
    int imgCount = ImageTrackerController::Instance()->GetImageCount();
    int vecCount = ImageTrackerController::Instance()->GetVectorCount();
    int fullCount = std::max(imgCount, vecCount);
    this->slideImageIndex->SetRange(0, std::max(fullCount-1, 1));
    this->slideImageIndex->Enable(fullCount > 1);

	// Update the list of vectors
	this->panelVectorFiles->SetFileSet(ImageTrackerController::Instance()->GetVectorFiles());

	// Update the selected filter
	int filterCount = this->listFilters->GetCount();
	if (filterCount > 0 && oldIdx != wxNOT_FOUND)
	{
		int newIdx = std::min(oldIdx, filterCount - 1);		// prefer the last filter in the list if count changed
		newIdx = std::max(newIdx, 0);						// prefer the first filter rather than none
		Logger::verbose << function << ": next selection => " << newIdx << std::endl;
		this->listFilters->SetSelection(newIdx);

		// Force display of the control panel
		wxCommandEvent evt;
		this->OnSelectFilter(evt);
	}
}

void ImageTracker::UpdatePlayState()
{
    int curIdx = this->slideImageIndex->GetValue();    
    int imgCount = ImageTrackerController::Instance()->GetImageCount();
    int vecCount = ImageTrackerController::Instance()->GetVectorCount();
    int maxIdx = std::max(imgCount, vecCount) - 1;
    int nextIdx = curIdx;
    
    int target;
    
    // Set the next index and play state based on the current state.
    // A state machine.
    switch (this->GetPlayState())
    {
        case Play:  // advance one frame
            if (curIdx < maxIdx)
            {
                nextIdx = curIdx + 1;
            }
            else if (this->loopPlay)  // loop to beginning
            {
                nextIdx = 0;
            }
            else
            {
                this->SetPlayState(Pause);
            }
            break;
        case Rewind:    // retreat one frame
            if (curIdx > 0)
            {
                nextIdx = curIdx - 1;
            }
            else if (this->loopPlay) // loop to end
            {
                nextIdx = maxIdx;
            }
            else
            {
                this->SetPlayState(Pause);
            }
            break;
        case StepForward:   // move forward one frame and stop
            target = curIdx + 1;
            if (target >= 0 && target <= maxIdx)
                nextIdx = target;
            this->SetPlayState(Pause);
            break;
        case StepBackward:  // move backward one frame and stop
            target = curIdx - 1;
            if (target >= 0 && target <= maxIdx)
                nextIdx = target;
            this->SetPlayState(Pause);
            break;
        case SkipFirst:     // go to the first frame
            if (maxIdx > 0)
                nextIdx = 0;
            this->SetPlayState(Pause);
            break;
        case SkipLast:      // go to the last frame
            if (maxIdx > 0)
                nextIdx = maxIdx;
            this->SetPlayState(Pause);
            break;
        case AboutToRecord:
            if (maxIdx > 0) // ensure data is loaded
            {
                nextIdx = this->saveFromIdx;
                this->SetPlayState(Record);
            }
            else
            {
                this->SetPlayState(Pause);
            }
            break;
        case Record:
            // save the current visualization
            char filename[80];
            sprintf(filename, this->saveFormat.c_str(), curIdx);
            ImageTrackerController::Instance()->SaveViewImage(std::string(filename));
            if (curIdx < this->saveToIdx)
            {
                nextIdx = curIdx + 1;
            }
            else
            {
                this->SetPlayState(Pause);
                wxMessageDialog alert(this, wxT("Recording complete."), wxT("Recording"), wxOK);
                alert.ShowModal();
            }
            break;
        case Pause: // pause does nothing--stable, default state
        default:
            break;
    }
    
    // Set the image index
    if (nextIdx != curIdx)
    {
        this->slideImageIndex->SetValue(nextIdx);
        ImageTrackerController::Instance()->SetImageIndex(nextIdx);
    }
}
