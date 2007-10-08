#include "VectorHeightMapPipeline.h"

#include "vtkPointData.h"

#include "ConnectVTKITK.h"
#include "ImageTrackerController.h"
#include "Logger.h"
#include "VectorHeightMapControlPanel.h"

const std::string VectorHeightMapPipeline::VECTOR_MAGNITUDE("sqrt(u^2 + v^2)");
const std::string VectorHeightMapPipeline::X_COMPONENT("u");
const std::string VectorHeightMapPipeline::Y_COMPONENT("v");

VectorHeightMapPipeline::VectorHeightMapPipeline()
{
    // Create pipeline components
    this->exporter      = ExportType::New();
    this->importer      = vtkImageImport::New();
    this->flip          = vtkImageFlip::New();
    this->surface       = vtkDataSetSurfaceFilter::New();
    this->calculator    = vtkArrayCalculator::New();
    this->warp          = vtkWarpScalar::New();
    this->normals       = vtkPolyDataNormals::New();
    this->mapper        = vtkPolyDataMapper::New();
    this->actor         = vtkActor::New();
    
    // Set pipeline properties
    this->flip->SetFilteredAxis(1);
    
    this->calculator->AddScalarVariable("u", "vector", 0);
    this->calculator->AddScalarVariable("v", "vector", 1);
    this->calculator->SetResultArrayName("magnitude");
    this->SetMagnitudeFunction(VectorMagnitude);
    
    this->SetScaleFactor(10.0);
    
    this->normals->SetFeatureAngle(180.0);
    
    this->SetColorMapType(IsoGray);
    
    // Connect pipeline components
    ConnectITKToVTK(this->exporter.GetPointer(), this->importer);
    this->flip->SetInputConnection(this->importer->GetOutputPort());
    this->surface->SetInputConnection(this->flip->GetOutputPort());
    this->calculator->SetInputConnection(this->surface->GetOutputPort());
    this->warp->SetInputConnection(this->calculator->GetOutputPort());
    this->normals->SetInputConnection(this->warp->GetOutputPort());
    this->mapper->SetInput(this->normals->GetOutput());
    this->actor->SetMapper(this->mapper);
    this->GetProps().push_back(this->actor);
}

VectorHeightMapPipeline::~VectorHeightMapPipeline()
{
    if (this->actor)
        this->actor->Delete();
}

void VectorHeightMapPipeline::SetInput(itk::DataObject* input)
{
    std::string function("VectorHeightMapPipeline::SetInput");
    InputImageType* image;
    if ((image = dynamic_cast<InputImageType*>(input)) == 0)
    {
        Logger::warning << function << ": input object did not match expected type (InputImageType*). Pipeline *not* changed." << std::endl;
        return;
    }
    
    this->exporter->SetInput(image);
    this->importer->Update();
    if (this->importer->GetOutput() != NULL)
    {
        this->importer->GetOutput()->GetPointData()->GetScalars()->SetName("vector");
    }
    
    this->Update();
}

void VectorHeightMapPipeline::Update()
{
    this->exporter->UpdateLargestPossibleRegion();
    this->importer->SetDataExtentToWholeExtent();
    this->importer->UpdateWholeExtent();
    this->normals->Update();
}

wxWindow* VectorHeightMapPipeline::CreateWxControl(wxWindow* parent)
{
    VectorHeightMapControlPanel* control = new VectorHeightMapControlPanel(parent, -1);
    control->SetPipeline(this);
    return control;
}

void VectorHeightMapPipeline::SetScaleFactor(double scale)
{
    this->warp->SetScaleFactor(scale);
    this->warp->Modified();
    ImageTrackerController::Instance()->Render();
}

double VectorHeightMapPipeline::GetScaleFactor()
{
    return this->warp->GetScaleFactor();
}

void VectorHeightMapPipeline::SetMagnitudeFunction(Function function)
{
    this->function = function;
    switch (this->function)
    {
        case XComponent:
            this->calculator->SetFunction(X_COMPONENT.c_str());
            break;
        case YComponent:
            this->calculator->SetFunction(Y_COMPONENT.c_str());
            break;
        default:
            this->calculator->SetFunction(VECTOR_MAGNITUDE.c_str());
            break;
    }
    this->calculator->Modified();
    ImageTrackerController::Instance()->Render();
}

VectorHeightMapPipeline::Function VectorHeightMapPipeline::GetMagnitudeFunction()
{
    return this->function;
}

void VectorHeightMapPipeline::SetColorMapType(ColorMap type)
{
    this->colorMapType = type;
    double minmax[2] = {0,0};
    this->normals->GetOutput()->GetScalarRange(minmax);
    this->mapper->SetLookupTable(MakeLookupTable(this->colorMapType, minmax[0], minmax[1]));
    
    this->mapper->Modified();
    ImageTrackerController::Instance()->Render();
}

ColorMap VectorHeightMapPipeline::GetColorMapType()
{
    return this->colorMapType;
}
