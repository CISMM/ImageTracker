#include "CLGOpticFlowPipeline.h"

#include "Logger.h"

CLGOpticFlowPipeline::CLGOpticFlowPipeline(void)
{
    this->input = NULL;
    this->flowFilter = FlowFilterType::New();
    this->smooth = SmoothFilterType::New();
}

void CLGOpticFlowPipeline::Update()
{
    std::string function("CLGOpticFlowPipeline::Update");
    Logger::debug << function << ": Checking parameters" << std::endl;
    if (!this->input || 
         this->input->size() < 2 ||
         this->GetOutputFiles().size() < 2)
    {
        Logger::warning << function << ": input and output not properly configured; aborting" << std::endl;
        return;
    }
    
    bool abort = this->NotifyProgress(0.0, "Initializing");
    
    Logger::debug << function << ": Setting up flow computation" << std::endl;
    int count = this->input->size() - 1;
    for (int i = 0; 
         i < count &&
         i < this->GetOutputFiles().size() &&
         !abort; i++)
    {
        this->flowFilter->SetInput1(dynamic_cast<ImageType*>(this->input->GetImage(i)));
        this->flowFilter->SetInput2(dynamic_cast<ImageType*>(this->input->GetImage(i+1)));
        WriteImage(this->flowFilter->GetOutput(), this->outputFiles[i]);
        bool abort = this->NotifyProgress((double) i / count, "Computing flow");
    }
    
    this->SetSuccess(!abort);
}

void CLGOpticFlowPipeline::SetSpatialSigma(double sigma) 
{ 
    this->flowFilter->SetSpatialSigma(sigma);
    
    // Discrete gaussian image filter takes variance--standard deviation squared
    this->smooth->SetVariance(sigma*sigma);
    this->smooth->Modified();
}
void CLGOpticFlowPipeline::SetRegularization(double reg) 
{ 
    this->flowFilter->SetRegularization(reg);
}
void CLGOpticFlowPipeline::SetRelaxation(double relax) 
{ 
    this->flowFilter->SetRelaxation(relax);
}
void CLGOpticFlowPipeline::SetIterations(unsigned int iter) 
{ 
    this->flowFilter->SetIterations(iter);
}
    
void CLGOpticFlowPipeline::SetInput(ReaderType input)
{
    this->input = input;
    this->smooth->SetInput(dynamic_cast<ImageTypeF2*>(this->input->GetImage(0)));
}

CLGOpticFlowPipeline::ImageType::Pointer CLGOpticFlowPipeline::GetPreviewImage()
{
    return this->smooth->GetOutput();
}
