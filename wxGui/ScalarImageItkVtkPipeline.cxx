#include "ScalarImageItkVtkPipeline.h"
#include "ConnectVTKITK.h"

ScalarImageItkVtkPipeline::ScalarImageItkVtkPipeline()
{
    this->normalize = RescaleType::New();
    this->exporter  = ExportType::New();
    this->importer  = vtkImageImport::New();
    this->flipper   = vtkImageFlip::New();
    this->rescaler  = vtkImageShiftScale::New();
    this->actor     = vtkImageActor::New();
    
    this->normalize->SetOutputMinimum(std::numeric_limits<DisplayImageType::PixelType>::min());
    this->normalize->SetOutputMaximum(std::numeric_limits<DisplayImageType::PixelType>::max());
    this->flipper->SetFilteredAxis(1); // flip the y axis.
    this->rescaler->SetOutputScalarTypeToUnsignedChar();
    
    this->exporter->SetInput(normalize->GetOutput());
    ConnectITKToVTK(this->exporter.GetPointer(), this->importer);
    this->flipper->SetInputConnection(this->importer->GetOutputPort());
    this->rescaler->SetInputConnection(this->flipper->GetOutputPort());
    this->actor->SetInput(this->rescaler->GetOutput());
}

ScalarImageItkVtkPipeline::~ScalarImageItkVtkPipeline()
{
    // TODO: Do we need to delete all elements of the pipeline?
    if (this->actor)
        this->actor->Delete();;
}

void ScalarImageItkVtkPipeline::SetInput(itk::DataObject* input)
{
    InputImageType* image;
    if ((image = dynamic_cast<InputImageType*>(input)) == 0)
    {
        Logger::warning << "ScalarImageItkVtkPipeline::SetInput: input object did not match expected type (InputImageType).  Pipeline *not* changed." << std::endl;
        return;
    }
    
    this->normalize->SetInput(image);
    this->Update();
}

void ScalarImageItkVtkPipeline::Update()
{
    // TODO: Do we need to update all pipeline components?
    // I feel like this would defeat the purpose of having a pipeline architecture...
    Logger::verbose << "ScalarImageItkVtkPipeline::Update()" << std::endl;
    this->normalize->UpdateLargestPossibleRegion();
    this->exporter->UpdateLargestPossibleRegion();
    
    // this->importer->InvokePipelineModifiedCallbacks();
    // this->importer->InvokeUpdateInformationCallbacks();
    // this->importer->InvokeExecuteInformationCallbacks();
    // this->importer->InvokeExecuteDataCallbacks();
    
    this->importer->SetDataExtentToWholeExtent();
    this->importer->UpdateWholeExtent();
    this->flipper->UpdateWholeExtent();
    this->rescaler->UpdateWholeExtent();
}

vtkProp* ScalarImageItkVtkPipeline::GetOutput()
{
    return this->actor;
}
