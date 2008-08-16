// -*- C++ -*- generated by wxGlade 0.6.3 on Tue Jul 29 11:45:14 2008

#include <wx/wx.h>
#include <wx/image.h>

#ifndef MULTIRESOLUTIONREGISTRATIONDIALOG_H
#define MULTIRESOLUTIONREGISTRATIONDIALOG_H

// begin wxGlade: ::dependencies
// end wxGlade

// begin wxGlade: ::extracode
#include "FilePatternPanel.h"
#include "wxDoubleSlider.h"
#include "wxIntSlider.h"

// end wxGlade

#include "ImageFileSet.h"
#include "MultiResolutionRegistrationPipeline.h"
#include "ScalarImageVisualization.h"

class MultiResolutionRegistrationDialog: public wxDialog {
public:
    // begin wxGlade: MultiResolutionRegistrationDialog::ids
    enum {
        SLIDE_MAX_SMOOTH = wxID_HIGHEST + 1088,
        SLIDE_MIN_SMOOTH = wxID_HIGHEST + 1089
    };
    // end wxGlade

    MultiResolutionRegistrationDialog(wxWindow* parent, int id, const wxString& title, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=wxDEFAULT_DIALOG_STYLE);

    
    virtual bool TransferDataToWindow();
    virtual bool TransferDataFromWindow();
        
    void SetInput(ImageFileSet* input);
    
private:
    // begin wxGlade: MultiResolutionRegistrationDialog::methods
    void set_properties();
    void do_layout();
    // end wxGlade

    bool preview;
    ImageFileSet* input;
    MultiResolutionRegistrationPipeline::Pointer pipeline;
    ScalarImageVisualization::Pointer visual;
    
protected:
    // begin wxGlade: MultiResolutionRegistrationDialog::attributes
    wxStaticBox* sizer_17_staticbox;
    wxStaticBox* sizer_23_staticbox;
    wxStaticBox* sizer_20_staticbox;
    wxStaticText* label_8;
    wxIntSlider* slideMaxSmooth;
    wxStaticText* label_9;
    wxIntSlider* slideMinSmooth;
    wxStaticText* label_21;
    wxIntSlider* slideIterations;
    wxStaticText* label_22;
    wxDoubleSlider* slideMaxStepLength;
    wxStaticText* label_23;
    wxDoubleSlider* slideMinStepLength;
    wxStaticText* label_24;
    wxDoubleSlider* slideStepScale;
    FilePatternPanel* panelFilePattern;
    wxStaticText* label_6;
    wxTextCtrl* textTransform;
    wxCheckBox* checkOpenOutput;
    wxButton* buttonRun;
    wxButton* buttonHide;
    // end wxGlade

    DECLARE_EVENT_TABLE();
    
    void ViewPreview(bool show);
    void UpdatePreview();

public:
    virtual void OnMaxSmooth(wxCommandEvent &event);
    virtual void OnMinSmooth(wxCommandEvent &event);
    virtual void OnRun(wxCommandEvent &event); // wxGlade: <event_handler>
    virtual void OnHide(wxCommandEvent &event); // wxGlade: <event_handler>
}; // wxGlade: end class


#endif // MULTIRESOLUTIONREGISTRATIONDIALOG_H
