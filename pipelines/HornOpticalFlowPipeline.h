#pragma once

#include "itkImage.h"
#include "itkObject.h"
#include "itkVector.h"

#include "CommonTypes.h"
#include "FileSet.h"
#include "ImageSetReader.h"
#include "Logger.h"

class HornOpticalFlowPipeline :
    public itk::Object
{
public:
    // Common itk typedefs
    typedef HornOpticalFlowPipeline Self;
    typedef itk::Object Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    itkNewMacro(Self);
    itkTypeMacro(HornOpticalFlowPipeline, itk::Object);
    
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
            
    const FileSet& GetOutputFiles() { return this->outputFiles; }
    void SetOutputFiles(const FileSet& files) { this->outputFiles = files; }
    
    void SetInput(ImageSetReaderBase* input) { this->input = input; }
    
    void Update();
    
protected:
    HornOpticalFlowPipeline()
    : m_Iterations(200),
      m_SpatialSigma(1.0),
      m_SmoothWeighting(100)
    {}
    virtual ~HornOpticalFlowPipeline(){}
    
private:
    // Not implemented
    HornOpticalFlowPipeline(const Self& other);
    void operator=(const Self& other);
    
    ImageSetReaderBase* input;
    FileSet outputFiles;
    
    unsigned int m_Iterations;
    float m_SpatialSigma;
    float m_SmoothWeighting;
};
