#pragma once

#include <string>
#include <utility>
#include <vector>

#include <wx/arrstr.h>
#include <wx/event.h>
#include <wx/thread.h>

#include "itkLightObject.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkTIFFWriter.h"
#include "vtkWindowToImageFilter.h"

#include "DataSource.h"
#include "ItkVtkPipeline.h"

/**
 * \class ImageTrackerController
 * \brief Key moderator of the ImageTracker application.
 *
 * Maintains data sources and visualization pipelines. Provides methods for
 * querying and managing the data and visualization objects that exist in
 * the system. This object is a singleton--only one of these is
 * expected to exist at a time, and many objects need to access controller
 * methods for dealing with rendering and obtaining data objects.
 *
 * The singleton implementation of this ImageTrackerController is method-static.
 * That is, the singleton object returned is a static variable in the Instance()
 * method. For this reason, DeleteInstance() needs to be called at a "safe" time
 * when the application is closing. A safe time means when the data and visualization
 * objects to which this controller refers can be cleaned up.
 *
 * This implementation was adopted after the global static approach was found to fail
 * on Windows platforms.  As the datavis pair was being cleared, itk would request
 * Mutex locks to delete these objects.  These requests failed, presumably because the
 * application was shutting down.
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
    
    // Type to tie each DataSource to an ItkVtkPipeline for visualization.
    typedef std::pair< DataSource::Pointer, ItkVtkPipeline::Pointer > DataVisualPair;
    // Type to hold a list of DataVisualPairs.
    typedef std::vector< DataVisualPair > DataVisualList;
    
    /**
     * Obtain the singleton instance of this ImageTrackerController.
     */
    static ImageTrackerController::Pointer Instance();

    /**
     * Delete the singleton instance of this ImageTrackerController. Use this method carefully--
     * typically, right before the application closes.
     */
    static void DeleteInstance();
    
    /**
     * Retrieves a reference to the DataSource at the specified index.
     */
    DataSource::Pointer GetDataSource(unsigned int i);
    
    /**
     *
     */
    ItkVtkPipeline::Pointer GetVisualization(unsigned int i);
    
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

    unsigned int GetFrameIndex() { return this->frameIndex; }
    
    unsigned int GetMaxIndex();
    
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
    void SetRenderWindow(vtkRenderWindow* rw);
    
    vtkRenderWindow* GetRenderWindow();
    
    /**
     * Populate a list of the data sources managed by this controller.  We
     * use a wxArrayString for using this list in wx widgets.
     */
    void GetDataSourceNames(wxArrayString& names);

    /**
     * Returns a handle to the renderer this controller manages.
     */
    vtkRenderer* GetRenderer();

    /**
     * Convenience method to update the renderer managed by this controller.
     */
    void Render();
    
    bool IsIndexChanged();
    void SetIsIndexChanged(bool changed);
    
    bool IsDataChanged();
    void SetIsDataChanged(bool changed);
    
    /**
     * Cycles through all frames of the data managed by this ImageTrackerControler,
     * and saves them as tiffs in files specified by a FileSet.
     */
    void SaveViewImage(const std::string& fileName);

protected:
    // Making the New() method protected ensures this is a singleton.
    itkNewMacro(Self);
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

    // Objects for saving view images to disk
    vtkWindowToImageFilter* capture;
    vtkTIFFWriter* writer;

    // The wxWidgets window that created this controller.
    wxWindow* parent;

    // A flag indicating if this controller has new data sources.
    bool isDataChanged;
    // A flag indicating if the controller's index has changed recently
    bool isIndexChanged;
    
    static wxMutex s_ControllerMutex;
};
