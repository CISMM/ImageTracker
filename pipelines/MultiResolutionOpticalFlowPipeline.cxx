#include "MultiResolutionOpticalFlowPipeline.h"

#include <string>

#include "CLGOpticFlowImageFilter.h"
#include "ImageUtils.h"
#include "MultiResolutionOpticalFlowMethod.h"
#include "Logger.h"

void MultiResolutionOpticalFlowPipeline::Update()
{
    std::string function("MultiResolutionOpticalFlowPipeline::Update");
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
    
    typedef CLGOpticFlowImageFilter< InternalImageType, InternalImageType, float > FlowType;
    typedef MultiResolutionOpticalFlowMethod< InternalImageType, InternalImageType > MRFlowType;
    FlowType::Pointer method = FlowType::New();
    method->SetIterations(this->GetIterations());
    method->SetSpatialSigma(this->GetSpatialSigma());
    method->SetRegularization(this->GetSmoothWeighting());
    method->SetRelaxation(this->GetRelaxation());
    
    MRFlowType::Pointer flow = MRFlowType::New();
    flow->SetNumberOfLevels(this->GetNumberOfLevels());
    flow->SetOpticalFlow(method);
    
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
        abort = this->NotifyProgress(((double)(i+1)/count), "Computing flow");
    }
}
