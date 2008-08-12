#include "HornOpticalFlowPipeline.h"

#include <string>

#include "HornOpticalFlowImageFilter.h"
#include "ImageUtils.h"
#include "Logger.h"

void HornOpticalFlowPipeline::Update()
{
    std::string function("HornOpticalFlowPipeline::Update");
    Logger::debug << function << ": Checking parameters" << std::endl;
    if (!this->input || 
         this->input->GetImageCount() < 2 ||
         this->GetOutputFiles().size() < 2)
    {
        Logger::warning << function << ": input and output not properly configured; aborting" << std::endl;
        return;
    }
    
    bool abort = this->NotifyProgress(0.0, "Initializing");
    
    Logger::debug << function << ": Setting up flow computation" << std::endl;
    typedef HornOpticalFlowImageFilter< ImageType, ImageType, float > FlowFilter;
    FlowFilter::Pointer flow = FlowFilter::New();
    flow->SetIterations(this->GetIterations());
    flow->SetSpatialSigma(this->GetSpatialSigma());
    flow->SetSmoothWeighting(this->GetSmoothWeighting());
    
    unsigned int count = this->input->GetImageCount() - 1;
    
    Logger::debug << function << ": Computing flow" << std::endl;
    for (unsigned int i = 0; 
         i < count &&
         i < this->GetOutputFiles().size() &&
         !abort;
         i++)
    {
        flow->SetInput1(CopyImage(this->input->GetImage(i)));
        flow->SetInput2(this->input->GetImage(i+1));
        flow->Update();
        WriteImage(flow->GetOutput(), this->GetOutputFiles()[i]);
        abort = this->NotifyProgress(((double)(i+1)/count), "Computing flow");
    }
}
