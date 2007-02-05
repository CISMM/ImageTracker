#pragma once

#include "itkImage.h"
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
    typedef itk::RescaleIntensityImageFilter< InputImageType, DisplayImageType > RescaleType;
    typedef itk::VTKImageExport< DisplayImageType > ExportType;
    
    virtual void SetInput(itk::DataObject* input);
    virtual vtkProp* GetOutput();
    virtual void Update();
    
protected:
    ScalarImageItkVtkPipeline();
    virtual ~ScalarImageItkVtkPipeline();
    
private:
    RescaleType::Pointer normalize;
    ExportType::Pointer exporter;
    vtkImageImport* importer;
    vtkImageFlip* flipper;
    vtkImageShiftScale* rescaler;
    vtkImageActor* actor;
};
