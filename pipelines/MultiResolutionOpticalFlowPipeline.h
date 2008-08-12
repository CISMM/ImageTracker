#pragma once

#include "itkImage.h"
#include "itkObject.h"
#include "itkVector.h"

#include "CommonTypes.h"
#include "HarrisFeatureInterestImageFilter.h"
#include "ItkImagePipeline.h"

class MultiResolutionOpticalFlowPipeline :
    public ItkImagePipeline
{
public:
    // Common itk typedefs
    typedef MultiResolutionOpticalFlowPipeline Self;
    typedef ItkImagePipeline Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    
    itkNewMacro(Self);
    itkTypeMacro(MultiResolutionOpticalFlowPipeline, ItkImagePipeline);
    
    // Image typedefs
    typedef CommonTypes::InternalImageType ImageType;
    typedef itk::Image< 
            itk::Vector< float, ImageType::ImageDimension >, 
            ImageType::ImageDimension > FlowImageType;
    typedef HarrisFeatureInterestImageFilter< ImageType, ImageType > HarrisType;
    
    itkGetMacro(Iterations, unsigned int);
    itkSetMacro(Iterations, unsigned int);
    itkGetMacro(SpatialSigma, float);
    itkGetMacro(IntegrationSigma, float);
    itkGetMacro(Regularization, float);
    itkSetMacro(Regularization, float);
    itkGetMacro(Relaxation, float);
    itkSetMacro(Relaxation, float);
    itkGetMacro(NumberOfLevels, unsigned int);
    itkSetMacro(NumberOfLevels, unsigned int);
    
    virtual void Update();
    virtual void SetInput(ImageFileSet* input);
    
    void SetSpatialSigma(float sigma);
    void SetIntegrationSigma(float sigma);
    
    ImageType::Pointer GetPreviewImage();
    
protected:
    MultiResolutionOpticalFlowPipeline();
    virtual ~MultiResolutionOpticalFlowPipeline(){}

private:
// Not implemented
    MultiResolutionOpticalFlowPipeline(const Self& other);
    void operator=(const Self& other);
     
    unsigned int m_Iterations;
    float m_SpatialSigma;
    float m_IntegrationSigma;
    float m_Regularization;
    float m_Relaxation;
    unsigned int m_NumberOfLevels;
    HarrisType::Pointer harris;
};
