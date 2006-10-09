// -*- C++ -*- generated by wxGlade 0.4cvs on Mon Oct  2 16:06:25 2006

#include "ImageTracker.h"
#include <string>
#include "wxUtils.h"

static const std::string APP_NAME("ImageTracker");
static const std::string APP_VERSION("2.03");
static const std::string APP_AUTHOR("Brian Eastwood");

bool ITApp::OnInit()
{
    ImageTracker *it = new ImageTracker((wxWindow *) NULL, -1, std2wx(APP_NAME + " " + APP_VERSION));
    it->Show(true);
    SetTopWindow(it);
    return true;
}

BEGIN_EVENT_TABLE(ImageTracker, wxFrame)
    EVT_MENU(MENU_EXIT, ImageTracker::OnExit)
    // begin wxGlade: ImageTracker::event_table
    EVT_BUTTON(BTN_ADD_FILES, ImageTracker::OnAddFiles)
    EVT_BUTTON(BTN_REMOVE_FILES, ImageTracker::OnRemoveFiles)
    // end wxGlade
END_EVENT_TABLE();

void ImageTracker::OnExit(wxCommandEvent &event)
{
    this->Destroy();
}

void ImageTracker::OnRemoveFiles(wxCommandEvent &event)
{
    this->theStatusBar->SetStatusText(wxT("OnRemoveFiles"),0);
    event.Skip();
}

void ImageTracker::OnAddFiles(wxCommandEvent &event)
{
    this->theStatusBar->SetStatusText(wxT("OnAddFiles"),0);
    event.Skip();
}
// wxGlade: add ImageTracker event handlers

ImageTracker::ImageTracker(wxWindow* parent, int id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
    wxFrame(parent, id, title, pos, size, wxDEFAULT_FRAME_STYLE)
{
    // begin wxGlade: ImageTracker::ImageTracker
    spltData = new wxSplitterWindow(this, -1, wxDefaultPosition, wxDefaultSize, wxSP_3D|wxSP_BORDER);
    pnlView = new wxPanel(spltData, -1);
    pnlData = new wxPanel(spltData, -1);
    sizer_5_staticbox = new wxStaticBox(pnlData, -1, wxT("Files"));
    sizer_4_staticbox = new wxStaticBox(pnlData, -1, wxT("Data Sources"));
    itMenuBar = new wxMenuBar();
    SetMenuBar(itMenuBar);
    wxMenu* menuFile = new wxMenu();
    menuFile->Append(wxNewId(), wxT("&Open"), wxT(""), wxITEM_NORMAL);
    menuFile->Append(MENU_EXIT, wxT("E&xit"), wxT("Quit ImageTracker"), wxITEM_NORMAL);
    itMenuBar->Append(menuFile, wxT("&File"));
    wxMenu* menuHelp = new wxMenu();
    menuHelp->Append(wxNewId(), wxT("&About"), wxT(""), wxITEM_NORMAL);
    itMenuBar->Append(menuHelp, wxT("&Help"));
    theStatusBar = CreateStatusBar(1, 0);
    const wxString lbxSources_choices[] = {
        
    };
    lbxSources = new wxListBox(pnlData, -1, wxDefaultPosition, wxDefaultSize, 0, lbxSources_choices, wxLB_EXTENDED);
    const wxString lbxFiles_choices[] = {
        
    };
    lbxFiles = new wxListBox(pnlData, -1, wxDefaultPosition, wxDefaultSize, 0, lbxFiles_choices, 0);
    btnAddFiles = new wxButton(pnlData, BTN_ADD_FILES, wxT("+"));
    btnRemoveFiles = new wxButton(pnlData, BTN_REMOVE_FILES, wxT("-"));
    rwiViewer = new wxVTKRenderWindowInteractor(pnlView, -1);
    sldFrameNumber = new wxSlider(pnlView, -1, 0, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL|wxSL_AUTOTICKS|wxSL_LABELS|wxSL_TOP);

    set_properties();
    do_layout();
    // end wxGlade
}

void ImageTracker::set_properties()
{
    // begin wxGlade: ImageTracker::set_properties
    SetSize(wxSize(930, 730));
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
    wxBoxSizer* sizer_7 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_3 = new wxBoxSizer(wxVERTICAL);
    wxStaticBoxSizer* sizer_5 = new wxStaticBoxSizer(sizer_5_staticbox, wxVERTICAL);
    wxBoxSizer* sizer_6 = new wxBoxSizer(wxHORIZONTAL);
    wxStaticBoxSizer* sizer_4 = new wxStaticBoxSizer(sizer_4_staticbox, wxVERTICAL);
    sizer_4->Add(lbxSources, 1, wxEXPAND|wxADJUST_MINSIZE, 0);
    sizer_3->Add(sizer_4, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxADJUST_MINSIZE, 0);
    sizer_5->Add(lbxFiles, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL, 0);
    sizer_6->Add(btnAddFiles, 1, wxALIGN_CENTER_HORIZONTAL|wxADJUST_MINSIZE, 0);
    sizer_6->Add(btnRemoveFiles, 1, wxALIGN_CENTER_HORIZONTAL, 0);
    sizer_5->Add(sizer_6, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL, 1);
    sizer_3->Add(sizer_5, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxADJUST_MINSIZE, 0);
    pnlData->SetAutoLayout(true);
    pnlData->SetSizer(sizer_3);
    sizer_3->Fit(pnlData);
    sizer_3->SetSizeHints(pnlData);
    sizer_7->Add(rwiViewer, 1, wxEXPAND, 0);
    sizer_7->Add(sldFrameNumber, 0, wxEXPAND|wxADJUST_MINSIZE, 0);
    pnlView->SetAutoLayout(true);
    pnlView->SetSizer(sizer_7);
    sizer_7->Fit(pnlView);
    sizer_7->SetSizeHints(pnlView);
    spltData->SplitVertically(pnlData, pnlView, 296);
    sizer_1->Add(spltData, 1, wxEXPAND, 0);
    SetAutoLayout(true);
    SetSizer(sizer_1);
    Layout();
    // end wxGlade
}
