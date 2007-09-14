#pragma once

#include "itkImage.h"
#include "itkIntensityWindowingImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkVTKImageExport.h"

#include "vtkImageActor.h"
#include "vtkImageFlip.h"
#include "vtkImageImport.h"
#include "vtkImageShiftScale.h"

#include "ItkVtkPipeline.h"
#include "Logger.h"

/**
 * Pipeline that connects an itk image object to a vtk renderer.
 * This class provides scalar image visualization options such as
 * intensity rescaling and opacity.
 */
class ScalarImageItkVtkPipeline :
    public ItkVtkPipeline
{
public:
    typedef ScalarImageItkVtkPipeline Self;
    typedef ItkVtkPipeline Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    itkNewMacro(Self);
    
    typedef itk::Image< float, 2 > InputImageType;
    typedef itk::Image< unsigned char, 2 > DisplayImageType;
    typedef InputImageType::PixelType InputPixelType;
    typedef DisplayImageType::PixelType DisplayPixelType;
    typedef itk::IntensityWindowingImageFilter< InputImageType, DisplayImageType > WindowType;
    typedef itk::VTKImageExport< DisplayImageType > ExportType;
    
    void SetVisibility(int visible);
    int GetVisibility() const;
    
    void SetWindowMinimum(const InputPixelType min);
    void SetWindowMaximum(const InputPixelType max);
    InputPixelType GetWindowMinimum() const;
    InputPixelType GetWindowMaximum() const;
    
//     void SetDisplayMinimum(const DisplayPixelType min);
//     void SetDisplayMaximum(const DisplayPixelType max);
    
    virtual void SetInput(itk::DataObject* input);
    virtual void Update();
    
    /**
     * Creates a wxWindow (panel) that can control the parameters of this vector glyph pipeline.
     */
    virtual wxWindow* CreateWxControl(wxWindow* parent);
    
    InputImageType::Pointer GetInput();
    
protected:
    ScalarImageItkVtkPipeline();
    virtual ~ScalarImageItkVtkPipeline();
    
private:
    WindowType::Pointer window;
    ExportType::Pointer exporter;
    vtkImageImport* importer;
    vtkImageFlip* flipper;
    vtkImageShiftScale* rescaler;
    vtkImageActor* actor;
    bool firstTime;
};
