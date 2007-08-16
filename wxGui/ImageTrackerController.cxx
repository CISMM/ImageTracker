#include "ImageTrackerController.h"

#include <typeinfo>

#include <wx/thread.h>

#include "ImageUtils.h"
#include "GuiUtils.h"
#include "Logger.h"
#include "wxUtils.h"

wxMutex ImageTrackerController::s_ControllerMutex;

ImageTrackerController::ImageTrackerController() :
    frameIndex(0),
    dataIndex(0),
    datavis(),
    resetCamera(false),
    isControllerChanged(false)
{
    this->renderer = NULL;
    this->renderWindow = NULL;
    this->parent = NULL;
}
    
ImageTrackerController::~ImageTrackerController()
{
    this->datavis.clear();
    if (this->renderer)
        this->renderer->Delete();
    
    // Other components should delete these:
    this->parent = NULL;
    this->renderWindow = NULL;
}

bool ImageTrackerController::IsControllerChanged()
{
    wxMutexLocker lock(ImageTrackerController::s_ControllerMutex);
    return this->isControllerChanged;
}

void ImageTrackerController::SetIsControllerChanged(bool changed)
{
    wxMutexLocker lock(ImageTrackerController::s_ControllerMutex);
    this->isControllerChanged = changed;
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
        this->GetRenderer()->AddActor(view->GetOutput());
        this->resetCamera = true;
    }

    this->SetIsControllerChanged(true);
    Logger::verbose << function << ": done" << std::endl;
}

void ImageTrackerController::RemoveDataSource(unsigned int i)
{
    if (i < this->datavis.size())
    {
        // Remove actor from renderer
        this->GetRenderer()->RemoveActor(this->datavis[i].second->GetOutput());
        this->datavis.erase(this->datavis.begin() + i);
    }
    this->SetIsControllerChanged(true);
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
    this->SetIsControllerChanged(true);
}

void ImageTrackerController::SetDataIndex(unsigned int index)
{
    index = std::min(index, this->datavis.size()-1);
    this->dataIndex = index;
    this->SetIsControllerChanged(true);
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
    this->renderWindow->Render();
}

