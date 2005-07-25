#pragma once

#include "itkImageFileWriter.h"
#include "itkLightObject.h"

#include "CLGOpticFlowImageFilter.h"
#include "FileSet.h"
#include "FileSetImageReader.h"

class CLGOpticFlowPipeline : public itk::LightObject
{
public:
    /** Standard itk object typedefs */
    typedef CLGOpticFlowPipeline Self;
    typedef itk::SmartPointer<Self> Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;
    typedef itk::LightObject Superclass;

    /** Some helpful typedefs */
    typedef FileSetImageReader::InternalImageType ImageType;
    typedef CLGOpticFlowImageFilter<ImageType, ImageType> FlowFilterType;
    typedef itk::ImageFileWriter<FlowFilterType::OutputImageType> WriterType;

    /** itk object macros */
    itkNewMacro(Self);
    itkTypeMacro(CLGOpticFlowPipeline, itk::LightObject);

    double GetSpatialSigma() { return this->flowFilter->GetSpatialSigma(); }
    double GetRegularization() { return this->flowFilter->GetRegularization(); }
    double GetRelaxation() { return this->flowFilter->GetRelaxation(); }
    unsigned int GetIterations() { return this->flowFilter->GetIterations(); }
    FileSet* GetSource() { return this->source; }
    FileSet* GetDestination() { return this->destination; }

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
    void SetSource(FileSet* source)
    {
        this->source = source;
        this->init = false;
    }
    void SetDestination(FileSet* dest)
    {
        this->destination = dest;
        this->init = false;
    }

    bool UpdateOne();
    void UpdateAll();

protected:
    CLGOpticFlowPipeline(void)
    {
        this->flowFilter = FlowFilterType::New();
        this->source = NULL;
        this->destination = NULL;
        this->reader = NULL;
        this->init = false;
    }

    virtual ~CLGOpticFlowPipeline(void) 
    {
        if (this->reader)
            delete reader;

        this->source = NULL;
        this->destination = NULL;
    }

private:
    void InitializePipeline();

    bool init;
    FileSet* source;
    FileSet* destination;

    FileSetImageReader* reader;
    FlowFilterType::Pointer flowFilter;
    WriterType::Pointer writer;
    FileSet::FileIterator destinationIt;
};
