// -*- C++ -*- generated by wxGlade 0.4cvs on Mon Oct  2 16:06:25 2006

#include <wx/wx.h>
#include <wx/image.h>

#ifndef IMAGETRACKER_H
#define IMAGETRACKER_H

// begin wxGlade: ::dependencies
#include <wx/splitter.h>
// end wxGlade

#include <string>

#include "AboutDialog.h"
#include "CLGOpticFlowDialog.h"
#include "DataSourceDialog.h"
#include "ImageTrackerController.h"
#include "MultiResolutionRegistrationDialog.h"
#include "RemoveOcclusionsDialog.h"
#include "wxVTKRenderWindowInteractor.h"

class ITApp : public wxApp
{
public:
    virtual bool OnInit();
};

class ImageTracker: public wxFrame {
public:
    // begin wxGlade: ImageTracker::ids
    // end wxGlade

    ImageTracker(wxWindow* parent, int id, const wxString& title, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=wxDEFAULT_FRAME_STYLE);

    virtual ~ImageTracker()
    {
        this->rwiView->Delete();
    }
    
    enum IDs
    { 
        MENU_EXIT           = 1000,
        MENU_ABOUT,
        MENU_OPEN,
        MENU_OCCLUSIONS,
        MENU_STABILIZE,
        MENU_CLG_OPTIC_FLOW,
        BTN_ADD_DATASOURCE  = 2000,
        BTN_REMOVE_DATASOURCE,
        LBX_DATASOURCES,
        SLD_IMAGE_INDEX,
        IMAGE_TRACKER_CONTROLLER = 1000000
    };
    
    /**
     * Updates the list of DataSources displayed in the ImageTracker.
     */
    void UpdateDataSources();
    
private:
    // begin wxGlade: ImageTracker::methods
    void set_properties();
    void do_layout();
    // end wxGlade
    
    ImageTrackerController::Pointer controller;
    DataSourceDialog* dlgDataSource;
    RemoveOcclusionsDialog* dlgRemoveOcclusions;
    MultiResolutionRegistrationDialog* dlgRegistration;
    CLGOpticFlowDialog* dlgCLGOpticFlow;
    AboutDialog* dlgAbout;
    wxStreamToTextRedirector* coutRedirect;
    
protected:
    // begin wxGlade: ImageTracker::attributes
    wxStaticBox* sizer_4_staticbox;
    wxMenuBar* itMenuBar;
    wxStatusBar* theStatusBar;
    wxListBox* lbxSources;
    wxButton* btnAddDataSource;
    wxButton* btnRemoveDataSource;
    wxPanel* window_2_pane_1;
    wxVTKRenderWindowInteractor* rwiView;
    wxSlider* sldImageIndex;
    wxPanel* window_2_pane_2;
    wxSplitterWindow* vsplitDataView;
    wxPanel* window_1_pane_1;
    wxTextCtrl* txtLogger;
    wxPanel* window_1_pane_2;
    wxSplitterWindow* hsplitDataLogger;
    // end wxGlade
    
    DECLARE_EVENT_TABLE()
    
public:
    void OnOpen(wxCommandEvent &event);
    void OnExit(wxCommandEvent &event);
    void OnAbout(wxCommandEvent &event);
    void OnOcclusions(wxCommandEvent &event);
    void OnStabilize(wxCommandEvent &event);
    void OnCLGOpticFlow(wxCommandEvent &event);
    void OnDataSourceChange(wxCommandEvent &event);
    void OnAddDataSource(wxCommandEvent &event); // wxGlade: <event_handler>
    void OnRemoveDataSource(wxCommandEvent &event); // wxGlade: <event_handler>
    void OnEditDataSource(wxCommandEvent &event); // wxGlade: <event_handler>
    void OnImageIndexScroll(wxScrollEvent &event); // wxGlade: <event_handler>
}; // wxGlade: end class


#endif // IMAGETRACKER_H
