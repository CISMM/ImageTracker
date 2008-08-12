#pragma once

#include "ItkPipeline.h"
#include "VectorFileSet.h"

class ItkVectorPipeline
    : public ItkPipeline
{
public:
    typedef ItkVectorPipeline Self;
    typedef ItkPipeline Superclass;
    typedef itk::SmartPointer<Self> Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;
    
    virtual void SetInput(VectorFileSet* images);
    virtual VectorFileSet* GetInput();

protected:
    ItkVectorPipeline();
    virtual ~ItkVectorPipeline();
    
    VectorFileSet* input;
    
private:
    ItkVectorPipeline(const Self& other);
    void operator=(const Self& other);
};
