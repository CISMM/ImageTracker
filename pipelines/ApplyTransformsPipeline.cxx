#include "ApplyTransformsPipeline.h"

#include <vector>

#include "itkCastImageFilter.h"
#include "itkResampleImageFilter.h"
#include "itkCenteredRigid2DTransform.h"

#include "ImageUtils.h"
#include "Logger.h"
#include "MultiResolutionRegistrationPipeline.h"
#include "TransformGroup.h"

void ApplyTransformsPipeline::Update()
{
    std::string function("ApplyTransformsPipeline::Update");
    Logger::verbose << function << ": Checking parameters" << std::endl;
    if (this->input == NULL ||
        this->input->size() < 1 ||
        this->outputFiles.size() < 1 ||
        this->m_TransformFile == "")
    {
        Logger::warning << function << ": input and output not properly configured; aborting" << std::endl;
        return;
    }
    
    typedef MultiResolutionRegistrationPipeline::TransformType TransformType;
    typedef std::vector< TransformType::Pointer > TransformVector;
    
    bool abort = this->NotifyProgress(0.0, "Loading transforms");
    
    Logger::verbose << function << ": Loading the transforms from transform file" << std::endl;
    TransformVector *pTransforms = TransformGroup::LoadTransforms(this->m_TransformFile);
    if (pTransforms == NULL ||
        pTransforms->size() < 1)
    {
        Logger::warning << function << ": tranform file " << this->m_TransformFile << " contains no transforms or is poorly formatted; aborting" << std::endl;
        return;
    }
    
    // Declare transform pipeline classes
    typedef itk::ResampleImageFilter< InternalImageType, InternalImageType > ResampleType;
    typedef itk::CastImageFilter< InternalImageType, WriteImageType > CastType;
    
    Logger::verbose << function << ": Setting up transform classes" << std::endl;
    ResampleType::Pointer resample = ResampleType::New();
    CastType::Pointer cast = CastType::New();
    cast->SetInput(resample->GetOutput());
    
    Logger::verbose << function << ": Using first image to set resampling parameters" << std::endl;
    InternalImageType::Pointer image(dynamic_cast< InternalImageType* >(this->input->GetImage(0)));
    resample->SetSize(image->GetLargestPossibleRegion().GetSize());
    resample->SetOutputOrigin(image->GetOrigin());
    resample->SetOutputSpacing(image->GetSpacing());
    resample->SetDefaultPixelValue(0);
    
    Logger::verbose << function << ": Creating base identity transform" << std::endl;
    TransformType::Pointer transform = TransformType::New();
    transform->SetIdentity();
    
    // Iterate through all transforms, making sure not to overrun either our input images or
    // output file names
    int index = 0;
    int count = this->input->size();
    
    for (TransformVector::iterator vecIt = pTransforms->begin();
         vecIt != pTransforms->end() && 
                 index < this->input->size() && 
                 index < this->outputFiles.size() &&
                 !abort;
         ++vecIt, 
         index++)
    {
        // Precompose the current transform with the previous one
        transform->Compose(*vecIt, true);
        
        // Set the resample inputs
        resample->SetInput(dynamic_cast< InternalImageType* >(this->input->GetImage(index)));
        resample->SetTransform(transform);
        
        // Write the image
        WriteImage(cast->GetOutput(), this->outputFiles[index]);
        abort = this->NotifyProgress(((double) (index+1)/count));
    }
    
    Logger::verbose << function << ": Cleaning up dumb pointers" << std::endl;
    delete (pTransforms);
}
