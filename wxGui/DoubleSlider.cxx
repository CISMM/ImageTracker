#include ".\DoubleSlider.h"

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
    if (min < max)
    {
        this->min = min;
        this->max = max;
        this->interval = interval;

        int count = (int) (max - min) / interval;
        ((wxSlider *) this)->SetRange(0, count);
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
    }
}
