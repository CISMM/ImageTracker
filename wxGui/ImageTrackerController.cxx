#include "ImageTrackerController.h"

#include <wx/thread.h>

#include "CompositeImageFileSet.h"
#include "ImageFileSetPanel.h"
#include "Logger.h"
#include "ScalarImageVisualization.h"
#include "VectorGlyphVisualization.h"
#include "wxUtils.h"

wxMutex ImageTrackerController::s_ControllerMutex;
wxMutex ImageTrackerController::s_BackdoorMutex;

ImageTrackerController::Pointer ImageTrackerController::Instance()
{
    // The singleton instance of this controller
    // Note that making this method-static means we have to be sure
    // to call DeleteInstance() when the application closes!!!
    static ImageTrackerController::Pointer s_instance;

    if (s_instance.IsNull())
        s_instance = ImageTrackerController::New();
    
    return s_instance;
}

void ImageTrackerController::DeleteInstance()
{
    ImageTrackerController::Pointer instance = ImageTrackerController::Instance();
    
    // Because the visualization capture filter has a reference to the renderer
    // window, if the renderer window is deleted first, we get a seg fault for 
    // trying to delete an object with non-zero reference count.  
    // So, here we reset the input to the capture object.  This method should 
    // be called before the renderer window is deleted.
    if (instance->capture)
        instance->capture->SetInput(NULL);
    
    // Delete the vtk renderer, which we create
    if (instance->renderer)
        instance->renderer->Delete();

    instance->renderWindow = NULL;
    instance->wxFilterParent = NULL;
    instance->wxBlankPanel = NULL;
    instance = NULL;
}

ImageTrackerController::ImageTrackerController() :
    imagePipeline(),
    resetCamera(false),
    isDataChanged(false),
    isBackdoorData(false),
	isBackdoorImage(true),
	backdoorFiles()
{
    // Set display pointers to null
    this->renderer = NULL;
    this->renderWindow = NULL;
    this->wxFilterParent = NULL;
    this->wxBlankPanel = NULL;

    this->imageVisualization = ScalarImageVisualization::New();
    
    // Set up vector pipeline
    this->vectorImages = VectorFileSetReader::New();
    this->vectorVisualization = VectorGlyphVisualization::New();
    
    // Setup view saving pipeline
    this->capture = vtkWindowToImageFilter::New();
    this->writer = vtkTIFFWriter::New();
    this->writer->SetInputConnection(this->capture->GetOutputPort());
    this->writer->SetCompressionToNoCompression();    
}
    
ImageTrackerController::~ImageTrackerController()
{
}

void ImageTrackerController::ClearFilters(const FileSet& files)
{
    // There is a danger of ImageTracker requesting a redraw
    // while there are no filters in the pipeline, so we need
    // to apply a mutex lock
    wxMutexLocker lock(ImageTrackerController::s_ControllerMutex);
    std::string function("ImageTrackerController::ClearFilters");
    
    Logger::verbose << function << ": Pipeline size: " << this->imagePipeline.size() << std::endl;
        
    // Remove the image visualization from the renderer because we cannot render anything while
    // we are updating the pipeline. We create a new visualization to reset region and contrast
    // in case the content of images has changed a lot.
    this->imageVisualization->RemovePropsFrom(this->GetRenderer());
    this->imageVisualization = ScalarImageVisualization::New();

	// Delete everything from the old pipeline
    if (this->imagePipeline.size() > 0)
    {
        for (ImagePipelineType::iterator it = this->imagePipeline.begin();
            it != this->imagePipeline.end();
            ++it)
        {
            delete (*it);
        }
        
        this->imagePipeline.clear();
    }
    
    // Add an ImageFileSetPanel to the image pipeline.  This should always
    // be the first "filter" in the pipeline.
    ImageFileSetPanel* pPanel = new ImageFileSetPanel(this->GetFilterParent(), -1);
    // push filter onto list
    this->imagePipeline.push_back(pPanel);

    // Add files to panel, if needed
	Logger::debug << function << ": new FileSet has size => " << files.size() << std::endl;
    if (files.size() > 0)
	{
        pPanel->SetFileSet(files);
		this->imageVisualization->AddPropsTo(this->GetRenderer());	// add visualization to renderer
	}
    
    // Our data has changed...
    this->resetCamera = true;
	this->isDataChanged = true;
}

void ImageTrackerController::AddFilter(FilterControlPanel* panel)
{
    wxMutexLocker lock(ImageTrackerController::s_ControllerMutex);
	// We are always guaranteed to have a previous filter
	FilterControlPanel* previous = this->imagePipeline[this->imagePipeline.size()-1];
	panel->SetInput(previous->GetOutput());
	this->imagePipeline.push_back(panel);
	this->isDataChanged = true;
}

void ImageTrackerController::RemoveFilter(int idx)
{
    // There is a danger of ImageTracker requesting a redraw
    // while we are changing filters in the pipeline, so we need
    // to apply our mutex lock
    wxMutexLocker lock(ImageTrackerController::s_ControllerMutex);
    std::string function("ImageTrackerController::RemoveFilter");

    int filterCount = this->imagePipeline.size();
    FilterControlPanel* old = NULL;
    
    if (idx <= 0 || idx >= filterCount)     // invalid request...ignore
    {
		Logger::verbose << function << ": cannot remove index => " << idx << std::endl;
        return;
    }
    else if (idx == (filterCount - 1))      // remove the last item from the list
    {
		Logger::verbose << function << ": removing last filter index => " << idx << std::endl;
        old = this->imagePipeline[idx];
        this->imagePipeline.erase(this->imagePipeline.begin() + idx);
    }
    else                                    // remove an item from the middle of the list
    {
		Logger::verbose << function << ": bypassing filter index => " << idx << std::endl;
        old = this->imagePipeline[idx];
        // rearrange filter pipeline to bypass old filter
        this->imagePipeline[idx+1]->SetInput(this->imagePipeline[idx-1]->GetOutput());
        this->imagePipeline.erase(this->imagePipeline.begin() + idx);
    }
    
    // Delete the removed filter
    if (old)
        delete (old);
    
	this->isDataChanged = true;
}

void ImageTrackerController::UpdateImageFileSet()
{
    // Perform the updating of the pipeline objects within a mutex lock
    wxMutexLocker lock(ImageTrackerController::s_ControllerMutex);
    
    // Remove the image visualization from the renderer (we may have nothing to display)
    this->imageVisualization->RemovePropsFrom(this->GetRenderer());
    this->imageVisualization = ScalarImageVisualization::New();
    
    // Update the image pipeline.
    // The image files are specified by the first filter in the image pipeline;
    // the input needs to be updated to the second filter, if it exists.
    if (this->imagePipeline.size() > 1)
    {
        this->imagePipeline[1]->SetInput(this->imagePipeline[0]->GetOutput());
    }
    
    // If there are images to display, add image visualization to renderer
    if (this->GetImageCount() > 0)
    {
        this->imageVisualization->AddPropsTo(this->GetRenderer());
    }
    
    // Update the display
    this->resetCamera = true;
	this->isDataChanged = true;
}

void ImageTrackerController::SetBackdoorFiles(const FileSet& files, bool isImage)
{
	wxMutexLocker lock(ImageTrackerController::s_BackdoorMutex);
	std::string function("ImageTrackerController::SetBackdoorFiles");
	Logger::debug << function << ":\tfile count => " << files.size() << "\tis images => " << isImage << std::endl;
	this->backdoorFiles = files;
	this->isBackdoorImage = isImage;
	this->isBackdoorData = true;
}

void ImageTrackerController::CheckBackdoorData()
{
	wxMutexLocker lock(ImageTrackerController::s_BackdoorMutex);
	std::string function("ImageTrackerController::CheckBackdoorData");
	if (this->isBackdoorData)
	{
		Logger::debug << function << ": updating controller with backdoor data" << std::endl;
		if (this->isBackdoorImage)
			this->ClearFilters(this->backdoorFiles);
		else
			this->SetVectorFiles(this->backdoorFiles);

		this->isBackdoorData = false;
	}
}

bool ImageTrackerController::CheckForUpdates()
{
	this->CheckBackdoorData();
	return this->IsDataChanged();
}

void ImageTrackerController::GetFilterNames(wxArrayString& names)
{
    names.clear();
    for (ImagePipelineType::iterator it = this->imagePipeline.begin();
         it != this->imagePipeline.end();
         ++it)
    {
        names.Add(nano::std2wx((*it)->GetName()));
    } 
}

wxWindow* ImageTrackerController::GetFilterPanel(unsigned int index)
{
    if (index < this->imagePipeline.size() && index >= 0)
        return this->imagePipeline[index];
    else
        return this->GetBlankPanel();
}

ImageFileSetPanel* ImageTrackerController::GetImageFileSetPanel()
{
    std::string function("ImageTrackerController::GetImageFileSetControl");
    ImageFileSetPanel* pPanel = NULL;
    
    if (this->imagePipeline.size() < 1 ||
        !(pPanel = dynamic_cast<ImageFileSetPanel*>(this->imagePipeline[0])))
    {
        Logger::error << function << ": ImageFileSetPanel was not the first item in the image pipeline.  This is unexpected." << std::endl;
    }
    
    return pPanel;
}

ImageFileSet* ImageTrackerController::GetImageFileSet()
{
    CompositeImageFileSet* composite = new CompositeImageFileSet();
    ImageFileSetReader* reader = this->GetImageFileSetPanel()->GetReader().GetPointer();
    FilterControlPanel* filter = this->imagePipeline[this->imagePipeline.size()-1];
    composite->SetStart(reader);
    composite->SetEnd(filter);
    
    return composite;
}

FileSet& ImageTrackerController::GetImageFiles()
{
    return this->GetImageFileSetPanel()->GetFileSet();
}

void ImageTrackerController::SetFilterParent(wxWindow* parent)
{
    this->wxFilterParent = parent;
}

wxWindow* ImageTrackerController::GetFilterParent()
{   
    return this->wxFilterParent;
}

wxWindow* ImageTrackerController::GetBlankPanel()
{
    if (!this->wxBlankPanel)
        this->wxBlankPanel = new wxPanel(this->GetFilterParent(), -1);
    
    return this->wxBlankPanel;
}

void ImageTrackerController::SetVectorFiles(const FileSet& files)
{
    wxMutexLocker lock(ImageTrackerController::s_ControllerMutex);
    // update our vector image reader
    this->vectorImages->SetFileSet(files);
    
    // remove the previous visualization
    this->vectorVisualization->RemovePropsFrom(this->GetRenderer());
    
    // add our visualization back, if needed
    if (this->vectorImages->GetImageCount() > 0)
    {
        this->vectorVisualization->AddPropsTo(this->GetRenderer());
    }
    
    // signal that our data has been changed
    this->resetCamera = true;
    this->isDataChanged = true;
}

FileSet& ImageTrackerController::GetVectorFiles()
{
    return this->vectorImages->GetFileSet();
}

VectorFileSet* ImageTrackerController::GetVectorFileSet()
{
    return this->vectorImages;
}

void ImageTrackerController::SetVectorVisualization(ItkVtkPipeline::Pointer pipeline)
{
    // remove the current visualization
    this->vectorVisualization->RemovePropsFrom(this->GetRenderer());
    
    // update our visualization
    this->vectorVisualization = pipeline;
    
    // add the visualization back, if needed
    if (this->vectorImages->GetImageCount() > 0)
    {
        this->vectorVisualization->AddPropsTo(this->GetRenderer());
    }
}

ItkVtkPipeline::Pointer ImageTrackerController::GetImageVisualization()
{
    return this->imageVisualization;
}

ItkVtkPipeline::Pointer ImageTrackerController::GetVectorVisualization()
{
    return this->vectorVisualization;
}

bool ImageTrackerController::IsDataChanged()
{
    wxMutexLocker lock(ImageTrackerController::s_ControllerMutex);
    return this->isDataChanged;
}

void ImageTrackerController::SetIsDataChanged(bool changed)
{
    wxMutexLocker lock(ImageTrackerController::s_ControllerMutex);
    this->isDataChanged = changed;
}

void ImageTrackerController::SetImageIndex(unsigned int index)
{
    // Logger::verbose << "ImageTrackerController::SetIndex: " << index << std::endl;
    this->GetImageFileSetPanel()->SetImageIndex(index);
    this->vectorImages->SetImageIndex(index);
}

unsigned int ImageTrackerController::GetImageIndex()
{
    return this->GetImageFileSet()->GetImageIndex();
}

unsigned int ImageTrackerController::GetImageCount()
{
    ImageFileSetPanel* pPanel = this->GetImageFileSetPanel();
    if (pPanel)
         return pPanel->GetReader()->GetImageCount();
    else
        return 0;
}

unsigned int ImageTrackerController::GetVectorCount()
{
    return this->vectorImages->GetImageCount();
}

/**
 * Updates the view of the image and vector visualizations.
 */
void ImageTrackerController::UpdateView()
{
//     std::string function("ImageTrackerController::UpdateView");
//     Logger::verbose << function << ": " << this->imagePipeline.size() << " filters in pipeline." << std::endl;
	int imgCount = this->GetImageCount();
	int vecCount = this->GetVectorCount();
	bool anyFiles = (imgCount > 0) || (vecCount > 0);

    if (anyFiles)	// only update if there are files to view
	{
		wxMutexLocker lock(ImageTrackerController::s_ControllerMutex);
		if (imgCount > 0)
		{
			this->imageVisualization->SetInput(this->imagePipeline.back()->GetOutput());
		}
	    
		if (vecCount > 0)
		{
			this->vectorVisualization->SetInput(this->vectorImages->GetOutput());
		}	

		// reset view
		if (this->resetCamera)
		{    
			this->GetRenderer()->ResetCamera();
			this->resetCamera = false;
		}

		// render (we need to ensure rendering does not happen before the 
		// render window is visible on Windows.  Updating only when files
		// are loaded ensures this.
		if (this->GetRenderWindow())
		{
			this->GetRenderWindow()->Render();
		}
    }
}

void ImageTrackerController::SaveViewImage(const std::string& fileName)
{
    this->GetRenderWindow()->Modified();
    this->capture->Modified();
    writer->SetFileName(fileName.c_str());
    writer->Update();
}

void ImageTrackerController::SetRenderWindow(vtkRenderWindow* rw)
{
    this->renderWindow = rw;
    this->renderWindow->AddRenderer(this->GetRenderer());
    this->capture->SetInput(this->renderWindow);
}

vtkRenderWindow* ImageTrackerController::GetRenderWindow()
{ 
    return this->renderWindow;
}

vtkRenderer* ImageTrackerController::GetRenderer()
{
    if (this->renderer == NULL)
        this->renderer = vtkRenderer::New();
        
    return this->renderer;
}

void ImageTrackerController::Render()
{
    this->GetRenderWindow()->Render();
}
