#include "VectorGlyphVisualization.h"

#include "vtkArrowSource.h"
#include "vtkIndent.h"
#include "vtkPointData.h"
#include "vtkProperty.h"

#include "ConnectVTKITK.h"
#include "ImageTrackerController.h"
#include "Logger.h"
#include "VectorGlyphControlPanel.h"

VectorGlyphVisualization::VectorGlyphVisualization()
{
    std::string function("VectorGlyphVisualization::VectorGlyphVisualization");
    Logger::verbose << function << ": creating pipeline components" << std::endl;
    this->exporter      = ExportType::New();
    this->importer      = vtkImageImport::New();
    this->flip          = vtkImageFlip::New();
    this->surface       = vtkDataSetSurfaceFilter::New();
    this->calculator    = vtkArrayCalculator::New();
    this->mask          = vtkMaskPoints::New();
    this->glyph         = vtkGlyph3D::New();
    this->glyphSource   = vtkArrowSource::New();
    this->mapper        = vtkPolyDataMapper::New();
    this->actor         = vtkActor::New();
    
    Logger::verbose << function << ": configuring pipeline components" << std::endl;
    // flip the image data around the y axis...there is a difference in origin convention between itk and vtk.
    this->flip->SetFilteredAxis(1);
    
    // calculate a vector quantity from the input scalar vector components
    this->calculator->AddScalarVariable("u", "velocity", 0);
    this->calculator->AddScalarVariable("v", "velocity", 1);
    this->calculator->SetFunction("u*iHat - v*jHat");
    this->calculator->SetResultArrayName("2D Velocity");
    
    // mask out some of the data points to increase rendering speed and readability
    this->mask->SetOnRatio(137);
    this->mask->RandomModeOff();
    
    // create an arrow glyph at each data point
    this->glyph->SetSourceConnection(this->glyphSource->GetOutputPort());
    this->glyph->SetScaleModeToScaleByVector();
    this->glyph->SetScaleFactor(1.0);
    
    // Set the color properties of the glyph field
    this->mapper->ScalarVisibilityOff();
    this->actor->GetProperty()->SetColor(1.0, 1.0, 0.0);
    
    Logger::verbose << function << ": connecting pipeline components" << std::endl;
    ConnectITKToVTK(this->exporter.GetPointer(), this->importer);
    this->flip->SetInputConnection(this->importer->GetOutputPort());
    this->surface->SetInputConnection(this->flip->GetOutputPort());
    this->calculator->SetInputConnection(this->surface->GetOutputPort());
    this->mask->SetInputConnection(this->calculator->GetOutputPort());
    this->glyph->SetInputConnection(this->mask->GetOutputPort());
    this->mapper->SetInput(this->glyph->GetOutput());
    this->actor->SetMapper(this->mapper);
    this->GetProps().push_back(this->actor);
    Logger::verbose << function << ": done" << std::endl;
}

VectorGlyphVisualization::~VectorGlyphVisualization()
{
    // Do we need to delete all elements of the pipeline?
	// Yes, according to David Feng.
    if (this->actor)
        this->actor->Delete();;
	if (this->mapper)
		this->mapper->Delete();
	if (this->glyph)
		this->glyph->Delete();
	if (this->mask)
		this->mask->Delete();
	if (this->calculator)
		this->calculator->Delete();
	if (this->surface)
		this->surface->Delete();
	if (this->flip)
		this->flip->Delete();
	if (this->importer)
		this->importer->Delete();
}

void VectorGlyphVisualization::SetInput(itk::DataObject* input)
{
    std::string function("VectorGlyphVisualization::SetInput");
    
//     Logger::verbose << function << ": checking input image type" << std::endl;
    InputImageType* image;
    if ((image = dynamic_cast<InputImageType*>(input)) == 0)
    {
        Logger::warning << function << ": input object did not match expected type (InputImageType).  Pipeline *not* changed." << std::endl;
        return;
    }
    
//     Logger::verbose << function << ": setting image exporter input" << std::endl;
    this->exporter->SetInput(image);
    this->importer->Update();
    
//     Logger::verbose << function << ": image information" << std::endl;
    if (this->importer->GetOutput() != NULL)
    {
        // this->importer->GetOutput()->PrintSelf(std::cout, vtkIndent());
//         Logger::verbose << function << ": setting point data scalar name" << std::endl;
        this->importer->GetOutput()->GetPointData()->GetScalars()->SetName("velocity");
        // this->importer->GetOutput()->PrintSelf(std::cout, vtkIndent());
    }
    
//     Logger::verbose << function << ": updating pipeline" << std::endl;
    this->Update();
}

void VectorGlyphVisualization::Update()
{
    std::string function("VectorGlyphVisualization::Update");
    // TODO: Do we need to update all pipeline components?
    // I feel like this would defeat the purpose of having a pipeline architecture...
//     Logger::verbose << function << ": updating image exporter (itk) and importer (vtk)" << std::endl;
    this->exporter->UpdateLargestPossibleRegion();
    this->importer->SetDataExtentToWholeExtent();
    this->importer->UpdateWholeExtent();
    
//     Logger::verbose << function << ": updating vtkGlyph3D" << std::endl;
    this->glyph->Update();
}

wxWindow* VectorGlyphVisualization::CreateWxControl(wxWindow* parent)
{
    VectorGlyphControlPanel* control = new VectorGlyphControlPanel(parent, -1);
    control->SetPipeline(this);
    return control;
}

void VectorGlyphVisualization::SetMaskRatio(int ratio) 
{ 
    this->mask->SetOnRatio(ratio); 
    this->mask->Modified();
    ImageTrackerController::Instance()->Render();
}
int VectorGlyphVisualization::GetMaskRatio() 
{ 
    return this->mask->GetOnRatio(); 
}

void VectorGlyphVisualization::SetScaleFactor(double factor) 
{
    this->glyph->SetScaleFactor(factor);
    this->glyph->Modified();
    ImageTrackerController::Instance()->Render();
}
double VectorGlyphVisualization::GetScaleFactor() 
{
    return this->glyph->GetScaleFactor();
}

void VectorGlyphVisualization::SetVisibility(int visible) 
{
    this->actor->SetVisibility(visible);
    ImageTrackerController::Instance()->Render();
}
int VectorGlyphVisualization::GetVisibility()
{
    return this->actor->GetVisibility();
}
