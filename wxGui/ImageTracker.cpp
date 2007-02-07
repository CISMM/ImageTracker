// -*- C++ -*- generated by wxGlade 0.4cvs on Mon Oct  2 16:06:25 2006

#include "ImageTracker.h"

#include <string>

#include "vtkInteractorStyleSwitch.h"

#include "Logger.h"
#include "wxUtils.h"

static const std::string APP_NAME("ImageTracker");
static const std::string APP_VERSION("v 2.03");
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
    EVT_COMMAND_ENTER(IMAGE_TRACKER_CONTROLLER, ImageTracker::OnDataSourceChange)
    EVT_MENU(MENU_OPEN, ImageTracker::OnOpen)
    EVT_MENU(MENU_EXIT, ImageTracker::OnExit)
    EVT_MENU(MENU_ABOUT, ImageTracker::OnAbout)
    EVT_MENU(MENU_OCCLUSIONS, ImageTracker::OnOcclusions)
    EVT_MENU(MENU_STABILIZE, ImageTracker::OnStabilize)
    EVT_MENU(MENU_CLG_OPTIC_FLOW, ImageTracker::OnCLGOpticFlow)
    // begin wxGlade: ImageTracker::event_table
    EVT_LISTBOX_DCLICK(LBX_DATASOURCES, ImageTracker::OnEditDataSource)
    EVT_BUTTON(BTN_ADD_DATASOURCE, ImageTracker::OnAddDataSource)
    EVT_BUTTON(BTN_REMOVE_DATASOURCE, ImageTracker::OnRemoveDataSource)
    EVT_COMMAND_SCROLL(SLD_IMAGE_INDEX, ImageTracker::OnImageIndexScroll)
    // end wxGlade
END_EVENT_TABLE();

void ImageTracker::OnOpen(wxCommandEvent &event)
{
    // Pass this off to another event handler
    this->OnAddDataSource(event);
}

void ImageTracker::OnExit(wxCommandEvent &event)
{
    this->Destroy();
}

void ImageTracker::OnAbout(wxCommandEvent &event)
{
    this->theStatusBar->SetStatusText(wxT("About..."));
    this->dlgAbout->Show(true);
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
    this->dlgRemoveOcclusions->SetInput(this->controller->GetDataSource(idx));
    this->dlgRemoveOcclusions->SetController(this->controller);
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
    
    // Find the user's selected DataSource.
    // Find the user's selected DataSource
    int idx = this->lbxSources->GetSelection();
    idx = (idx == wxNOT_FOUND) ? 0 : idx;
    if (this->controller->GetDataSource(idx)->size() < 2) // Need at least two images to register.
    {
        wxMessageDialog alert(this, wxT("This operation requires at least two images in the data source."),
                              wxT("Not enough images."), wxOK);
        alert.ShowModal();
        return;
    }
    this->dlgRegistration->SetInput(this->controller->GetDataSource(idx));
    this->dlgRegistration->SetController(this->controller);
    this->dlgRegistration->Show(true);
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
    
    this->theStatusBar->SetStatusText(wxT("Computing optic flow..."));
    
    // Find the user's selected DataSource.
    // Find the user's selected DataSource
    int idx = this->lbxSources->GetSelection();
    idx = (idx == wxNOT_FOUND) ? 0 : idx;
    if (this->controller->GetDataSource(idx)->size() < 2) // Need at least two images to register.
    {
        wxMessageDialog alert(this, wxT("This operation requires at least two images in the data source."),
                              wxT("Not enough images."), wxOK);
        alert.ShowModal();
        return;
    }
    
    this->dlgCLGOpticFlow->SetInput(this->controller->GetDataSource(idx));
    this->dlgCLGOpticFlow->SetController(this->controller);
    this->dlgCLGOpticFlow->Show(true);
}

void ImageTracker::OnRemoveDataSource(wxCommandEvent &event)
{
    this->theStatusBar->SetStatusText(wxT("Removing data source..."),0);
    
    int idx = this->lbxSources->GetSelection();
    if (idx != wxNOT_FOUND)
    {
        this->controller->RemoveDataSource(idx);
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
        this->controller->AddDataSource(this->dlgDataSource->GetDataSource());
        this->theStatusBar->SetStatusText(wxT("Added data source"));
    }
    else
    {
        this->theStatusBar->SetStatusText(wxT("No new data source"));
    }
}

void ImageTracker::OnEditDataSource(wxCommandEvent &event)
{
    this->theStatusBar->SetStatusText(wxT("Editing data source..."),0);
    
    int idx = this->lbxSources->GetSelection();
    if (idx != wxNOT_FOUND)
    {
        this->dlgDataSource->SetNewSource(false);
        this->dlgDataSource->SetDataSource(this->controller->GetDataSource(idx));
        if (this->dlgDataSource->ShowModal() == wxID_OK)
        {
            this->controller->UpdateView();
            this->UpdateDataSources();
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
    this->controller->SetIndex(this->sldImageIndex->GetValue());
}

// wxGlade: add ImageTracker event handlers

void ImageTracker::OnDataSourceChange(wxCommandEvent &event)
{
    this->UpdateDataSources();
}

void ImageTracker::UpdateDataSources()
{
    int idx = this->lbxSources->GetSelection();
    wxArrayString names;
    this->controller->GetDataSourceNames(names);
    this->lbxSources->Set(names);
    this->lbxSources->SetSelection(idx);
    this->sldImageIndex->SetRange(0, this->controller->GetMaxSize()-1);
}

ImageTracker::ImageTracker(wxWindow* parent, int id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
    wxFrame(parent, id, title, pos, size, wxDEFAULT_FRAME_STYLE)
{
    // begin wxGlade: ImageTracker::ImageTracker
    hsplitDataLogger = new wxSplitterWindow(this, -1, wxDefaultPosition, wxDefaultSize, wxSP_3D|wxSP_BORDER);
    window_1_pane_2 = new wxPanel(hsplitDataLogger, -1);
    window_1_pane_1 = new wxPanel(hsplitDataLogger, -1);
    vsplitDataView = new wxSplitterWindow(window_1_pane_1, -1, wxDefaultPosition, wxDefaultSize, wxSP_3D|wxSP_BORDER);
    window_2_pane_2 = new wxPanel(vsplitDataView, -1);
    window_2_pane_1 = new wxPanel(vsplitDataView, -1);
    sizer_4_staticbox = new wxStaticBox(window_2_pane_1, -1, wxT("Data Sources"));
    itMenuBar = new wxMenuBar();
    SetMenuBar(itMenuBar);
    wxMenu* menuFile = new wxMenu();
    menuFile->Append(MENU_OPEN, wxT("&Open"), wxT("Open a set of image files"), wxITEM_NORMAL);
    menuFile->Append(MENU_EXIT, wxT("E&xit"), wxT("Quit ImageTracker"), wxITEM_NORMAL);
    itMenuBar->Append(menuFile, wxT("&File"));
    wxMenu* menuEnhance = new wxMenu();
    menuEnhance->Append(MENU_OCCLUSIONS, wxT("&Remove Occlusions"), wxT("Detect and remove static partial occlusions from bright-field microscopy images"), wxITEM_NORMAL);
    menuEnhance->Append(MENU_STABILIZE, wxT("&Stabilize"), wxT("Remove global drift to keep an object stationary"), wxITEM_NORMAL);
    itMenuBar->Append(menuEnhance, wxT("&Enhance"));
    wxMenu* menuCompute = new wxMenu();
    menuCompute->Append(MENU_CLG_OPTIC_FLOW, wxT("CLG &Optic Flow"), wxT("Combined local-global motion computation"), wxITEM_NORMAL);
    itMenuBar->Append(menuCompute, wxT("&Compute"));
    wxMenu* menuHelp = new wxMenu();
    menuHelp->Append(MENU_ABOUT, wxT("&About"), wxT("About this application"), wxITEM_NORMAL);
    itMenuBar->Append(menuHelp, wxT("&Help"));
    theStatusBar = CreateStatusBar(1, 0);
    const wxString lbxSources_choices[] = {
        
    };
    lbxSources = new wxListBox(window_2_pane_1, LBX_DATASOURCES, wxDefaultPosition, wxDefaultSize, 0, lbxSources_choices, wxLB_SINGLE);
    btnAddDataSource = new wxButton(window_2_pane_1, BTN_ADD_DATASOURCE, wxT("+"));
    btnRemoveDataSource = new wxButton(window_2_pane_1, BTN_REMOVE_DATASOURCE, wxT("-"));
    rwiView = new wxVTKRenderWindowInteractor(window_2_pane_2, -1);
    sldImageIndex = new wxSlider(window_2_pane_2, SLD_IMAGE_INDEX, 0, 0, 10, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL|wxSL_LABELS);
    txtLogger = new wxTextCtrl(window_1_pane_2, -1, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);

    set_properties();
    do_layout();
    // end wxGlade
    
    // ----- Custom code! ----- //
    // Select log level, and redirect output to log panel
    Logger::setLevel(Verbose);
    this->coutRedirect = new wxStreamToTextRedirector(this->txtLogger);
    
    // Pre-create dialogs
    this->dlgDataSource = new DataSourceDialog(this, -1, wxT("Edit DataSource"));
    this->dlgRemoveOcclusions = new RemoveOcclusionsDialog(this, -1, wxT("Remove Partial Occlusions"));
    this->dlgRegistration = new MultiResolutionRegistrationDialog(this, -1, wxT("Stabilize"));
    this->dlgCLGOpticFlow = new CLGOpticFlowDialog(this, -1, wxT("CLG Optic Flow"));
    this->dlgAbout = new AboutDialog(this, -1, wxT("About"));
    this->dlgAbout->SetMessage(APP_NAME + " " + APP_VERSION + "\n" + APP_AUTHOR + " " + APP_COPYRIGHT + "\n" + APP_WEBSITE);
    
    // Set up the interactor style...we like the trackball camera
    vtkInteractorStyleSwitch* inter = vtkInteractorStyleSwitch::New();
    inter->SetCurrentStyleToTrackballCamera();
    this->rwiView->SetInteractorStyle(inter);
    
    // Set up the ImageTracker controller
    this->controller = ImageTrackerController::New();
    this->controller->SetParent(this, IMAGE_TRACKER_CONTROLLER);
    this->controller->SetRenderWindow(this->rwiView->GetRenderWindow());
}

void ImageTracker::set_properties()
{
    // begin wxGlade: ImageTracker::set_properties
    SetSize(wxSize(921, 623));
    int theStatusBar_widths[] = { -1 };
    theStatusBar->SetStatusWidths(1, theStatusBar_widths);
    const wxString theStatusBar_fields[] = {
        wxT("Welcome!")
    };
    for(int i = 0; i < theStatusBar->GetFieldsCount(); ++i) {
        theStatusBar->SetStatusText(theStatusBar_fields[i], i);
    }
    // end wxGlade
}


void ImageTracker::do_layout()
{
    // begin wxGlade: ImageTracker::do_layout
    wxBoxSizer* sizer_1 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_14 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_9 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_10 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_3 = new wxBoxSizer(wxVERTICAL);
    wxStaticBoxSizer* sizer_4 = new wxStaticBoxSizer(sizer_4_staticbox, wxVERTICAL);
    wxBoxSizer* sizer_6 = new wxBoxSizer(wxHORIZONTAL);
    sizer_4->Add(lbxSources, 1, wxEXPAND|wxADJUST_MINSIZE, 0);
    sizer_6->Add(btnAddDataSource, 1, wxALIGN_CENTER_HORIZONTAL|wxADJUST_MINSIZE, 0);
    sizer_6->Add(btnRemoveDataSource, 1, wxALIGN_CENTER_HORIZONTAL, 0);
    sizer_4->Add(sizer_6, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL, 1);
    sizer_3->Add(sizer_4, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxADJUST_MINSIZE, 0);
    window_2_pane_1->SetAutoLayout(true);
    window_2_pane_1->SetSizer(sizer_3);
    sizer_3->Fit(window_2_pane_1);
    sizer_3->SetSizeHints(window_2_pane_1);
    sizer_10->Add(rwiView, 1, wxEXPAND, 0);
    sizer_10->Add(sldImageIndex, 0, wxEXPAND|wxADJUST_MINSIZE, 0);
    window_2_pane_2->SetAutoLayout(true);
    window_2_pane_2->SetSizer(sizer_10);
    sizer_10->Fit(window_2_pane_2);
    sizer_10->SetSizeHints(window_2_pane_2);
    vsplitDataView->SplitVertically(window_2_pane_1, window_2_pane_2, 10);
    sizer_9->Add(vsplitDataView, 2, wxEXPAND, 0);
    window_1_pane_1->SetAutoLayout(true);
    window_1_pane_1->SetSizer(sizer_9);
    sizer_9->Fit(window_1_pane_1);
    sizer_9->SetSizeHints(window_1_pane_1);
    sizer_14->Add(txtLogger, 1, wxEXPAND|wxADJUST_MINSIZE, 0);
    window_1_pane_2->SetAutoLayout(true);
    window_1_pane_2->SetSizer(sizer_14);
    sizer_14->Fit(window_1_pane_2);
    sizer_14->SetSizeHints(window_1_pane_2);
    hsplitDataLogger->SplitHorizontally(window_1_pane_1, window_1_pane_2, 415);
    sizer_1->Add(hsplitDataLogger, 1, wxEXPAND, 0);
    SetAutoLayout(true);
    SetSizer(sizer_1);
    Layout();
    // end wxGlade
}

