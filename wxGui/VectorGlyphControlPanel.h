// -*- C++ -*- generated by wxGlade 0.4.1 on Mon Aug 20 16:08:08 2007

#include <wx/wx.h>
#include <wx/image.h>

#ifndef VECTORGLYPHCONTROLPANEL_H
#define VECTORGLYPHCONTROLPANEL_H

#include "wxDoubleSlider.h"
#include "wxIntSlider.h"
#include "VectorGlyphPipeline.h"

// begin wxGlade: ::dependencies
// end wxGlade


class VectorGlyphControlPanel: public wxPanel {
public:
    enum ids
    {
        SLD_SCALE_FACTOR = 6000,
        SLD_MASK_RATIO,
        CBX_VISIBILITY
    };
    
    // begin wxGlade: VectorGlyphControlPanel::ids
    // end wxGlade

    VectorGlyphControlPanel(wxWindow* parent, int id, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=0);
    
    /**
     * Get/Set the visualization pipeline.
     */
    VectorGlyphPipeline::Pointer GetPipeline();
    void SetPipeline(VectorGlyphPipeline::Pointer pipeline);

private:
    // begin wxGlade: VectorGlyphControlPanel::methods
    void set_properties();
    void do_layout();
    // end wxGlade

protected:
    // begin wxGlade: VectorGlyphControlPanel::attributes
    wxStaticBox* sizer_40_staticbox;
    wxCheckBox* checkVisibility;
    wxStaticText* label_33;
    wxDoubleSlider* slideGlyphScaleFactor;
    wxStaticText* label_34;
    wxIntSlider* slideGlyphMaskRatio;
    // end wxGlade

    DECLARE_EVENT_TABLE();

public:
    void OnGlyphScaleFactor(wxCommandEvent &event);
    void OnGlyphMaskRatio(wxCommandEvent &event);
    void OnVisibility(wxCommandEvent &event); // wxGlade: <event_handler>
    
    virtual bool TransferDataToWindow();
    
private:
    VectorGlyphPipeline::Pointer pipeline;
}; // wxGlade: end class


#endif // VECTORGLYPHCONTROLPANEL_H