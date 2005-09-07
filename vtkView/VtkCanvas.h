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
#include "vtkInteractorStyle.h"
#include "vtkInteractorStyleImage.h"
#include "vtkRenderer.h"

#include "../CommonTypes.h"
#include "ItkMagickIO.h"
#include "wxVTKRenderWindowInteractor.h"

// WDR: class declarations

/*
 * VtkCanvas is a wxPanel that can display images using vtk.  It wraps a
 * wxVTKRenderWindowInteractor and an image display pipeline.  As a panel,
 * it can be embedded anywhere in a wx application pretty easily.  Its
 * interface makes displaying new images easy--just pass a file name or
 * itk image pointer.
 */
class VtkCanvas : public wxPanel
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
    /* Get the vtk render window interactor embedded in this canvas */
    wxVTKRenderWindowInteractor* GetVtkInteractor()  { return (wxVTKRenderWindowInteractor*) FindWindow( ID_VTK_INTERACTOR ); }
    /* Set the file name of the image to display in this canvas */
    void SetFileName(std::string filename);
    /* Set the itk image to display in this canvas */
    void SetItkImage(InputImageType::Pointer image);

private:
    /* 
     * Create and use a default renderer for this canvas.  This is
     * useful when the canvas is first displayed and nothing is yet
     * placed inside it.
     */
    void DefaultRenderer();
    /*
     * Initialize the components and assemble an image viewing pipeline.
     * This includes components to read images, rescale intensities (for
     * display) pixel conversion, axis flipping, and rendering in vtk.
     */
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
    vtkInteractorStyle* style;
    
private:
    // WDR: handler declarations for VtkCanvas

private:
    DECLARE_EVENT_TABLE()
};

#endif
