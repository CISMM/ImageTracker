#include "VectorHeightMapVisualization.h"

#include "vtkPointData.h"

#include "ConnectVTKITK.h"
#include "ImageTrackerController.h"
#include "Logger.h"
#include "VectorHeightMapControlPanel.h"

const std::string VectorHeightMapVisualization::VECTOR_MAGNITUDE("sqrt(u^2 + v^2)");
const std::string VectorHeightMapVisualization::X_COMPONENT("u");
const std::string VectorHeightMapVisualization::Y_COMPONENT("v");

VectorHeightMapVisualization::VectorHeightMapVisualization()
	: initialized(false)
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
    this->SetScaleFactor(1.0);
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

VectorHeightMapVisualization::~VectorHeightMapVisualization()
{
    if (this->actor)
        this->actor->Delete();
	if (this->mapper)
		this->mapper->Delete();
	if (this->normals)
		this->normals->Delete();
	if (this->warp)
		this->warp->Delete();
	if (this->calculator)
		this->calculator->Delete();
	if (this->surface)
		this->surface->Delete();
	if (this->flip)
		this->flip->Delete();
	if (this->importer)
		this->importer->Delete();
}

void VectorHeightMapVisualization::SetInput(itk::DataObject* input)
{
    std::string function("VectorHeightMapVisualization::SetInput");
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
    
	this->initialized = true;
    this->Update();
}

void VectorHeightMapVisualization::Update()
{
    this->exporter->UpdateLargestPossibleRegion();
    this->importer->SetDataExtentToWholeExtent();
    this->importer->UpdateWholeExtent();
    this->normals->Update();
}

wxWindow* VectorHeightMapVisualization::CreateWxControl(wxWindow* parent)
{
    VectorHeightMapControlPanel* control = new VectorHeightMapControlPanel(parent, -1);
    control->SetPipeline(this);
    return control;
}

void VectorHeightMapVisualization::SetScaleFactor(double scale)
{
    this->warp->SetScaleFactor(scale);
    this->warp->Modified();
	if (this->initialized)
	    ImageTrackerController::Instance()->Render();
}

double VectorHeightMapVisualization::GetScaleFactor()
{
    return this->warp->GetScaleFactor();
}

void VectorHeightMapVisualization::SetMagnitudeFunction(Function function)
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
	if (this->initialized)
        ImageTrackerController::Instance()->Render();
}

VectorHeightMapVisualization::Function VectorHeightMapVisualization::GetMagnitudeFunction()
{
    return this->function;
}

void VectorHeightMapVisualization::SetColorMapType(ColorMap type)
{
    this->colorMapType = type;
    double minmax[2] = {0,0};
    this->normals->GetOutput()->GetScalarRange(minmax);
    this->mapper->SetLookupTable(MakeLookupTable(this->colorMapType, minmax[0], minmax[1]));
    
    this->mapper->Modified();
	if (this->initialized)
	    ImageTrackerController::Instance()->Render();
}

ColorMap VectorHeightMapVisualization::GetColorMapType()
{
    return this->colorMapType;
}

void VectorHeightMapVisualization::SetVisibility(int visible) 
{
    this->actor->SetVisibility(visible);
    ImageTrackerController::Instance()->Render();
}

int VectorHeightMapVisualization::GetVisibility()
{
    return this->actor->GetVisibility();
}
