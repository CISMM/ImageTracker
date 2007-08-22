// -*- C++ -*- generated by wxGlade 0.4.1 on Mon Aug 20 16:08:08 2007

#include "VectorGlyphControlPanel.h"
#include "Logger.h"

VectorGlyphPipeline::Pointer VectorGlyphControlPanel::GetPipeline()
{
    return this->pipeline;
}

void VectorGlyphControlPanel::SetPipeline(VectorGlyphPipeline::Pointer pipeline)
{
    this->pipeline = pipeline;
    this->TransferDataToWindow();
}

VectorGlyphControlPanel::VectorGlyphControlPanel(wxWindow* parent, int id, const wxPoint& pos, const wxSize& size, long style):
    wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL)
{
    // begin wxGlade: VectorGlyphControlPanel::VectorGlyphControlPanel
    sizer_40_staticbox = new wxStaticBox(this, -1, wxT("Vector Glyphs"));
    checkVisibility = new wxCheckBox(this, CBX_VISIBILITY, wxT("Visibility"));
    label_33 = new wxStaticText(this, -1, wxT("Scale Factor"));
    slideGlyphScaleFactor = new wxDoubleSlider(this, SLD_SCALE_FACTOR);
    label_34 = new wxStaticText(this, -1, wxT("Mask Ratio"));
    slideGlyphMaskRatio = new wxIntSlider(this, SLD_MASK_RATIO);

    set_properties();
    do_layout();
    // end wxGlade
    
    // Setup sliders
    this->slideGlyphScaleFactor->SetRange(1.0, 500.0, 1.0);
    this->slideGlyphScaleFactor->SetValue(1.0);
    this->slideGlyphMaskRatio->SetRange(50, 1000);
    this->slideGlyphMaskRatio->SetValue(100);
}


BEGIN_EVENT_TABLE(VectorGlyphControlPanel, wxPanel)
    EVT_SLIDER(SLD_SCALE_FACTOR, VectorGlyphControlPanel::OnGlyphScaleFactor)
    EVT_SLIDER(SLD_MASK_RATIO, VectorGlyphControlPanel::OnGlyphMaskRatio)
    // begin wxGlade: VectorGlyphControlPanel::event_table
    EVT_CHECKBOX(CBX_VISIBILITY, VectorGlyphControlPanel::OnVisibility)
    // end wxGlade
END_EVENT_TABLE();

bool VectorGlyphControlPanel::TransferDataToWindow()
{
    Logger::debug << "VectorGlyphControlPanel::TransferDataToWindow" << std::endl;
    if (this->pipeline && this->pipeline.IsNotNull())
    {
        this->checkVisibility->SetValue(this->pipeline->GetVisibility());
        this->slideGlyphScaleFactor->SetValue(this->pipeline->GetScaleFactor());
        this->slideGlyphMaskRatio->SetValue(this->pipeline->GetMaskRatio());
    }
}

void VectorGlyphControlPanel::OnVisibility(wxCommandEvent &event)
{
    Logger::debug << "VectorGlyphControlPanel::OnVisibility" << std::endl;
    if (this->pipeline && this->pipeline.IsNotNull())
        this->pipeline->SetVisibility(this->checkVisibility->IsChecked());
}

void VectorGlyphControlPanel::OnGlyphScaleFactor(wxCommandEvent &event)
{
    Logger::debug << "VectorGlyphControlPanel::OnGlyphScaleFactor" << std::endl;
    if (this->pipeline && this->pipeline.IsNotNull())
        this->pipeline->SetScaleFactor(this->slideGlyphScaleFactor->GetValue());
}

void VectorGlyphControlPanel::OnGlyphMaskRatio(wxCommandEvent &event)
{
    Logger::debug << "VectorGlyphControlPanel::OnGlyphMaskRatio" << std::endl;
    if (this->pipeline && this->pipeline.IsNotNull())
        this->pipeline->SetMaskRatio(this->slideGlyphMaskRatio->GetValue());
}

// wxGlade: add VectorGlyphControlPanel event handlers


void VectorGlyphControlPanel::set_properties()
{
    // begin wxGlade: VectorGlyphControlPanel::set_properties
    checkVisibility->SetToolTip(wxT("Turn on/off visibility of this visualization"));
    checkVisibility->SetValue(1);
    slideGlyphScaleFactor->SetToolTip(wxT("Scaling to apply to the vector glyphs."));
    slideGlyphMaskRatio->SetToolTip(wxT("Spacing between visible glyphs (larger number means fewer glyphs)"));
    // end wxGlade
}


void VectorGlyphControlPanel::do_layout()
{
    // begin wxGlade: VectorGlyphControlPanel::do_layout
    wxBoxSizer* sizer_38 = new wxBoxSizer(wxVERTICAL);
    wxStaticBoxSizer* sizer_40 = new wxStaticBoxSizer(sizer_40_staticbox, wxVERTICAL);
    wxFlexGridSizer* grid_sizer_13 = new wxFlexGridSizer(3, 2, 5, 5);
    grid_sizer_13->Add(20, 20, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_13->Add(checkVisibility, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_13->Add(label_33, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_13->Add(slideGlyphScaleFactor, 1, wxEXPAND, 0);
    grid_sizer_13->Add(label_34, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_13->Add(slideGlyphMaskRatio, 1, wxEXPAND, 0);
    grid_sizer_13->AddGrowableCol(1);
    sizer_40->Add(grid_sizer_13, 1, wxEXPAND, 0);
    sizer_38->Add(sizer_40, 1, wxEXPAND, 0);
    SetAutoLayout(true);
    SetSizer(sizer_38);
    sizer_38->Fit(this);
    sizer_38->SetSizeHints(this);
    // end wxGlade
}

