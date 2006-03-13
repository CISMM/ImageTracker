/////////////////////////////////////////////////////////////////////////////
// Name:        wxDoubleSlider.h
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#ifndef __wxDoubleSlider_H__
#define __wxDoubleSlider_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "wxDoubleSlider.cxx"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wxCore_wdr.h"
#include <string>

// WDR: class declarations

//----------------------------------------------------------------------------
// wxDoubleSlider
//----------------------------------------------------------------------------

/**
 * A floating point slider control widget.  This control includes a text box
 * control that allows a user to enter a numeric value directly.  The slider
 * control has a defined range and precision, but values can be set
 * arbitrarily using the text control.
 */
class wxDoubleSlider: public wxPanel
{
public:
    // constructors and destructors
    wxDoubleSlider( wxWindow *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER );
    virtual ~wxDoubleSlider();

    // Set the range and precision for this DoubleSlider.
    void SetRange(double min, double max, double interval = 0.1);
    // Set the value held by this FloatSlider.
    void SetValue(double value);
    // Retrieve the value held by this DoubleSlider.
    double GetValue();
	// Retrieve the value held by this DoubleSlider as an int.
	int GetValueAsInt();
    // Set the format used to print numeric labels.
    void SetFormat(const std::string& format);
    
protected:
    // WDR: method declarations for wxDoubleSlider
    wxStaticText* GetLabelRight()  { return (wxStaticText*) FindWindow( FS_LABEL_RIGHT ); }
    wxStaticText* GetLabelLeft()  { return (wxStaticText*) FindWindow( FS_LABEL_LEFT ); }
    wxTextCtrl* GetTextControl()  { return (wxTextCtrl*) FindWindow( FS_TEXTCTRL ); }
    wxSlider* GetSlider()  { return (wxSlider*) FindWindow( FS_SLIDER ); }
    
private:
    // WDR: member variable declarations for wxDoubleSlider
    double minimum;
    double maximum;
    double interval;    // amount a tick on the slider represents
    double value;       // current value
    wxString format;    // format to use for string representation
	bool ignoreNext;	// indicates that the next slider event should be ignored
	bool isLive;		// indicates that this slider is live...
    
private:
    // WDR: handler declarations for wxDoubleSlider
    void OnText( wxCommandEvent &event );
    void OnSlider( wxCommandEvent &event );
    int DoubleToSlider();
    double SliderToDouble();
	// Generate an event indicating this DoubleSlider has changed its value.
	void GenerateEvent();

private:
    DECLARE_EVENT_TABLE()
};

#endif
