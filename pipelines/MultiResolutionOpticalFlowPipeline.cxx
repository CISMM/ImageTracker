#include "MultiResolutionOpticalFlowPipeline.h"

#include <string>

#include "CLGOpticFlowImageFilter.h"
#include "HornOpticalFlowImageFilter.h"
#include "ImageUtils.h"
#include "MultiResolutionOpticalFlowMethod.h"
#include "Logger.h"

MultiResolutionOpticalFlowPipeline::MultiResolutionOpticalFlowPipeline()
    : m_Iterations(200),
    m_SpatialSigma(1.0),
    m_IntegrationSigma(4.0),
    m_Regularization(10e4),
    m_Relaxation(1.9),
    m_NumberOfLevels(1)
{
    this->harris = HarrisType::New();
}

void MultiResolutionOpticalFlowPipeline::SetInput(ImageFileSet* input)
{
    this->input = input;
    this->harris->SetInput(input->GetOutput());
}

void MultiResolutionOpticalFlowPipeline::SetSpatialSigma(float sigma)
{
    this->m_SpatialSigma = sigma;
    this->harris->SetDerivativeSigma(sigma);
}

void MultiResolutionOpticalFlowPipeline::SetIntegrationSigma(float sigma)
{
    this->m_IntegrationSigma = sigma;
    this->harris->SetIntegrationSigma(sigma);
}

MultiResolutionOpticalFlowPipeline::ImageType::Pointer
MultiResolutionOpticalFlowPipeline::GetPreviewImage()
{
    return this->harris->GetOutput();
}

void MultiResolutionOpticalFlowPipeline::Update()
{
    std::string function("MultiResolutionOpticalFlowPipeline::Update");
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
    
    typedef CLGOpticFlowImageFilter< ImageType, ImageType, float > FlowType;
//     typedef HornOpticalFlowImageFilter< ImageType, ImageType, float > FlowType;
    typedef MultiResolutionOpticalFlowMethod< ImageType, ImageType > MRFlowType;
    
    FlowType::Pointer method = FlowType::New();
    method->SetIterations(this->GetIterations());
    method->SetSpatialSigma(this->GetSpatialSigma());
    method->SetIntegrationSigma(this->GetIntegrationSigma());
    method->SetRegularization(this->GetRegularization());
//     method->SetRelaxation(this->GetRelaxation());
    
    MRFlowType::Pointer flow = MRFlowType::New();
    flow->SetNumberOfLevels(this->GetNumberOfLevels());
    flow->SetOpticalFlow(method);
    
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
