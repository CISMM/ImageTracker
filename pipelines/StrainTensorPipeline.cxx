#include "StrainTensorPipeline.h"

#include "itkAdaptImageFilter.h"
#include "itkNthElementPixelAccessor.h"

#include "FileSet.h"
#include "GaussianGradientImageFilter.h"
#include "ImageSetReader.h"
#include "ImageUtils.h"
#include "Logger.h"

void StrainTensorPipeline::Update()
{
    std::string function("StrainTensorPipeline::Update()");
    
    typedef float ElementType;
    typedef itk::Vector< ElementType, 2 > VectorPixelType;
    typedef itk::Image< VectorPixelType, 2 > VectorImageType;
    typedef itk::Image< ElementType, 2 > ScalarImageType;

    typedef itk::AdaptImageFilter< VectorImageType, ScalarImageType, itk::NthElementPixelAccessor< ElementType, VectorPixelType > > ComponentExtractType;
    typedef GaussianGradientImageFilter< ScalarImageType > GradientType;

    if (this->GetStrainXFormat() == "" || 
        this->GetStrainYFormat() == "")
    {
        Logger::warning << function << ": Output file format(s) not set" << std::endl;
        return;
    }
    
    bool abort = this->NotifyProgress(0.0, "Initializing");
    
    Logger::verbose << function << ": Setting up strain pipeline" << std::endl;
    ComponentExtractType::Pointer component[2];
    GradientType::Pointer grad[2];
    
    for (int c = 0; c < 2; c++)
    {
        Logger::verbose << function << ": Component extractor: " << c << std::endl;
        component[c] = ComponentExtractType::New();
        component[c]->GetAccessor().SetElementNumber(c);
        
        Logger::verbose << function << ": Gradient filter: " << c << std::endl;
        grad[c] = GradientType::New();
        grad[c]->SetInput(component[c]->GetOutput());
        grad[c]->SetSigma(this->GetSigma());
    }

    int count = this->GetInput()->size();
    char file[120];
    
    for (unsigned int i = 0;
         i < count && !abort;
         i++)
    {
        component[0]->SetInput(dynamic_cast<VectorImageType*>(this->GetInput()->GetImage(i)));
        component[1]->SetInput(dynamic_cast<VectorImageType*>(this->GetInput()->GetImage(i)));
        
        sprintf(file, this->GetStrainXFormat().c_str(), i);
        WriteImage<GradientType::OutputImageType>(grad[0]->GetOutput(), std::string(file));
        sprintf(file, this->GetStrainYFormat().c_str(), i);
        WriteImage<GradientType::OutputImageType>(grad[1]->GetOutput(), std::string(file));
        abort = this->NotifyProgress(((double)(i+1)/count), "Computing strain");
    }
}
