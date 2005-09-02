/////////////////////////////////////////////////////////////////////////////
// Name:        VtkCanvas.h
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#ifndef __VtkCanvas_H__
#define __VtkCanvas_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "VtkCanvas.cxx"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "vtkViewer_wdr.h"

#include "itkRescaleIntensityImageFilter.h"
#include "itkImageToVTKImageFilter.h"

#include "vtkImageActor.h"
#include "vtkImageFlip.h"
#include "vtkInteractorStyleImage.h"
#include "vtkRenderer.h"

#include "../CommonTypes.h"
#include "ItkMagickIO.h"
#include "wxVTKRenderWindowInteractor.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// VtkCanvas
//----------------------------------------------------------------------------

class VtkCanvas: public wxPanel
{
public:
    typedef CommonTypes::InternalImageType InputImageType;
    typedef CommonTypes::DisplayImageType DisplayImageType;

    // constructors and destructors
    VtkCanvas( wxWindow *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER );
    virtual ~VtkCanvas();
    
    // WDR: method declarations for VtkCanvas
    wxVTKRenderWindowInteractor* GetVtkInteractor()  { return (wxVTKRenderWindowInteractor*) FindWindow( ID_VTK_INTERACTOR ); }
    void SetFileName(std::string filename);
    void SetItkImage(InputImageType::Pointer image);

private:
    void DefaultRenderer();
    void InitializePipeline();
    
    // WDR: member variable declarations for VtkCanvas
    typedef itk::RescaleIntensityImageFilter<InputImageType, DisplayImageType> RescalerType;
    typedef itk::ImageToVTKImageFilter<DisplayImageType> ConverterType;

    bool firstTime;
    ItkMagickIO::Pointer reader;
    RescalerType::Pointer rescaler;
    ConverterType::Pointer converter;
    vtkImageFlip* flipper;
    vtkImageActor* actor;
    vtkRenderer* renderer;
    
private:
    // WDR: handler declarations for VtkCanvas

private:
    DECLARE_EVENT_TABLE()
};

#endif
