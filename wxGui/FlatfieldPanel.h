// -*- C++ -*- generated by wxGlade 0.6.3 on Wed Aug 13 17:29:24 2008

#include <wx/wx.h>
#include <wx/image.h>

#ifndef FLATFIELDPANEL_H
#define FLATFIELDPANEL_H

// begin wxGlade: ::dependencies
// end wxGlade

// begin wxGlade: ::extracode
#include "FilterControlPanel.h"

// end wxGlade

#include "itkDivideImageFilter.h"
#include "itkSubtractImageFilter.h"


class FlatfieldPanel: public FilterControlPanel {
public:
    // begin wxGlade: FlatfieldPanel::ids
    enum {
        CHECK_FLAT_IMAGE = wxID_HIGHEST + 1155,
        BUTTON_BROWSE_FLAT = wxID_HIGHEST + 1157,
        CHECK_BACKGROUND_IMAGE = wxID_HIGHEST + 1159,
        BUTTON_BROWSE_BACKGROUND = wxID_HIGHEST + 1161
    };
    // end wxGlade

    FlatfieldPanel(wxWindow* parent, int id, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=0);
    
    typedef FilterControlPanel::ImageType ImageType;
    typedef itk::SubtractImageFilter< ImageType, ImageType, ImageType > SubtractType;
    typedef itk::DivideImageFilter< ImageType, ImageType, ImageType > DivideType;
    
    virtual void SetInput(const ImageType* input);
    virtual ImageType* GetOutput();

private:
    // begin wxGlade: FlatfieldPanel::methods
    void set_properties();
    void do_layout();
    // end wxGlade
    
    SubtractType::Pointer subtract;
    DivideType::Pointer divide;

protected:
    // begin wxGlade: FlatfieldPanel::attributes
    wxStaticBox* sizer_70_staticbox;
    wxCheckBox* checkFlatImage;
    wxButton* buttonBrowseFlat;
    wxTextCtrl* textFlatImage;
    wxCheckBox* checkBackgroundImage;
    wxButton* buttonBrowseBackground;
    wxTextCtrl* textBackgroundImage;
    wxTextCtrl* text_ctrl_5;
    // end wxGlade

    DECLARE_EVENT_TABLE();

    ImageType::Pointer MakeSubtractImage();
    ImageType::Pointer MakeDivideImage();
    
public:
    virtual void OnFlatImage(wxCommandEvent &event); // wxGlade: <event_handler>
    virtual void OnBrowseFlat(wxCommandEvent &event); // wxGlade: <event_handler>
    virtual void OnBackgroundImage(wxCommandEvent &event); // wxGlade: <event_handler>
    virtual void OnBrowseBackground(wxCommandEvent &event); // wxGlade: <event_handler>
}; // wxGlade: end class


#endif // FLATFIELDPANEL_H
