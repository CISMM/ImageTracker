#include "ScalarImageVisualization.h"

#include "itkStatisticsImageFilter.h"

#include "ConnectVTKITK.h"
#include "ImageTrackerController.h"
#include "ScalarImageControlPanel.h"

ScalarImageVisualization::ScalarImageVisualization() :
    firstTime(true)
{
    this->window    = WindowType::New();
    this->exporter  = ExportType::New();
    this->importer  = vtkImageImport::New();
    this->flipper   = vtkImageFlip::New();
    this->rescaler  = vtkImageShiftScale::New();
    this->actor     = vtkImageActor::New();
    
    this->window->SetOutputMinimum(std::numeric_limits<DisplayImageType::PixelType>::min());
    this->window->SetOutputMaximum(std::numeric_limits<DisplayImageType::PixelType>::max());
    this->window->SetWindowMinimum(std::numeric_limits<DisplayImageType::PixelType>::min());
    this->window->SetWindowMaximum(std::numeric_limits<DisplayImageType::PixelType>::max());
    this->flipper->SetFilteredAxis(1); // flip the y axis.
    this->rescaler->SetOutputScalarTypeToUnsignedChar();
    
    this->exporter->SetInput(window->GetOutput());
    ConnectITKToVTK(this->exporter.GetPointer(), this->importer);
    this->flipper->SetInputConnection(this->importer->GetOutputPort());
    this->rescaler->SetInputConnection(this->flipper->GetOutputPort());
    this->actor->SetInput(this->rescaler->GetOutput());
    this->GetProps().push_back(this->actor);
}

ScalarImageVisualization::~ScalarImageVisualization()
{
    // TODO: Do we need to delete all elements of the pipeline?
    if (this->actor)
        this->actor->Delete();;
}

void ScalarImageVisualization::SetInput(itk::DataObject* input)
{
    InputImageType* image;
    if ((image = dynamic_cast<InputImageType*>(input)) == 0)
    {
        Logger::warning << "ScalarImageVisualization::SetInput: input object did not match expected type (InputImageType).  Pipeline *not* changed." << std::endl;
        return;
    }
    
    this->window->SetInput(image);
    
    // Adjust windowing to display first image optimally
    if (this->firstTime)
    {
        this->firstTime = false;
        
        typedef itk::StatisticsImageFilter< InputImageType > StatsType;
        StatsType::Pointer stats = StatsType::New();
        stats->SetInput(image);
        stats->Update();
        this->window->SetWindowMaximum(stats->GetMaximum());
        this->window->SetWindowMinimum(stats->GetMinimum());
    }
    this->Update();
}

ScalarImageVisualization::InputImageType::Pointer ScalarImageVisualization::GetInput()
{
    return const_cast< InputImageType* >(this->window->GetInput(0));
}

void ScalarImageVisualization::Update()
{
    // Why do we need to update all pipeline components?
    // I feel like this would defeat the purpose of having a pipeline architecture...
    // Yes, but if you try to remove any of these lines of code, suddenly your image
    // data doesn't get updated in the view panel.  sigh.
//     Logger::verbose << "ScalarImageVisualization::Update()" << std::endl;
    this->window->UpdateLargestPossibleRegion();
    this->exporter->UpdateLargestPossibleRegion();
    
    this->importer->SetDataExtentToWholeExtent();
    this->importer->UpdateWholeExtent();
    this->flipper->UpdateWholeExtent();
    this->rescaler->UpdateWholeExtent();
}

wxWindow* ScalarImageVisualization::CreateWxControl(wxWindow* parent)
{
    ScalarImageControlPanel* control = new ScalarImageControlPanel(parent, -1);
    control->SetPipeline(this);
    return control;
}

void ScalarImageVisualization::SetVisibility(int visible)
{
    this->actor->SetVisibility(visible);
    ImageTrackerController::Instance()->Render();
}

int ScalarImageVisualization::GetVisibility() const
{
    return this->actor->GetVisibility();
}

void ScalarImageVisualization::SetWindowMinimum(const InputPixelType min)
{
    this->window->SetWindowMinimum(min);
    this->Update();
    ImageTrackerController::Instance()->Render();
}

void ScalarImageVisualization::SetWindowMaximum(const InputPixelType max)
{
    this->window->SetWindowMaximum(max);
    this->Update();
    ImageTrackerController::Instance()->Render();
}

ScalarImageVisualization::InputPixelType ScalarImageVisualization::GetWindowMinimum() const
{
    return this->window->GetWindowMinimum();
}

ScalarImageVisualization::InputPixelType ScalarImageVisualization::GetWindowMaximum() const
{
    return this->window->GetWindowMaximum();
}
