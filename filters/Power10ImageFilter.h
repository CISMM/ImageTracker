#pragma once

#include "vnl/vnl_math.h"
#include "itkUnaryFunctorImageFilter.h"

/**
 * Computes vcl_pow(10,x), which is 10^x, for any input number x.
 */
template < class TInput, class TOutput >
class Pow10Functor
{
public:
    Pow10Functor(){}
    virtual ~Pow10Functor(){}
    
    TOutput operator()(const TInput& in)
    {
        return static_cast<TOutput>(vcl_pow(10, in));
    }
    
    bool operator!=(const Pow10Functor& other) const
    { return false; }
};

/**
 * /class Power10ImageFilter
 * /brief Computes vcl_pow(10, x) for each pixel in an input image.
 */
template < class TInputImage, class TOutputImage >
class Power10ImageFilter :
public itk::UnaryFunctorImageFilter< TInputImage, TOutputImage, 
    Pow10Functor< typename TInputImage::PixelType, typename TOutputImage::PixelType > >
{
public:
    // Common itk typedefs
    typedef Power10ImageFilter Self;
    typedef itk::UnaryFunctorImageFilter< TInputImage, TOutputImage, 
        Pow10Functor< typename TInputImage::PixelType, typename TOutputImage::PixelType > > Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    
    itkNewMacro(Self);
    itkTypeMacro(Power10ImageFilter, UnaryFunctorImageFilter);
protected:
    Power10ImageFilter(){}
    virtual ~Power10ImageFilter(){}
private:
    // purposefully not implemented
    Power10ImageFilter(const Self& other);
    void operator=(const Self& other);
};
