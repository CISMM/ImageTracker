#include "DoubleSlider.h"
#include "wx/msw/subwin.h"

// Copied from slider95.cpp
// indices of labels in wxSlider::m_labels
enum
{
    SliderLabel_Min,
    SliderLabel_Max,
    SliderLabel_Value,
    SliderLabel_Last
};

DoubleSlider::DoubleSlider(wxWindow* parent, wxWindowID id, 
                           int value, int minValue, int maxValue, 
                           const wxPoint& point, const wxSize& size, 
                           long style, const wxValidator& validator, 
                           const wxString& name)
                           : wxSlider(parent, id, value, minValue, maxValue, point, size, style, validator, name)
{
    this->max = minValue;
    this->max = maxValue;
    this->interval = 1.0;
}

DoubleSlider::~DoubleSlider(void)
{
}

void DoubleSlider::SetRange(double min, double max, double interval)
{
    if (min < max && interval > 0.0)
    {
        this->min = min;
        this->max = max;
        this->interval = interval;

        int count = (int) (max - min) / interval + 1;
        ((wxSlider *) this)->SetRange(0, count);
    
		if ( wxSlider::m_labels )
		{
			SetWindowText((*m_labels)[SliderLabel_Min], wxString::Format("%4.2f", min));
			SetWindowText((*m_labels)[SliderLabel_Max], wxString::Format("%4.2f", max));
		}
    }
}

double DoubleSlider::GetDoubleValue()
{
    double intVal = (double) this->GetValue();
    return (this->min + this->interval * intVal);
}

void DoubleSlider::SetValue(double value)
{
    if (value >= this->min && value <= this->max)
    {
        int ticks = (int) ((value - this->min) / this->interval);
        ((wxSlider *) this)->SetValue(ticks);

		if ( wxSlider::m_labels )
		{
			SetWindowText((*m_labels)[SliderLabel_Value], wxString::Format("%4.2f", value));
		}
    }
}
