#include "ImageTrackerController.h"

#include <typeinfo>

#include "ImageUtils.h"
#include "GuiUtils.h"
#include "Logger.h"
#include "wxUtils.h"

ImageTrackerController::ImageTrackerController() :
    index(0),
    datavis(),
    resetCamera(false),
    wxId(-1),
    isDataSourceChanged(false)
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

void ImageTrackerController::AddDataSource(DataSource::Pointer source)
{
    ItkVtkPipeline::Pointer view = GenerateVisualPipeline(source);
    this->datavis.push_back(std::make_pair(source, view));
    if (view.IsNotNull())
    {
        this->GetRenderer()->AddActor(view->GetOutput());
        this->resetCamera = true;
    }

    this->UpdateView();
    // this->OnDataSourceChange();
    this->SetIsDataSourceChanged(true);
}

void ImageTrackerController::RemoveDataSource(unsigned int i)
{
    if (i < this->datavis.size())
    {
        // Remove actor from renderer
        this->GetRenderer()->RemoveActor(this->datavis[i].second->GetOutput());
        this->datavis.erase(this->datavis.begin() + i);
    }
    this->UpdateView();
    // this->OnDataSourceChange();
    this->SetIsDataSourceChanged(true);
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

void ImageTrackerController::SetIndex(unsigned int index)
{
    // Logger::verbose << "ImageTrackerController::SetIndex: " << index << std::endl;
    this->index = index;
    this->UpdateView();
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

void ImageTrackerController::SetParent(wxWindow* parent, int id)
{
    this->parent = parent;
    this->wxId = id;
}

void ImageTrackerController::OnDataSourceChange()
{
    if (this->parent)
    {
        wxCommandEvent evt(wxEVT_COMMAND_ENTER, wxId);
        this->parent->GetEventHandler()->ProcessEvent(evt);
    }
}

/**
 * Cycles through all of the datasource/visualization pairs in this controller, and
 * updates each.  Assigns the indexed datasource object to the appropriate ItkVtkPipeline.
 */
void ImageTrackerController::UpdateView()
{
    Logger::verbose << "ImageTrackerController::UpdateViewPipelines: " << this->datavis.size() << " data/vis pairs" << std::endl;
    for (unsigned int i = 0;
         i < this->datavis.size();
         i++)
    {
        DataSource::Pointer data = this->datavis[i].first;
        ItkVtkPipeline::Pointer view = this->datavis[i].second;
        
        if (data.IsNull() || data->size() == 0) // no objects in the datasource
        {
            Logger::info << "ImageTrackerController::UpdateViewPipelines: datasource " << i << " is empty...skipping" << std::endl;
        }
        else if (view.IsNull())                      // no valid visualization
        {
            Logger::info << "ImageTrackerController::UpdateViewPipelines: visualization " << i << " is null...skipping" << std::endl;
        }
        else                                    // should be able to render this
        {
            Logger::debug << "ImageTrackerController::UpdateViewPipelines: requesting file " << data->GetFiles()[this->index] << std::endl;
            view->SetInput(data->GetImage(this->index));
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

