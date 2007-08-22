#pragma once

#include <string>
#include <utility>
#include <vector>

#include <wx/arrstr.h>
#include <wx/event.h>
#include <wx/thread.h>
#include <wx/window.h>

#include "itkLightObject.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"

#include "DataSource.h"
#include "ItkVtkPipeline.h"

/**
 * /class ImageTrackerController
 * /brief Key moderator of the ImageTracker application.
 * Maintains data sources and visualization pipelines. Provides methods for
 * querying and managing the data and visualization objects that exist in
 * the system.
 */
class ImageTrackerController :
    public itk::LightObject
{
public:
    // Commong ITK typedefs
    typedef ImageTrackerController Self;
    typedef itk::LightObject Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    itkNewMacro(Self);
    
    // Type to tie each DataSource to an ItkVtkPipeline for visualization.
    typedef std::pair< DataSource::Pointer, ItkVtkPipeline::Pointer > DataVisualPair;
    // Type to hold a list of DataVisualPairs.
    typedef std::vector< DataVisualPair > DataVisualList;
    
    /**
     * Retrieves a reference to the DataSource at the specified index.
     */
    DataSource::Pointer GetDataSource(unsigned int i)
    {
        return i >= this->datavis.size() ? NULL : this->datavis[i].first;
    }
    
    /**
     *
     */
    ItkVtkPipeline::Pointer GetVisualization(unsigned int i)
    {
        return i >= this->datavis.size() ? NULL : this->datavis[i].second;
    }
    
    /**
     * Adds a DataSource to the list managed by this controller.
     */
    void AddDataSource(DataSource::Pointer source);
    
    /**
     * Removes the data source at the given index from this controller.
     */
    void RemoveDataSource(unsigned int i);
    
    /**
     * Sets the index into the DataSources managed by this controller. This 
     * specifies the active data item within all the DataSources.  (As opposed 
     * to the index into the list of DataSources managed by this controller.)
     */
    void SetFrameIndex(unsigned int index);
    
    /**
     * Sets the index of the active DataSource managed by this controller.
     */
    void SetDataIndex(unsigned int index);
    
    /**
     * Returns the size of the largest data source set managed by this controller.
     */
    unsigned int GetMaxSize();
    
    /**
     * Updates all visualization pipelines with the proper parameters.
     */
    void UpdateView();
    
    /**
     * Sets the vtkRenderWindow drawn to by this controller.
     */
    void SetRenderWindow(vtkRenderWindow* rw)
    {
        this->renderWindow = rw;
        this->renderWindow->AddRenderer(this->GetRenderer());
    }
    
    vtkRenderWindow* GetRenderWindow()
    { return this->renderWindow; }
    
    /**
     * Sets the ImageTracker window referring to this controller.
     */
    void SetParent(wxWindow* parent);
    
    /**
     * Populate a list of the data sources managed by this controller.  We
     * use a wxArrayString for using this list in wx widgets.
     */
    void GetDataSourceNames(wxArrayString& names);

    /**
     * Returns a handle to the renderer this controller manages.
     */
    vtkRenderer* GetRenderer()
    {
        if (this->renderer == NULL)
            this->renderer = vtkRenderer::New();
        
        return this->renderer;
    }

    bool IsControllerChanged();
    void SetIsControllerChanged(bool changed);

protected:
    ImageTrackerController();
    virtual ~ImageTrackerController();
    
private:
    // Not implemented
    ImageTrackerController(const Self& other);
    void operator=(const Self& other);
    
    // Data members
    
    // The current data frame index.
    unsigned int frameIndex;

    // The current data source index
    unsigned int dataIndex;
    
    // The set of data and visualization pairs.
    DataVisualList datavis;

    // A flag indicating if the camera should be reset.
    bool resetCamera;

    // The VTK Renderer in which visualizations are displayed
    vtkRenderer* renderer;

    // The VTK RenderWindow that houses the vtk renderer.
    vtkRenderWindow* renderWindow;

    // The wxWidgets window that created this controller.
    wxWindow* parent;

    // A flag indicating if this controller has new data sources.
    bool isControllerChanged;
    static wxMutex s_ControllerMutex;
};
