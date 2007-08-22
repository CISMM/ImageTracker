// -*- C++ -*- generated by wxGlade 0.4.1 on Tue Feb  6 01:59:37 2007

#include <wx/wx.h>
#include <wx/image.h>

#include "CLGOpticFlowPipeline.h"
#include "DataSource.h"
#include "ScalarImageItkVtkPipeline.h"
#include "wxDoubleSlider.h"
#include "wxIntSlider.h"
        
#ifndef CLGOPTICFLOWDIALOG_H
#define CLGOPTICFLOWDIALOG_H

// begin wxGlade: ::dependencies
// end wxGlade


class CLGOpticFlowDialog: public wxDialog {
public:
    // begin wxGlade: CLGOpticFlowDialog::ids
    // end wxGlade
    
    enum IDs
    {
        BTN_BROWSE = 3050,
        SLD_SMOOTHING
    };

    CLGOpticFlowDialog(wxWindow* parent, int id, const wxString& title, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=wxDEFAULT_DIALOG_STYLE);

private:
    // begin wxGlade: CLGOpticFlowDialog::methods
    void set_properties();
    void do_layout();
    // end wxGlade

protected:
    // begin wxGlade: CLGOpticFlowDialog::attributes
    wxStaticBox* sizer_26_staticbox;
    wxStaticBox* sizer_27_staticbox;
    wxStaticText* label_15;
    wxDoubleSlider* slideSmoothing;
    wxStaticText* label_16;
    wxDoubleSlider* slideRegularization;
    wxStaticText* label_19;
    wxDoubleSlider* slideRelaxation;
    wxStaticText* label_20;
    wxIntSlider* slideIterations;
    wxStaticText* label_13;
    wxTextCtrl* textDirectory;
    wxButton* btnBrowse;
    wxStaticText* label_14;
    wxTextCtrl* textPrefix;
    wxCheckBox* checkOpenOutput;
    wxButton* btnRun;
    wxButton* btnHide;
    // end wxGlade

    DECLARE_EVENT_TABLE();

public:
    void OnSlideSmoothing(wxCommandEvent &event);
    void OnBrowse(wxCommandEvent &event); // wxGlade: <event_handler>
    void OnRun(wxCommandEvent &event); // wxGlade: <event_handler>
    void OnHide(wxCommandEvent &event); // wxGlade: <event_handler>
    
public:
    virtual bool TransferDataToWindow();
    virtual bool TransferDataFromWindow();
    void SetInput(DataSource::Pointer input);

protected:
    void ViewPreview(bool show);
    void UpdatePreview();
    
private:
    DataSource::Pointer input;
    CLGOpticFlowPipeline::Pointer pipeline;
    ScalarImageItkVtkPipeline::Pointer visual;
    bool preview;

}; // wxGlade: end class


#endif // CLGOPTICFLOWDIALOG_H
