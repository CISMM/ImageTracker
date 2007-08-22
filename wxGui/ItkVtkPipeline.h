#pragma once

#include <vector>

#include "wx/window.h"

#include "itkDataObject.h"
#include "itkLightObject.h"
#include "vtkProp.h"
#include "vtkPropCollection.h"
#include "vtkRenderer.h"

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
    
    typedef std::vector< vtkProp* > PropCollectionType;
    
    virtual void SetInput(itk::DataObject* input) {}
    virtual void Update() {}
    
    /**
     * Get the vtkProp collection that this pipeline manages.
     */
    PropCollectionType& GetProps();
    
    /**
     * Adds the vtkProp objects managed by this pipeline to the given renderer.
     */
    void AddPropsTo(vtkRenderer* renderer);
    
    /**
     * Removes the vtkProp objects managed by this pipeline from the given renderer.
     */
    void RemovePropsFrom(vtkRenderer* renderer);
    
    /**
     * Create a wx control panel that can change options on this pipeline.  The default
     * wx control panel has no configurable options. Subclasses will override this method 
     * to provide controls that can manage them.  The returned wxWindow can be added to a
     * panel in a control interface.
     */
    virtual wxWindow* CreateWxControl(wxWindow* parent);
    
    // TODO: add visibility, opacity
    
protected:
    ItkVtkPipeline();
    virtual ~ItkVtkPipeline();
    
private:
    // Purposefully not implemented
    ItkVtkPipeline(const Self& other);
    void operator=(const Self& other);
    
    PropCollectionType props;
};
