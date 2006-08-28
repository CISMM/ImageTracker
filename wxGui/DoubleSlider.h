#pragma once
#include <wx/wx.h>
/*
 * A simple extension to wxSlider that allows for double values.  Note that
 * since this is just an extension, all of the underlying wxSlider methods are
 * available.  But, the range and value getters and setters from that class won't 
 * perform as expected...because the wxSlider only deals with integer values.
 */
class DoubleSlider : public wxSlider
{
public:
    /*
     * Constructor; resembles that of wxSlider.
     */
    DoubleSlider(wxWindow* parent, wxWindowID id, int value, int minValue, int maxValue, const wxPoint& point=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=wxSL_HORIZONTAL, const wxValidator& validator=wxDefaultValidator, const wxString& name=wxT("slider"));
    
    /*
     * Destructor.
     */
    ~DoubleSlider(void);

    /*
     * Set the double precision range of this slider.
     */
    void SetRange(double min, double max, double interval);

    /*
     * Set the value for this slider to display.
     */
    void SetValue(double value);

    /*
     * Get the double precision value associted with this slider.
     */
    double GetDoubleValue();

private:
    // WDR: member variable declarations for SliderControl
    double min;
    double max;
    double interval;
};
