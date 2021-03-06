#pragma once

#include "itkImage.h"

#include "ItkVectorPipeline.h"

/**
 * Integrates the displacement of massless particles in time-varying flow field.
 */ 
class IntegrateFlowFieldPipeline :
    public ItkVectorPipeline
{
public:
    // Common ITK typedefs
    typedef IntegrateFlowFieldPipeline Self;
    typedef ItkVectorPipeline Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    
    itkNewMacro(Self);
    itkTypeMacro(IntegrateFlowFieldPipeline, ItkVectorPipeline);
    
    // Image types
    typedef float ComponentType;
    typedef itk::Vector< ComponentType, 2 > PixelType;
    typedef itk::Image< PixelType, 2 > FlowImageType;
    typedef itk::Image< PixelType, 3 > FlowSeriesImageType;
            
    itkGetMacro(StepSize, double);
    itkSetMacro(StepSize, double);
    
    virtual void Update();
    
protected:
    IntegrateFlowFieldPipeline():
        m_StepSize(0.5)
    {}
    
    virtual ~IntegrateFlowFieldPipeline(){}
    
private:
    // Not implemented
    IntegrateFlowFieldPipeline(const Self& other);
    void operator=(const Self& other);
    
    double m_StepSize;
};
