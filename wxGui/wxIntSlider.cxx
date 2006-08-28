/////////////////////////////////////////////////////////////////////////////
// Name:        wxIntSlider.cxx
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "wxIntSlider.cxx"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "Logger.h"
#include "wxUtils.h"
#include "wxIntSlider.h"


// WDR: class implementations

//----------------------------------------------------------------------------
// wxIntSlider
//----------------------------------------------------------------------------

// WDR: event table for wxIntSlider

BEGIN_EVENT_TABLE(wxIntSlider,wxPanel)
    EVT_SLIDER( IS_SLIDER, wxIntSlider::OnSlider )
    EVT_TEXT_ENTER( IS_TEXTCTRL, wxIntSlider::OnText )
END_EVENT_TABLE()

wxIntSlider::wxIntSlider( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxPanel( parent, id, position, size, style )
{
    // WDR: dialog function CreateIntSlider for wxIntSlider
    CreateIntSlider( this, TRUE );

	// Select defaults
	this->isLive = false; // disable events during init
	this->ignoreNext = false;
	this->SetFormat("%d");
	this->SetRange(0, 100);
	this->SetValue(0);
	this->isLive = true;
}

wxIntSlider::~wxIntSlider()
{
}

void wxIntSlider::SetRange(int min, int max)
{
	this->minimum = min;
	this->maximum = max;

	this->GetSlider()->SetRange(min, max);
	this->GetLabelLeft()->SetLabel(wxString::Format(this->format, this->minimum));
	this->GetLabelRight()->SetLabel(wxString::Format(this->format, this->maximum));
}

void wxIntSlider::SetValue(int value)
{
	Logger::verbose << "wxIntSlider(" << this->GetId() << ")::SetValue: " << value << std::endl;
	if (this->value != value) // value has changed
	{
		this->value = value;

		this->GetTextControl()->SetValue(wxString::Format(this->format, this->value));
		if (this->minimum <= this->value &&
			this->maximum >= this->value)
		{
			// ignore the wxSlider event that gets generated when we set the slider value
			this->ignoreNext = true;
			this->GetSlider()->SetValue(this->value);
		}

		// Alert listeners
		this->GenerateEvent();
	}
}

int wxIntSlider::GetValue()
{
	return this->value;
}

void wxIntSlider::SetFormat(const std::string& format)
{
	this->format = std2wx(format);

	// change labels
	this->GetLabelLeft()->SetLabel(wxString::Format(this->format, this->minimum));
	this->GetLabelRight()->SetLabel(wxString::Format(this->format, this->maximum));
	this->GetTextControl()->SetValue(wxString::Format(this->format, this->value));
}

void wxIntSlider::GenerateEvent()
{
	if (this->isLive && this->GetParent())
	{
		wxCommandEvent evt(wxEVT_COMMAND_SLIDER_UPDATED, this->GetId());
		evt.SetEventObject(this);
		evt.SetString(this->GetTextControl()->GetValue());
		this->GetParent()->GetEventHandler()->ProcessEvent(evt);
	}
}

// WDR: handler implementations for wxIntSlider

void wxIntSlider::OnText( wxCommandEvent &event )
{
    Logger::verbose << "wxIntSlider(" << this->GetId() << ")::OnText: " << this->GetTextControl()->GetValue().c_str() << std::endl;

	// Convert text entered to an int
	long val;
	bool isLong = this->GetTextControl()->GetValue().ToLong(&val);
	if (isLong && val != this->value)
	{
		this->value = (int) val;
		this->ignoreNext = true;
		this->GetSlider()->SetValue(this->value);
		this->GenerateEvent();
	}
	else // user has entered invalid text
	{
		// reset
		this->GetTextControl()->SetValue(wxString::Format(this->format, this->value));
	}
}

void wxIntSlider::OnSlider( wxCommandEvent &event )
{
    Logger::verbose << "wxIntSlider(" << this->GetId() << ")::OnSlider: " 
		<< this->GetSlider()->GetValue() 
		<< "\tIgnore => " << this->ignoreNext << std::endl;

	// find the value of the slider
	int newVal = this->GetSlider()->GetValue();
	if (!this->ignoreNext && newVal != this->value)
	{
		this->value = newVal;
		this->GetTextControl()->SetValue(wxString::Format(this->format, this->value));
		this->GenerateEvent();
	}
	this->ignoreNext = false;
}
