// -*- C++ -*- generated by wxGlade 0.4.1 on Mon Oct  1 12:40:16 2007

#include "wxIntSlider.h"

#include "wxUtils.h"

wxIntSlider::wxIntSlider(wxWindow* parent, int id, const wxPoint& pos, const wxSize& size, long style):
    wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL)
{
    // begin wxGlade: wxIntSlider::wxIntSlider
    sldSlider = new wxSlider(this, -1, 0, 0, 10);
    textValue = new wxTextCtrl(this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    labelMinimum = new wxStaticText(this, -1, wxT("0"));
    labelMaximum = new wxStaticText(this, -1, wxT("0"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);

    set_properties();
    do_layout();
    // end wxGlade
    
    // defaults
    this->enforceRange = false;
    this->SetRange(0,10);
    this->SetValue(0);
    this->SetFormat("%d");
}

wxIntSlider::~wxIntSlider()
{}

BEGIN_EVENT_TABLE(wxIntSlider, wxPanel)
    // begin wxGlade: wxIntSlider::event_table
    EVT_COMMAND_SCROLL(-1, wxIntSlider::OnSlider)
    EVT_TEXT_ENTER(-1, wxIntSlider::OnText)
    // end wxGlade
END_EVENT_TABLE();


void wxIntSlider::OnSlider(wxScrollEvent &event)
{
    this->value = this->sldSlider->GetValue();
    this->textValue->SetValue(wxString::Format(this->format, this->value));
    this->GenerateEvent();
}


void wxIntSlider::OnText(wxCommandEvent &event)
{
    // convert text entered to an int
    long val;
    bool isLong = this->textValue->GetValue().ToLong(&val);
    if (isLong && val != this->value && this->CheckRange(val))
    {
        this->value = (int) val;
        this->sldSlider->SetValue(this->value);
        this->GenerateEvent();
    }
    else // invalid stuff entered, e.g. text
    {
        this->textValue->SetValue(wxString::Format(this->format, this->value));
    }
}


// wxGlade: add wxIntSlider event handlers

void wxIntSlider::GenerateEvent()
{
    if (this->GetParent())
    {
        wxCommandEvent evt(wxEVT_COMMAND_SLIDER_UPDATED, this->GetId());
        evt.SetEventObject(this);
        evt.SetString(this->textValue->GetValue());
        this->GetParent()->GetEventHandler()->ProcessEvent(evt);
    }
}

bool wxIntSlider::CheckRange(int val)
{
    return (!this->enforceRange) || (val >= this->minimum && val <= this->maximum);
}

bool wxIntSlider::GetEnforceRange()
{
    return this->enforceRange;
}

void wxIntSlider::SetEnforceRange(bool enforce)
{
    this->enforceRange = enforce;
}

int wxIntSlider::GetValue()
{
    return this->value;
}

int wxIntSlider::GetMin()
{
    return this->minimum;
}

int wxIntSlider::GetMax()
{
    return this->maximum;
}

void wxIntSlider::SetValue(int value)
{
    if (this->CheckRange(value))
    {
        this->value = value;
        this->textValue->SetValue(wxString::Format(this->format, this->value));
        this->sldSlider->SetValue(this->value);
    }
}

void wxIntSlider::SetRange(int min, int max)
{
    this->minimum = min;
    this->maximum = max;
    
    this->sldSlider->SetRange(this->minimum, this->maximum);
    this->labelMinimum->SetLabel(wxString::Format(this->format, this->minimum));
    this->labelMaximum->SetLabel(wxString::Format(this->format, this->maximum));
    
    // Keep slider position in sync with value
    this->sldSlider->SetValue(this->value);
}

void wxIntSlider::SetFormat(const std::string& format)
{
    this->format = std2wx(format);
    this->labelMinimum->SetLabel(wxString::Format(this->format, this->minimum));
    this->labelMaximum->SetLabel(wxString::Format(this->format, this->maximum));
    this->textValue->SetValue(wxString::Format(this->format, this->value));
}

void wxIntSlider::set_properties()
{
    // begin wxGlade: wxIntSlider::set_properties
    // end wxGlade
}


void wxIntSlider::do_layout()
{
    // begin wxGlade: wxIntSlider::do_layout
    wxFlexGridSizer* grid_sizer_17 = new wxFlexGridSizer(2, 2, 5, 5);
    wxBoxSizer* sizer_46 = new wxBoxSizer(wxHORIZONTAL);
    grid_sizer_17->Add(sldSlider, 1, wxEXPAND|wxADJUST_MINSIZE, 0);
    grid_sizer_17->Add(textValue, 0, wxADJUST_MINSIZE, 0);
    sizer_46->Add(labelMinimum, 1, wxEXPAND|wxADJUST_MINSIZE, 0);
    sizer_46->Add(labelMaximum, 0, wxEXPAND|wxADJUST_MINSIZE, 0);
    grid_sizer_17->Add(sizer_46, 1, wxEXPAND, 0);
    grid_sizer_17->Add(20, 20, 0, wxADJUST_MINSIZE, 0);
    SetAutoLayout(true);
    SetSizer(grid_sizer_17);
    grid_sizer_17->Fit(this);
    grid_sizer_17->SetSizeHints(this);
    grid_sizer_17->AddGrowableCol(0);
    // end wxGlade
}
