/////////////////////////////////////////////////////////////////////////////
// Name:        wxIntSlider.h
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#ifndef __wxIntSlider_H__
#define __wxIntSlider_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "wxIntSlider.cxx"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wxCore_wdr.h"
#include <string>

// WDR: class declarations

//----------------------------------------------------------------------------
// wxIntSlider
//----------------------------------------------------------------------------

/**
 * An integer slider control widget.  This control includes a text box 
 * that allows a user to enter a numeric value directly.  The slider
 * control has a range, but values can be set arbitrarily using the
 * text control.
 */
class wxIntSlider: public wxPanel
{
public:
    // constructors and destructors
    wxIntSlider( wxWindow *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER );
    virtual ~wxIntSlider();
    
	// Set the range for this IntSlider.
	void SetRange(int min, int max);
	// Set the value held by this IntSlider.
	void SetValue(int value);
	// Retrieve the value held by this IntSlider.
	int GetValue();

protected:
    // WDR: method declarations for wxIntSlider
    wxStaticText* GetLabelRight()  { return (wxStaticText*) FindWindow( IS_LABEL_RIGHT ); }
    wxStaticText* GetLabelLeft()  { return (wxStaticText*) FindWindow( IS_LABEL_LEFT ); }
    wxTextCtrl* GetTextControl()  { return (wxTextCtrl*) FindWindow( IS_TEXTCTRL ); }
    wxSlider* GetSlider()  { return (wxSlider*) FindWindow( IS_SLIDER ); }
	void SetFormat(const std::string& format);
    
private:
    // WDR: member variable declarations for wxIntSlider
	int minimum;
	int maximum;
	int value;
	wxString format;
	bool ignoreNext;
	bool isLive;
    
private:
    // WDR: handler declarations for wxIntSlider
    void OnText( wxCommandEvent &event );
    void OnSlider( wxCommandEvent &event );
	// Generate an event indicating this DoubleSlider has changed value.
	void GenerateEvent();

private:
    DECLARE_EVENT_TABLE()
};

#endif
