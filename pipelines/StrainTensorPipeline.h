#pragma once

#include <string>

#include "itkImage.h"
#include "itkVector.h"

#include "CommonTypes.h"
#include "ItkPipeline.h"

/**
 * \class StrainTensorPipeline
 * \brief Computes the strain tensor for a given 2D displacement field.
 * Computes the tensile strain given a displacement field.  A displacment field represents
 * displacement in X and Y at each image location--it is a vector field: 
 * L(x,y) = [Lx(x,y), Ly(x,y)].
 *
 * Tensile strain is a measure of change in displacement.  For example, for a rubber band of
 * unstretched length L and stretched length L+dL, the strain is dL/L.  In the continuum,
 * strain is the spatial derivative of displacement.  The 2D strain tensor, therefore, has 4
 * components: d Lx / dx, d Lx / dy, d Ly / dx, d Ly / dy.
 *
 * This pipeline is a little different from most; it ignores its OutputFiles variable, but 
 * instead relies on StrainXFormat and StrainYFormat to generate output file names.  This is
 * because most pipelines have a single output stream, while this one has two.
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
    
    itkGetMacro(StrainXFormat, std::string);
    itkSetMacro(StrainXFormat, std::string);
    
    itkGetMacro(StrainYFormat, std::string);
    itkSetMacro(StrainYFormat, std::string);

    
protected:
    StrainTensorPipeline()
    {}
    virtual ~StrainTensorPipeline(){}
    
private:
    // Not implemented
    StrainTensorPipeline(const Self& other);
    void operator=(const Self& other);
    
    double m_Sigma;
    std::string m_StrainXFormat;
    std::string m_StrainYFormat;
};
