#pragma once

#include <string>
#include "wx/wx.h"

#include "CommonTypes.h"

/**
 * \class FilterControlPanel
 * \brief Manages an itk image pipeline and associated controls.
 * 
 * A controller class that encapulates one or several itk filters within a wx controlling panel.
 */
class FilterControlPanel :
    public wxPanel
{
public:    
    typedef CommonTypes::InternalImageType ImageType;
    typedef ImageType::Pointer ImagePointer;
    
    /**
     * Set the input itk pipeline object for this FilterControlPanel.
     * The input data object is added to the beginning of the 
     * filter set controlled by this FilterControl.
     */
    virtual void SetInput(const ImageType* input);
    
    /**
     * Get the output itk pipeline object for this FilterControlPanel.
     * This is obtained from the end of the filter set controlled by
     * this FilterControlPanel.
     */
    virtual ImageType* GetOutput();
    
    std::string GetName();
    
    virtual ~FilterControlPanel(){}
    
protected:
    FilterControlPanel(wxWindow* parent, int id, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=wxTAB_TRAVERSAL);
    
    std::string name;

private:

};
