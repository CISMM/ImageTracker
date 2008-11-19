#include "StrainTensorPipeline.h"

#include "FileSet.h"
#include "ImageSetReader.h"
#include "ImageUtils.h"
#include "Logger.h"
#include "StrainTensorImageFilter.h"

void StrainTensorPipeline::Update()
{
    std::string function("StrainTensorPipeline::Update()");
    
    typedef float ElementType;
    typedef itk::Vector< ElementType, 2 > VectorPixelType;
    typedef itk::Image< VectorPixelType, 2 > VectorImageType;
    
    typedef StrainTensorImageFilter< VectorImageType > StrainType;
    
    bool abort = this->NotifyProgress(0.0, "Initializing");
    
    Logger::verbose << function << ": Setting up strain pipeline" << std::endl;
    StrainType::Pointer strain = StrainType::New();
    strain->SetSigma(this->GetSigma());

    int count = this->GetInput()->GetImageCount();
    char file[120];
    
    for (unsigned int i = 0;
         i < count && !abort;
         i++)
    {
        strain->SetInput(dynamic_cast<VectorImageType*>(this->GetInput()->GetImage(i)));
        
        WriteImage<StrainType::OutputImageType>(strain->GetOutput(), this->GetOutputFiles()[i]);
        abort = this->NotifyProgress(((double)(i+1)/count), "Computing strain");
    }
}
