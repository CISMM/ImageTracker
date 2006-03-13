/////////////////////////////////////////////////////////////////////////////
// Name:        ImageWindow.cxx
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#include "ImageWindow.h"
#include "Logger.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// ImageWindow
//----------------------------------------------------------------------------

std::vector<ImageWindow*>* ImageWindow::s_windowList = new std::vector<ImageWindow*>;
int ImageWindow::s_windowCount = 0;

// WDR: event table for ImageWindow

BEGIN_EVENT_TABLE(ImageWindow,wxDialog)
END_EVENT_TABLE()

ImageWindow::ImageWindow( wxWindow *parent, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style ) :
    wxDialog( parent, id, title, position, size, style )
{
    // WDR: dialog function ImageWindowDialog for ImageWindow
    CreateVtkViewer(this, true);
    Logger::logDebug("ImageWindow::constructor");
}

ImageWindow::~ImageWindow()
{
    this->Close(true);
}

void ImageWindow::ImShow(CommonTypes::InternalImageType::Pointer image)
{
    ImageWindow::s_windowCount++;
    wxString title;
    title.Printf("Image %d", ImageWindow::s_windowCount);

    ImageWindow::ImShow(image, std::string(title.c_str()));
}

void ImageWindow::ImShow(CommonTypes::InternalImageType::Pointer image, std::string title)
{
    ImageWindow::s_windowCount++;
    wxString wxtitle = _(title.c_str());

    ImageWindow* wind = new ImageWindow((wxWindow*) NULL, -1, wxtitle);
    wind->GetCanvas()->SetItkImage(image);
    wind->Show(true);
    ImageWindow::s_windowList->push_back(wind);
}

void ImageWindow::Destroy()
{
    std::vector<ImageWindow*>::iterator it = ImageWindow::s_windowList->begin();
    while (it != ImageWindow::s_windowList->end())
    {
        delete *it;
        it++;
    }
}

// WDR: handler implementations for ImageWindow




