#pragma once

#include "itkImage.h"
#include "itkObject.h"
#include "itkVector.h"

#include "CommonTypes.h"
#include "FileSet.h"
#include "ImageSetReader.h"
#include "ItkPipeline.h"
#include "Logger.h"

class MultiResolutionOpticalFlowPipeline :
    public ItkPipeline
{
public:
    // Common itk typedefs
    typedef MultiResolutionOpticalFlowPipeline Self;
    typedef ItkPipeline Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    
    itkNewMacro(Self);
    itkTypeMacro(MultiResolutionOpticalFlowPipeline, ItkPipeline);
    
    // Image typedefs
    typedef CommonTypes::InternalImageType InternalImageType;
    typedef itk::Image< 
            itk::Vector< float, InternalImageType::ImageDimension >, 
            InternalImageType::ImageDimension > FlowImageType;
    
    itkGetMacro(Iterations, unsigned int);
    itkSetMacro(Iterations, unsigned int);
    itkGetMacro(SpatialSigma, float);
    itkSetMacro(SpatialSigma, float);
    itkGetMacro(SmoothWeighting, float);
    itkSetMacro(SmoothWeighting, float);
    itkGetMacro(Relaxation, float);
    itkSetMacro(Relaxation, float);
    itkGetMacro(NumberOfLevels, unsigned int);
    itkSetMacro(NumberOfLevels, unsigned int);
            
    //const FileSet& GetOutputFiles() { return this->outputFiles; }
    //void SetOutputFiles(const FileSet& files) { this->outputFiles = files; }
    //void SetInput(ImageSetReaderBase* input) { this->input = input; }
    
    virtual void Update();
    
protected:
    MultiResolutionOpticalFlowPipeline()
        : m_Iterations(200),
          m_SpatialSigma(1.0),
          m_SmoothWeighting(10e4),
          m_Relaxation(1.9),
          m_NumberOfLevels(1)
        {}
    virtual ~MultiResolutionOpticalFlowPipeline(){}

private:
// Not implemented
    MultiResolutionOpticalFlowPipeline(const Self& other);
    void operator=(const Self& other);
     
    unsigned int m_Iterations;
    float m_SpatialSigma;
    float m_SmoothWeighting;
    float m_Relaxation;
    unsigned int m_NumberOfLevels;
};
