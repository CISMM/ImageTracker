#pragma once

#include <string>

#include "itkImage.h"
#include "itkVector.h"

#include "CommonTypes.h"
#include "ItkPipeline.h"

/**
 * \class StrainTensorPipeline
 * \brief Computes the strain tensor for a given 2D displacement field.
 * Computes the Lagrangian strain tensor given a displacement field.  A displacment field represents
 * displacement in X and Y at each image location--it is a vector field: 
 * L(x,y) = [Lx(x,y), Ly(x,y)].
 *
 * Strain is a measure of change in displacement as an object is deformed.
 */
class StrainTensorPipeline :
    public ItkPipeline
{
public:
    // Common itk typedefs
    typedef StrainTensorPipeline Self;
    typedef ItkPipeline Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    
    itkNewMacro(Self);
    itkTypeMacro(StrainTensorPipeline, ItkPipeline);
    
    virtual void Update();
    
    itkGetMacro(Sigma, double);
    itkSetMacro(Sigma, double);
    
protected:
    StrainTensorPipeline()
    {}
    virtual ~StrainTensorPipeline(){}
    
private:
    // Not implemented
    StrainTensorPipeline(const Self& other);
    void operator=(const Self& other);
    
    double m_Sigma;
};
