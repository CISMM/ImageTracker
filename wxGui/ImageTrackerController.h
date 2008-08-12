#pragma once

#include <string>
#include <vector>

#include <wx/arrstr.h>
#include <wx/event.h>
#include <wx/thread.h>
#include <wx/wx.h>

#include "itkLightObject.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkTIFFWriter.h"
#include "vtkWindowToImageFilter.h"

#include "FileSet.h"
#include "FilterControlPanel.h"
#include "ImageFileSet.h"
#include "ImageFileSetPanel.h"
#include "ItkVtkPipeline.h"
#include "VectorFileSetReader.h"

/**
 * \class ImageTrackerController
 * \brief Key moderator of the ImageTracker application.
 *
 * Maintains data sets and visualization pipelines. Provides methods for
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
 * on Windows platforms.  As the image filters were being cleared, itk would request
 * Mutex locks to delete these objects.  These requests failed, presumably because the
 * application was shutting down.
 * 
 * Synchronization among different threads is a little bit odd, due to a need for a
 * cross-platform solution.  This approach works in Linux and Windows.  Access to image
 * filter data is controlled through a mutex s_ControllerMutex.  This mutex controls
 * access to methods to clear, add, and remove filters from the pipeline as well as
 * accessor methods to check whether the data held by this controller has been changed.
 * These methods are expected to be used on the main application thread.
 * 
 * Data can also be submitted to the controller through a "backdoor."  This approach was
 * adopted because there were inconsistent synchronization blocks occuring when a process
 * on a different thread tried to clear the filter pipeline while the UI thread requested
 * an update of information.  A mutex s_BackdoorMutex controls access to these backdoor
 * methods.  The main application thread can call CheckForUpdates() to apply any data
 * changes requested through the backdoor, and then check for the data changed flag.  This
 * ensures that data is loaded into the controller objects and reflected in the UI from 
 * one thread.
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
        
    // Type to hold a set of FilterControlPanels.
    typedef std::vector< FilterControlPanel* > ImagePipelineType;

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
     * Add a FilterControlPanel to the set of filters managed by this ImageTrackerController.
     */
    void AddFilter(FilterControlPanel* filter);
    
    /**
     * Removes the filter at the specified index, if possible.
     */
    void RemoveFilter(int idx);
    
    /**
     * Reset the FilterControlPanel set managed by this ImageTrackerController.
     * Optionally, load a new set of image files into the initial ImageFileSetPanel.
     */
    void ClearFilters(const FileSet& newFiles = FileSet());

    /**
     * Updates the image filter pipeline and display to reflect changes
     * made to the image file set.
     */
    void UpdateImageFileSet();
    
    /**
     * Populate a list of the names of the FilterControlPanels managed by 
     * this ImageTrackerController.
     */
    void GetFilterNames(wxArrayString& list);
    
    /**
     * Return the wx panel that controls the filter with the given index.
     */
    wxWindow* GetFilterPanel(unsigned int index);    
    
    /**
     * Set the wxWindow parent that is the container for filter control
     * panels.  This needs to be available to all control panels that 
     * get created, possibly initially hidden.
     */
    void SetFilterParent(wxWindow* parent);
    
    /**
     * Retrieve the wxWindow parent that is the container for filter 
     * control panels.  This is needed to create control panels that 
     * may not be immediately visible and may be swapped for other 
     * content in some window.
     */
    wxWindow* GetFilterParent();    
    
    /**
     * Return a reference to the currently loaded ImageFileSet, with
     * filters applied.
     */
    ImageFileSet* GetImageFileSet();
    
	/**
	 * Get the list of image files currently loaded.
	 */
    FileSet& GetImageFiles();
    
	/**
	 * Set the list of vector image files loaded.
	 */
    void SetVectorFiles(const FileSet& files);
    
	/**
	 * Get the list of vector image files loaded.
	 */
    FileSet& GetVectorFiles();
    
	/**
	 * Get the VectorFileSet managed by this ImageTrackerController.
	 */
    VectorFileSet* GetVectorFileSet();
    
	/**
	 * Set the visualization used for displaying vector images.
	 */
    void SetVectorVisualization(ItkVtkPipeline::Pointer pipeline);

	/**
	 * Get the visualization used for displaying vector images.
	 */
	ItkVtkPipeline::Pointer GetVectorVisualization();

	/**
	 * Get the visualization used for displaying images.
	 */
    ItkVtkPipeline::Pointer GetImageVisualization();    
        
    /**
     * Sets the image index for the image file set that is currently
     * controled by this ImageTrackerController.
     */
    void SetImageIndex(unsigned int index);

    /**
     * Returns the image index for the image file set.
     */
    unsigned int GetImageIndex();
        
    /**
     * Returns the number of images in the set managed by this controller.
     */
    unsigned int GetImageCount();
    
    /**
     * Returns the number of vector images in the set managed by this controller.
     */
    unsigned int GetVectorCount();
    
    /**
     * Updates all visualizations with the proper parameters.
     */
    void UpdateView();
    
    /**
     * Sets the vtkRenderWindow drawn to by this controller.
     */
    void SetRenderWindow(vtkRenderWindow* rw);
    
    /**
     * Gets the vtkRenderWindow drawn to by this controller.
     */
    vtkRenderWindow* GetRenderWindow();
    
    /**
     * Returns a handle to the renderer this controller manages.
     */
    vtkRenderer* GetRenderer();

    /**
     * Convenience method to update the renderer managed by this controller.
     */
    void Render();
    
    bool IsDataChanged();
    void SetIsDataChanged(bool changed);
    
    /**
     * Checks for updates to the image files loaded.  Updates the internal set of image
     * files if any have been loaded "through the backdoor."  Returns true if there has
     * been a change to the data held by the ImageTrackerController.
     */
	bool CheckForUpdates();
    
    /**
     * Saves the image displayed in the render window as a TIFF
     * file with the specified name.
     */
    void SaveViewImage(const std::string& fileName);

    /**
     * Specify file names that should be loaded the next time ImageTrackerController is
     * asked to check for updates.
     */
	void SetBackdoorFiles(const FileSet& files, bool isImage);

protected:
    // Making the New() method protected ensures this is a singleton.
    itkNewMacro(Self);
    ImageTrackerController();
    virtual ~ImageTrackerController();
    
    /**
     * Return the ImageFileSetPanel that loads the images into
     * the itk pipeline.
     */
    ImageFileSetPanel* GetImageFileSetPanel();
    
    /**
     * Retrieve a blank panel that can take the place of filter
     * controls when none is active.
     */
    wxWindow* GetBlankPanel();

	void CheckBackdoorData();
    
private:
    // Not implemented
    ImageTrackerController(const Self& other);
    void operator=(const Self& other);
    
    // Data members
    
    /**
     * Holds a set of FilterControlPanels, which creates an itk image filter
     * pipeline and associated control panels for parameter manipulation.
     */
    ImagePipelineType imagePipeline;
    
    // Holds a set of vector files
    VectorFileSetReader::Pointer vectorImages;
        
    // The image visualization
    ItkVtkPipeline::Pointer imageVisualization;
    
    // The vector visualization
    ItkVtkPipeline::Pointer vectorVisualization;

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
    wxWindow* wxFilterParent;
    wxWindow* wxBlankPanel;

    // A flag indicating if this controller has new data
    bool isDataChanged;
    
	// A flag indicating if this controller has been passed data
    bool isBackdoorData;
    // A flag that indicates if the backdoor FileSet specifies image files
	bool isBackdoorImage;
    // A set of files to load through the backdoor
	FileSet backdoorFiles;
    
    /**
     * These mutexes syncronizes changes to critical data that may occur
     * across different threads.
     */
    static wxMutex s_ControllerMutex;
	static wxMutex s_BackdoorMutex;
};
