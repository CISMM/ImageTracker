#pragma once

#include <string>

#include "itkObject.h"

/**
 * \class ItkPipelineObserver
 * \brief Observer/Subscriber to an ItkPipeline's progress.
 *
 * Interface for observers of the progress of ItkPipelines.  An ItkPipelineObserver
 * that is registered with an ItkPipeline object will receive periodic Update() 
 * callbacks as the pipeline executes.  The Update() method provides progress
 * information, and optional message, and provides this observer the opportunity
 * to notify the pipeline that it should abort execution when convenient.  The
 * progress update values should be between zero and Maximum.
 */
class ItkPipelineObserver :
    public itk::Object
{
public:
    typedef ItkPipelineObserver Self;
    typedef itk::Object Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    
    /**
     * Update this observer with the progress from the ItkPipeline under observation.
     * If Update() returns true, the ItkPipeline should halt at an opportune moment.
     */
    virtual bool Update(double progress, const std::string& message) = 0;
    
    itkGetMacro(Progress, double);

    itkGetMacro(Maximum, double);
    itkSetMacro(Maximum, double);
    
    itkGetMacro(Aborted, bool);
    itkSetMacro(Aborted, bool);
    
protected:
    ItkPipelineObserver() :
        m_Progress(0.0),
        m_Maximum(1.0),
        m_Aborted(false)
    {}
    virtual ~ItkPipelineObserver() {}
    
private:
    // Not implemented
    ItkPipelineObserver(const Self& other);
    void operator=(const Self& other);
    
    double m_Progress;
    double m_Maximum;
    bool m_Aborted;
};
