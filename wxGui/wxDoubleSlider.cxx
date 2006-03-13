/////////////////////////////////////////////////////////////////////////////
// Name:        wxDoubleSlider.cxx
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "wxDoubleSlider.cxx"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wxDoubleSlider.h"
#include "Logger.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// wxDoubleSlider
//----------------------------------------------------------------------------

// WDR: event table for wxDoubleSlider

BEGIN_EVENT_TABLE(wxDoubleSlider,wxPanel)
    EVT_SLIDER( FS_SLIDER, wxDoubleSlider::OnSlider )
    EVT_TEXT_ENTER( FS_TEXTCTRL, wxDoubleSlider::OnText )
END_EVENT_TABLE()

wxDoubleSlider::wxDoubleSlider( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxPanel( parent, id, position, size, style )
{
    // WDR: dialog function CreateFloatSlider for wxDoubleSlider
    CreateDoubleSlider( this, TRUE );

    // Select some defaults
	this->isLive = false; // disable events during init
	this->ignoreNext = false;
    this->SetFormat("%.1f");
    this->SetRange(0.0, 1.0, 0.1);
    this->SetValue(0.0);
	this->isLive = true;
}

wxDoubleSlider::~wxDoubleSlider()
{
}

void wxDoubleSlider::SetRange(double min, double max, double interval)
{
    this->minimum = min;
    this->maximum = max;
    this->interval = interval;

    // Compute number of ticks to use for an integer-based slider
    int ticks = (this->maximum - this->minimum) / this->interval;
    this->GetSlider()->SetRange(0, ticks);

    // Create min and max labels
    this->GetLabelLeft()->SetLabel(wxString::Format(this->format, this->minimum));
    this->GetLabelRight()->SetLabel(wxString::Format(this->format, this->maximum));
}

void wxDoubleSlider::SetValue(double value)
{
    Logger::verbose << "wxDoubleSlider(" << this->GetId() << ")::SetValue: " << value << std::endl;
    if (this->value != value) // value has changed
    {
		this->value = value;

        // Set the contents of the text box
        this->GetTextControl()->SetValue(wxString::Format(this->format, this->value));

        // Update the slider position, if value is within range
        if (this->minimum <= this->value && 
            this->maximum >= this->value)
        {
			// ignore the wxSlider event that gets generated when we set the slider value
			this->ignoreNext = true;
            this->GetSlider()->SetValue(this->DoubleToSlider());
        }
		
		// Alert listeners
		this->GenerateEvent();
    }
}

double wxDoubleSlider::GetValue()
{
    return this->value;
}

int wxDoubleSlider::GetValueAsInt()
{
	return (int) this->value;
}

void wxDoubleSlider::SetFormat(const std::string& format)
{
    this->format = format.c_str();

    // Create min and max labels and value
    this->GetLabelLeft()->SetLabel(wxString::Format(this->format, this->minimum));
    this->GetLabelRight()->SetLabel(wxString::Format(this->format, this->maximum));
    this->GetTextControl()->SetValue(wxString::Format(this->format, this->value));
}

int wxDoubleSlider::DoubleToSlider()
{
    return this->GetSlider()->GetMin() + 
        static_cast<int>((this->value - this->minimum) / this->interval);
}

double wxDoubleSlider::SliderToDouble()
{
    return this->minimum + this->interval * this->GetSlider()->GetValue();
}

void wxDoubleSlider::GenerateEvent()
{
	// We want the window that contains us to handle this event.  We
	// do not want our own slider to see this event.
	if (this->isLive && this->GetParent())
	{
		wxCommandEvent evt(wxEVT_COMMAND_SLIDER_UPDATED, this->GetId());
		evt.SetEventObject(this);
		evt.SetString(this->GetTextControl()->GetValue());
		// this->GetEventHandler()->ProcessEvent(evt);
		this->GetParent()->GetEventHandler()->ProcessEvent(evt);
	}
}

// WDR: handler implementations for wxDoubleSlider

void wxDoubleSlider::OnText( wxCommandEvent &event )
{
    Logger::verbose << "wxDoubleSlider(" << this->GetId() << ")::OnText: " << this->GetTextControl()->GetValue().c_str() << std::endl;
    
    // Convert text entered to a double (then cast to float)
    double val;
    bool isDouble = this->GetTextControl()->GetValue().ToDouble(&val);
	if (isDouble && val != this->value)
    {
		this->value = val;
		this->ignoreNext = true;
		this->GetSlider()->SetValue(this->DoubleToSlider());
		this->GenerateEvent();
    }
    else // user has entered invalid text
    {
        // reset
        this->GetTextControl()->SetValue(wxString::Format(this->format, this->value));
    }
}

void wxDoubleSlider::OnSlider( wxCommandEvent &event )
{
    Logger::verbose << "wxDoubleSlider(" << this->GetId() << ")::OnSlider: " 
		<< this->GetSlider()->GetValue() 
		<< "\tIgnore => " << this->ignoreNext << std::endl;
    
    // Find the float value that the slider position corresponds to
	double newVal = this->SliderToDouble();
	if (!this->ignoreNext && newVal != this->value)
	{
		this->value = newVal;
		this->GetTextControl()->SetValue(wxString::Format(this->format, this->value));
		this->GenerateEvent();
	}
	this->ignoreNext = false;
}

