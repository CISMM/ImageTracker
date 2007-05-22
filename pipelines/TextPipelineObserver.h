#pragma once

#include "itkObject.h"
#include "itkObjectFactory.h"

#include "ItkPipelineObserver.h"

/**
 * \class TextPipelineObserver
 * \brief Provides logger updates to the progress of an ItkPipeline object.
 *
 * ItkPipelineObserver that creates logger messages in response to ItkPipeline
 * progress events.
 */
class TextPipelineObserver :
    public ItkPipelineObserver
{
public:
    typedef TextPipelineObserver Self;
    typedef ItkPipelineObserver Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    
    itkNewMacro(Self);
    itkTypeMacro(TextPipelineObserver, ItkPipelineObserver);
    
    virtual bool Update(double progress, const std::string& message);
    
protected:
    TextPipelineObserver(){}
    virtual ~TextPipelineObserver(){}
    
private:
    // not implemented
    TextPipelineObserver(const Self& other);
    void operator=(const Self& other);
};
