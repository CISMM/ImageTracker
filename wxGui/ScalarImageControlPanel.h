// -*- C++ -*- generated by wxGlade 0.4.1 on Thu Sep 13 13:25:54 2007

#include <wx/wx.h>
#include <wx/image.h>

#ifndef SCALARIMAGECONTROLPANEL_H
#define SCALARIMAGECONTROLPANEL_H

#include "wxDoubleSlider.h"
#include "ScalarImageItkVtkPipeline.h"

// begin wxGlade: ::dependencies
// end wxGlade


class ScalarImageControlPanel: public wxPanel {
public:
    enum ids
    {
        SLD_WINDOW_MIN = 6100,
        SLD_WINDOW_MAX,
        CMB_WINDOW_RANGE,
        CBX_VISIBILITY
    };
    
    // begin wxGlade: ScalarImageControlPanel::ids
    // end wxGlade

    ScalarImageControlPanel(wxWindow* parent, int id, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=0);

    /**
     * Get/Set the visualization pipeline.
     */
    ScalarImageItkVtkPipeline::Pointer GetPipeline();
    void SetPipeline(ScalarImageItkVtkPipeline::Pointer pipeline);
    
private:
    // begin wxGlade: ScalarImageControlPanel::methods
    void set_properties();
    void do_layout();
    // end wxGlade

protected:
    // begin wxGlade: ScalarImageControlPanel::attributes
    wxStaticBox* sizer_14_staticbox;
    wxStaticBox* sizer_42_staticbox;
    wxCheckBox* checkVisibility;
    wxStaticText* label_36;
    wxDoubleSlider* slideWindowMax;
    wxStaticText* label_37;
    wxDoubleSlider* slideWindowMin;
    wxStaticText* label_38;
    wxComboBox* comboWindowRange;
    // end wxGlade

    DECLARE_EVENT_TABLE();

public:
    void OnWindowMax(wxCommandEvent &event);
    void OnWindowMin(wxCommandEvent &event);
    void OnVisibility(wxCommandEvent &event); // wxGlade: <event_handler>
    void OnRange(wxCommandEvent &event); // wxGlade: <event_handler>
    
    virtual bool TransferDataToWindow();
    
private:
    ScalarImageItkVtkPipeline::Pointer pipeline;
}; // wxGlade: end class


#endif // SCALARIMAGECONTROLPANEL_H