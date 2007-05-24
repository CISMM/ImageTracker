#include "CLGOpticFlowPipeline.h"

#include "Logger.h"

CLGOpticFlowPipeline::CLGOpticFlowPipeline(void) :
    init(false),
    index(0)
{
    this->input = NULL;
    this->flowFilter = FlowFilterType::New();
    this->smooth = SmoothFilterType::New();
}

void CLGOpticFlowPipeline::InitializePipeline()
{
    if (this->input && 
         this->input->size() != 0 && 
         this->outputFiles.size() != 0)
    {
        this->index = 0;
        this->init = true;
    }
    else
    {
        Logger::logWarn("CLGOpticFlowPipeline: Source and/or destination files have not been setup.");
        this->init = false;
    }
}

bool CLGOpticFlowPipeline::UpdateOne()
{
    if (!this->init)
    {
        this->InitializePipeline();
    }

    if (this->init &&
        this->index < (this->input->size() - 1) &&
        this->index < this->outputFiles.size())
    {
        this->flowFilter->SetInput1(dynamic_cast<ImageType*>(this->input->GetImage(this->index)));
        this->flowFilter->SetInput2(dynamic_cast<ImageType*>(this->input->GetImage(this->index+1)));

        Logger::logInfo("Writing flow field image: " + this->outputFiles[this->index]);
        WriteImage(this->flowFilter->GetOutput(), this->outputFiles[this->index]);

        this->index++;
        bool abort = this->NotifyProgress(((double) this->index) / (this->input->size()-1));
        return !abort;
    }

    this->init = false;
    return false; // No frames to process.
}

void CLGOpticFlowPipeline::UpdateAll()
{
    this->NotifyProgress(0.0, "Computing flow...this may take some time.");
    while (this->UpdateOne());
}

void CLGOpticFlowPipeline::Update()
{
    this->UpdateAll();
}

void CLGOpticFlowPipeline::SetSpatialSigma(double sigma) 
{ 
    this->flowFilter->SetSpatialSigma(sigma);
    
    // Discrete gaussian image filter takes variance--standard deviation squared
    this->smooth->SetVariance(sigma*sigma);
    this->smooth->Modified();
    
    this->init = false;
}
void CLGOpticFlowPipeline::SetRegularization(double reg) 
{ 
    this->flowFilter->SetRegularization(reg);
    this->init = false;
}
void CLGOpticFlowPipeline::SetRelaxation(double relax) 
{ 
    this->flowFilter->SetRelaxation(relax);
    this->init = false;
}
void CLGOpticFlowPipeline::SetIterations(unsigned int iter) 
{ 
    this->flowFilter->SetIterations(iter);
    this->init = false;
}
    
void CLGOpticFlowPipeline::SetInput(ReaderType input)
{
    this->input = input;
    this->smooth->SetInput(dynamic_cast<ImageTypeF2*>(this->input->GetImage(0)));
    this->init = false;
}
    
void CLGOpticFlowPipeline::SetOutputFiles(const FileSet& dest)
{
    this->outputFiles = dest;
    this->init = false;
}

CLGOpticFlowPipeline::ImageType::Pointer CLGOpticFlowPipeline::GetPreviewImage()
{
    return this->smooth->GetOutput();
}
