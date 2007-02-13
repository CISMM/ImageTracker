#pragma once

#include "itkDataObject.h"
#include "itkLightObject.h"
#include "vtkProp.h"

#include "Logger.h"

/**
 * Links an itk data object (image) to a vtk visualization (renderer).
 * Specific subclasses handle data transfer and visualization for different
 * types of itk objects.
 */
class ItkVtkPipeline :
    public itk::LightObject
{
public:
    typedef ItkVtkPipeline Self;
    typedef itk::LightObject Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    
    virtual void SetInput(itk::DataObject* input) {}
    virtual vtkProp* GetOutput()
    {
        Logger::warning << "ItkVtkPipeline::GetOutput(): Base class method called, but should be overriden." << std::endl;
		return NULL;
    }
    virtual void Update() {}
    
    // TODO: add visibility, opacity
    
protected:
    ItkVtkPipeline()
    {}
    virtual ~ItkVtkPipeline()
    {}
    
private:
    // Purposefully not implemented
    ItkVtkPipeline(const Self& other);
    void operator=(const Self& other);
};
