#pragma once

#include "ImageFileSet.h"
#include "ItkPipeline.h"

class ItkImagePipeline
    : public ItkPipeline
{
public:
    typedef ItkImagePipeline Self;
    typedef ItkPipeline Superclass;
    typedef itk::SmartPointer<Self> Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;
    
    virtual void SetInput(ImageFileSet* images);
    virtual ImageFileSet* GetInput();
    
protected:
    ItkImagePipeline();
    virtual ~ItkImagePipeline();
    
    ImageFileSet* input;
    
private:
    ItkImagePipeline(const Self& other);
    void operator=(const Self& other);
};
