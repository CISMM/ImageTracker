#pragma once

#include "itkDiscreteGaussianImageFilter.h"
#include "itkObject.h"

#include "CLGOpticFlowImageFilter.h"
#include "CommonTypes.h"
#include "FileSet.h"
#include "ImageSetReader.h"
#include "ItkPipeline.h"

class CLGOpticFlowPipeline : 
    public ItkPipeline
{
public:
    /** Standard itk object typedefs */
    typedef CLGOpticFlowPipeline Self;
    typedef itk::SmartPointer<Self> Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;
    typedef ItkPipeline Superclass;

    /** Some helpful typedefs */
    typedef CommonTypes::InternalImageType ImageType;
    typedef ImageSetReaderBase* ReaderType;
    typedef CLGOpticFlowImageFilter<ImageType, ImageType> FlowFilterType;
    typedef itk::DiscreteGaussianImageFilter< ImageType, ImageType > SmoothFilterType;

    /** itk object macros */
    itkNewMacro(Self);
    itkTypeMacro(CLGOpticFlowPipeline, ItkPipeline);

    double GetSpatialSigma() { return this->flowFilter->GetSpatialSigma(); }
    double GetRegularization() { return this->flowFilter->GetRegularization(); }
    double GetRelaxation() { return this->flowFilter->GetRelaxation(); }
    unsigned int GetIterations() { return this->flowFilter->GetIterations(); }

    //const FileSet& GetOutputFiles() { return this->outputFiles; }
    
    ImageType::Pointer GetPreviewImage();

    void SetSpatialSigma(double sigma);
    void SetRegularization(double reg);
    void SetRelaxation(double relax);
    void SetIterations(unsigned int iter);
    
    virtual void SetInput(ReaderType input);
    virtual void SetOutputFiles(const FileSet& dest);

    bool UpdateOne();
    void UpdateAll();
    virtual void Update();

protected:
    CLGOpticFlowPipeline(void);
    virtual ~CLGOpticFlowPipeline(void) {}

private:
    void InitializePipeline();

    bool init;
    unsigned int index;
    //FileSet outputFiles;
    
    FlowFilterType::Pointer flowFilter;
    SmoothFilterType::Pointer smooth;
};
