#include "FilterControlPanel.h"

#include "Logger.h"

FilterControlPanel::FilterControlPanel(wxWindow* parent, int id, const wxPoint& pos, const wxSize& size, long style)
    : wxPanel(parent, id, pos, size, style), name("Filter")
{
}

std::string FilterControlPanel::GetName()
{
    return this->name;
}

void FilterControlPanel::SetInput(const ImageType* input)
{
    std::string function("FilterControlPanel::SetInput");
    Logger::warning << function << ": Please implement this method in the subclass!" << std::endl;
}

FilterControlPanel::ImageType* FilterControlPanel::GetOutput()
{
    std::string function("FilterControlPanel::GetOutput");
    Logger::warning << function << ": Please implement this method in the subclass!" << std::endl;
	return NULL;
}
