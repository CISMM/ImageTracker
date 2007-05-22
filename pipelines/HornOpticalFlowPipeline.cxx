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
         this->input->size() < 2 ||
         this->GetOutputFiles().size() < 2)
    {
        Logger::warning << function << ": input and output not properly configured; aborting" << std::endl;
        return;
    }
    
    bool abort = this->Notify(0.0, "Initializing");
    
    Logger::debug << function << ": Setting up flow computation" << std::endl;
    typedef HornOpticalFlowImageFilter< InternalImageType, InternalImageType, float > FlowFilter;
    FlowFilter::Pointer flow = FlowFilter::New();
    flow->SetIterations(this->GetIterations());
    flow->SetSpatialSigma(this->GetSpatialSigma());
    flow->SetSmoothWeighting(this->GetSmoothWeighting());
    
    unsigned int count = this->input->size() - 1;
    
    Logger::debug << function << ": Computing flow" << std::endl;
    for (unsigned int i = 0; 
         i < this->input->size()-1 &&
         i < this->GetOutputFiles().size() &&
         !abort;
         i++)
    {
        flow->SetInput1(dynamic_cast< InternalImageType* >(this->input->GetImage(i)));
        flow->SetInput2(dynamic_cast< InternalImageType* >(this->input->GetImage(i+1)));
        flow->Update();
        WriteImage(flow->GetOutput(), this->GetOutputFiles()[i]);
        abort = this->Notify(((double)(i+1)/count), "Computing flow");
    }
}
