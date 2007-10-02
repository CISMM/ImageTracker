#include "ImageTrackerController.h"

#include <typeinfo>

#include <wx/thread.h>

#include "FileSet.h"
#include "ImageUtils.h"
#include "GuiUtils.h"
#include "Logger.h"
#include "wxUtils.h"

wxMutex ImageTrackerController::s_ControllerMutex;
ImageTrackerController::Pointer ImageTrackerController::s_instance;

ImageTrackerController::Pointer ImageTrackerController::Instance()
{
    if (ImageTrackerController::s_instance.IsNull())
        ImageTrackerController::s_instance = ImageTrackerController::New();
    
    return ImageTrackerController::s_instance;
}

ImageTrackerController::ImageTrackerController() :
    frameIndex(0),
    dataIndex(0),
    datavis(),
    resetCamera(false),
    isDataChanged(false),
    isIndexChanged(false)
{
    this->renderer = NULL;
    this->renderWindow = NULL;
    this->parent = NULL;

    // Setup view saving pipeline
    this->capture = vtkWindowToImageFilter::New();
    this->writer = vtkTIFFWriter::New();
    this->writer->SetInputConnection(this->capture->GetOutputPort());
    this->writer->SetCompressionToNoCompression();
}
    
ImageTrackerController::~ImageTrackerController()
{
    // Delete our own vtk objects
    if (this->writer)
        this->writer->Delete();
    if (this->capture)
        this->capture->Delete();
    if (this->renderer)
        this->renderer->Delete();
    
    this->datavis.clear();
    
    // Other components should delete these:
    this->parent = NULL;
    this->renderWindow = NULL;
}

void ImageTrackerController::FreeResources()
{
    // There is a problem with deleting the objects related to this resource.
    // Because the visualization capture filter has a reference to the renderer
    // window, if the renderer window is deleted first, we get a seg fault for 
    // trying to delete an object with non-zero reference count.  
    // So, here we reset the input to the caputre object.  This method should 
    // be called before the renderer window is deleted.
    if (this->capture)
        this->capture->SetInput(NULL);
}

bool ImageTrackerController::IsDataChanged()
{
    wxMutexLocker lock(ImageTrackerController::s_ControllerMutex);
    return this->isDataChanged;
}

bool ImageTrackerController::IsIndexChanged()
{
    wxMutexLocker lock(ImageTrackerController::s_ControllerMutex);
    return this->isIndexChanged;
}

void ImageTrackerController::SetIsDataChanged(bool changed)
{
    wxMutexLocker lock(ImageTrackerController::s_ControllerMutex);
    this->isDataChanged = changed;
}

void ImageTrackerController::SetIsIndexChanged(bool changed)
{
    wxMutexLocker lock(ImageTrackerController::s_ControllerMutex);
    this->isIndexChanged = changed;
}

DataSource::Pointer ImageTrackerController::GetDataSource(unsigned int i)
{
    return i >= this->datavis.size() ? NULL : this->datavis[i].first;
}

ItkVtkPipeline::Pointer ImageTrackerController::GetVisualization(unsigned int i)
{
    return i >= this->datavis.size() ? NULL : this->datavis[i].second;
}

void ImageTrackerController::AddDataSource(DataSource::Pointer source)
{
    std::string function("ImageTrackerController::AddDataSource");
    Logger::verbose << function << ": generating visualization pipeline for data source" << std::endl;
    ItkVtkPipeline::Pointer view = GenerateVisualPipeline(source);
    
    Logger::verbose << function << ": adding data source and visualization to managed data-vis pairs" << std::endl;
    this->datavis.push_back(std::make_pair(source, view));
    if (view.IsNotNull())
    {
        Logger::verbose << function << ": adding visualization output to renderer" << std::endl;
        view->AddPropsTo(this->GetRenderer());
        this->resetCamera = true;
    }

    this->SetIsDataChanged(true);
    this->SetIsIndexChanged(true);
    Logger::verbose << function << ": done" << std::endl;
}

void ImageTrackerController::RemoveDataSource(unsigned int i)
{
    if (i < this->datavis.size())
    {
        // Remove actor from renderer
        this->datavis[i].second->RemovePropsFrom(this->GetRenderer());
        this->datavis.erase(this->datavis.begin() + i);
    }
    this->SetIsDataChanged(true);
    this->SetIsIndexChanged(true);
}

void ImageTrackerController::GetDataSourceNames(wxArrayString& names)
{
    names.clear();
    for (DataVisualList::iterator it = this->datavis.begin();
         it != this->datavis.end();
         ++it)
    {
        names.Add(std2wx(it->first->GetName()));
    } 
}

void ImageTrackerController::SetFrameIndex(unsigned int index)
{
    // Logger::verbose << "ImageTrackerController::SetIndex: " << index << std::endl;
    this->frameIndex = index;
    this->SetIsIndexChanged(true);
}

unsigned int ImageTrackerController::GetMaxSize()
{
    int max = 0;
    for (DataVisualList::iterator it = this->datavis.begin();
         it != this->datavis.end();
         ++it)
    {
        max = std::max(max, it->first->size());
    }
    return (unsigned int) max;
}

unsigned int ImageTrackerController::GetMaxIndex()
{
    return this->GetMaxSize() - 1;
}

void ImageTrackerController::SetParent(wxWindow* parent)
{
    this->parent = parent;
}

/**
 * Cycles through all of the datasource/visualization pairs in this controller, and
 * updates each.  Assigns the indexed datasource object to the appropriate ItkVtkPipeline.
 */
void ImageTrackerController::UpdateView()
{
    std::string function("ImageTrackerController::UpdateView");
    Logger::verbose << function << ": " << this->datavis.size() << " data/vis pairs" << std::endl;
    
    for (DataVisualList::size_type i = 0; i < this->datavis.size(); i++)
    {
        DataSource::Pointer data = this->datavis[i].first;
        ItkVtkPipeline::Pointer view = this->datavis[i].second;
        
        if (data.IsNull() || data->size() == 0)
        {
            Logger::info << function << ": datasource " << this->dataIndex << " is empty...skipping" << std::endl;
        }
        else if (view.IsNull())
        {
            Logger::info << function << ": visualization " << this->dataIndex << " is null...skipping" << std::endl;
        }
        else
        {
            Logger::debug << function << ": requesting file " << data->GetFiles()[this->frameIndex] << std::endl;
            view->SetInput(data->GetImage(this->frameIndex));
        }
    }
        
    // Render
    if (this->resetCamera)
    {    
        this->GetRenderer()->ResetCamera();
        this->resetCamera = false;
    }
    this->GetRenderWindow()->Render();
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
