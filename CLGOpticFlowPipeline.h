#pragma once

#include "itkImageFileWriter.h"
#include "itkLightObject.h"

#include "CLGOpticFlowImageFilter.h"
#include "CommonTypes.h"
#include "FileSet.h"
#include "ImageSetReader.h"

class CLGOpticFlowPipeline : public itk::LightObject
{
public:
    /** Standard itk object typedefs */
    typedef CLGOpticFlowPipeline Self;
    typedef itk::SmartPointer<Self> Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;
    typedef itk::LightObject Superclass;

    /** Some helpful typedefs */
    typedef CommonTypes::InputImageType InputImageType;
    typedef CommonTypes::InternalImageType ImageType;
    typedef ImageSetReader<InputImageType, ImageType> ReaderType;
    typedef CLGOpticFlowImageFilter<ImageType, ImageType> FlowFilterType;

    /** itk object macros */
    itkNewMacro(Self);
    itkTypeMacro(CLGOpticFlowPipeline, itk::LightObject);

    double GetSpatialSigma() { return this->flowFilter->GetSpatialSigma(); }
    double GetRegularization() { return this->flowFilter->GetRegularization(); }
    double GetRelaxation() { return this->flowFilter->GetRelaxation(); }
    unsigned int GetIterations() { return this->flowFilter->GetIterations(); }
    const FileSet& GetSource() { return this->source; }
    const FileSet& GetDestination() { return this->destination; }

    void SetSpatialSigma(double sigma) 
    { 
        this->flowFilter->SetSpatialSigma(sigma);
        this->init = false;
    }
    void SetRegularization(double reg) 
    { 
        this->flowFilter->SetRegularization(reg);
        this->init = false;
    }
    void SetRelaxation(double relax) 
    { 
        this->flowFilter->SetRelaxation(relax);
        this->init = false;
    }
    void SetIterations(unsigned int iter) 
    { 
        this->flowFilter->SetIterations(iter);
        this->init = false;
    }
    void SetSource(const FileSet& source)
    {
        this->source = source;
        this->init = false;
    }
    void SetDestination(const FileSet& dest)
    {
        this->destination = dest;
        this->init = false;
    }

    bool UpdateOne();
    void UpdateAll();

protected:
    CLGOpticFlowPipeline(void) :
        init(false),
        index(0)
    {
        this->flowFilter = FlowFilterType::New();
        this->init = false;
    }

    virtual ~CLGOpticFlowPipeline(void) 
    {
    }

private:
    void InitializePipeline();

    bool init;
    unsigned int index;
    FileSet source;
    FileSet destination;
    ReaderType reader;
    
    FlowFilterType::Pointer flowFilter;
};
