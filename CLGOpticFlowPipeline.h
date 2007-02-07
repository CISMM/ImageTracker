#pragma once

#include "itkDiscreteGaussianImageFilter.h"
#include "itkObject.h"

#include "CLGOpticFlowImageFilter.h"
#include "CommonTypes.h"
#include "FileSet.h"
#include "ImageSetReader.h"

class CLGOpticFlowPipeline : 
    public itk::Object
{
public:
    /** Standard itk object typedefs */
    typedef CLGOpticFlowPipeline Self;
    typedef itk::SmartPointer<Self> Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;
    typedef itk::LightObject Superclass;

    /** Some helpful typedefs */
    typedef CommonTypes::InternalImageType ImageType;
    typedef ImageSetReaderBase* ReaderType;
    typedef CLGOpticFlowImageFilter<ImageType, ImageType> FlowFilterType;
    typedef itk::DiscreteGaussianImageFilter< ImageType, ImageType > SmoothFilterType;

    /** itk object macros */
    itkNewMacro(Self);
    itkTypeMacro(CLGOpticFlowPipeline, itk::Object);

    double GetSpatialSigma() { return this->flowFilter->GetSpatialSigma(); }
    double GetRegularization() { return this->flowFilter->GetRegularization(); }
    double GetRelaxation() { return this->flowFilter->GetRelaxation(); }
    unsigned int GetIterations() { return this->flowFilter->GetIterations(); }

    const FileSet& GetOutputFiles() { return this->outputFiles; }
    
    ImageType::Pointer GetPreviewImage();

    void SetSpatialSigma(double sigma);
    void SetRegularization(double reg);
    void SetRelaxation(double relax);
    void SetIterations(unsigned int iter);
    
    void SetInput(ReaderType input);
    void SetOutputFiles(const FileSet& dest);

    bool UpdateOne();
    void UpdateAll();
    void Update();

protected:
    CLGOpticFlowPipeline(void);
    virtual ~CLGOpticFlowPipeline(void) {}

private:
    void InitializePipeline();

    bool init;
    unsigned int index;
    FileSet outputFiles;
    ReaderType inputReader;
    
    FlowFilterType::Pointer flowFilter;
    SmoothFilterType::Pointer smooth;
};
