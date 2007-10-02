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
#include "ApplyTransformDialog.h"
#include "CLGOpticFlowDialog.h"
#include "DataSourceDialog.h"
#include "HornOpticalFlowDialog.h"
#include "ImageTrackerController.h"
#include "IntegrateFlowDialog.h"
#include "MultiResolutionRegistrationDialog.h"
#include "RemoveOcclusionsDialog.h"
#include "SaveVisualizationDialog.h"
#include "wxIntSlider.h"
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

    virtual ~ImageTracker();
    
    enum IDs
    { 
        MENU_EXIT           = 1000,
        MENU_ABOUT,
        MENU_OPEN,
        MENU_SAVE_IMAGES,
        MENU_OCCLUSIONS,
        MENU_STABILIZE,
        MENU_APPLY_TRANSFORM,
        MENU_CLG_OPTIC_FLOW,
        MENU_HORN_OPTICAL_FLOW,
        MENU_INTEGRATE_FLOW,
        MENU_IMAGE_INFO,
        MENU_LOG_ERROR,
        MENU_LOG_WARN,
        MENU_LOG_INFO,
        MENU_LOG_DEBUG,
        MENU_LOG_VERBOSE,
        BTN_ADD_DATASOURCE  = 2000,
        BTN_REMOVE_DATASOURCE,
        BTN_FIRST,
        BTN_PREVIOUS,
        BTN_REWIND,
        BTN_PAUSE,
        BTN_PLAY,
        BTN_NEXT,
        BTN_LAST,
        LBX_DATASOURCES,
        SLD_IMAGE_INDEX
    };

    // Possible playing states (controls a state machine)
    enum PlayState
    {
        Pause = 0,
        Play,
        Rewind,
        GoToFrame,
        SkipFirst,
        SkipLast,
        AboutToRecord,
        Record
    };
    
    /**
     * Updates the list of DataSources displayed in the ImageTracker.
     */
    void UpdateDataSources();

    /**
     * Get/Set the play state of the ImageTracker data window.
     */
    PlayState GetPlayState() { return this->playState; }
    void SetPlayState(PlayState state) { this->playState = state; }
    
    // We override Destroy to make sure we clean up resources in the right order.
    virtual bool Destroy();
    
private:
    // begin wxGlade: ImageTracker::methods
    void set_properties();
    void do_layout();
    // end wxGlade
    
    DataSourceDialog* dlgDataSource;
    RemoveOcclusionsDialog* dlgRemoveOcclusions;
    MultiResolutionRegistrationDialog* dlgRegistration;
    ApplyTransformDialog* dlgApplyTransform;
    CLGOpticFlowDialog* dlgCLGOpticFlow;
    HornOpticalFlowDialog* dlgHornOpticalFlow;
    IntegrateFlowDialog* dlgIntegrateFlow;
    SaveVisualizationDialog* dlgSaveVisualization;
    AboutDialog* dlgAbout;
    wxStreamToTextRedirector* coutRedirect;
    PlayState playState;
    bool loopPlay;
    int targetFrame;
    
    std::string saveFormat;
    unsigned int saveFrom;
    unsigned int saveTo;
    
protected:
    // begin wxGlade: ImageTracker::attributes
    wxStaticBox* sizerStaticVisualization_staticbox;
    wxStaticBox* sizer_4_staticbox;
    wxMenuBar* itMenuBar;
    wxStatusBar* theStatusBar;
    wxListBox* lbxSources;
    wxButton* btnAddDataSource;
    wxButton* btnRemoveDataSource;
    wxPanel* panelLeftUpper;
    wxPanel* panelVisualization;
    wxPanel* panelLeftLower;
    wxSplitterWindow* hsplitSourceVisControl;
    wxPanel* panelLeft;
    wxVTKRenderWindowInteractor* rwiView;
    wxIntSlider* sldImageIndex;
    wxButton* btnFirst;
    wxButton* btnPrevious;
    wxButton* btnRewind;
    wxButton* btnPause;
    wxButton* btnPlay;
    wxButton* btnNext;
    wxButton* btnLast;
    wxPanel* panelRight;
    wxSplitterWindow* vsplitDataView;
    wxPanel* panelMainUpper;
    wxTextCtrl* txtLogger;
    wxPanel* panelMainLower;
    wxSplitterWindow* hsplitDataLogger;
    // end wxGlade
    
    DECLARE_EVENT_TABLE()
    
    /**
     * Updates the currently displayed data slice based on
     * the ImageTracker play state.
     */
    void UpdatePlayState();

public:
    void OnIdle(wxIdleEvent &event);
    void OnSaveViewImages(wxCommandEvent &event);
    void OnOpen(wxCommandEvent &event);
    void OnExit(wxCommandEvent &event);
    void OnAbout(wxCommandEvent &event);
    void OnOcclusions(wxCommandEvent &event);
    void OnStabilize(wxCommandEvent &event);
    void OnApplyTransform(wxCommandEvent &event);
    void OnCLGOpticFlow(wxCommandEvent &event);
    void OnHornOpticalFlow(wxCommandEvent &event);
    void OnIntegrateFlow(wxCommandEvent &event);
    void OnLoggingMenu(wxCommandEvent &event);
    void OnImageInfo(wxCommandEvent &event);
    void OnImageIndex(wxCommandEvent &event);
    void OnAddDataSource(wxCommandEvent &event); // wxGlade: <event_handler>
    void OnRemoveDataSource(wxCommandEvent &event); // wxGlade: <event_handler>
    void OnEditDataSource(wxCommandEvent &event); // wxGlade: <event_handler>
    void OnPlay(wxCommandEvent &event); // wxGlade: <event_handler>
    void OnFirstFrame(wxCommandEvent &event); // wxGlade: <event_handler>
    void OnRewind(wxCommandEvent &event); // wxGlade: <event_handler>
    void OnLastFrame(wxCommandEvent &event); // wxGlade: <event_handler>
    void OnPause(wxCommandEvent &event); // wxGlade: <event_handler>
    void OnSelectDataSource(wxCommandEvent &event); // wxGlade: <event_handler>
    void OnPrevious(wxCommandEvent &event); // wxGlade: <event_handler>
    void OnNext(wxCommandEvent &event); // wxGlade: <event_handler>
}; // wxGlade: end class


#endif // IMAGETRACKER_H
