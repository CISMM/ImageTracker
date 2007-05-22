#pragma once

#include <list>
#include <string>

#include "itkObject.h"

#include "ItkPipelineObserver.h"

/**
 * \class ItkPipeline
 * \brief Base class for ITK image processing pipeline aggregates.
 * 
 * Interface for classes that compose groups of ITK objects into
 * useful image processing tasks.  For example, a pipeline might
 * compute the optical flow for all frames in a series of images.
 * The Update() method executes the pipeline.
 *
 * An Observer pattern provides progress information for pipeline
 * execution.  Subclasses should periodically call NotifyProgress()
 * from within the Update() method.
 */
class ItkPipeline :
    public itk::Object
{
public:
    typedef ItkPipeline Self;
    typedef itk::Object Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    
    typedef std::list< ItkPipelineObserver* > ObserverList;
    
    virtual void Update() = 0;
    
    virtual void AddObserver(ItkPipelineObserver* observer);
    virtual void RemoveObserver(ItkPipelineObserver* observer);
    
    /** 
     * Notifies all observers of the progress of this ItkPipeline. Returns
     * true if one of the observers has requested that the pipeline execution
     * be aborted.
     */
    virtual bool NotifyProgress(double progress, const std::string& message = "");
    
protected:
    ItkPipeline(){}
    virtual ~ItkPipeline(){}
    
    ObserverList observers;
    
private:
    // not implemented
    ItkPipeline(const Self& other);
    void operator=(const Self& other);
};
