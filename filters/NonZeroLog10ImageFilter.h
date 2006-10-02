#pragma once

#include "itkUnaryFunctorImageFilter.h"
#include "vnl/vnl_math.h"

/**
 * Functor that calculates a base ten logarithm for non-zero input. Any 
 * input value that is zero is treated as if it were a 1 (the result of
 * the logarithm is zero).
 */
template < class TInput, class TOutput >
class NZLog10Functor
{
public:
    NZLog10Functor(){}
    virtual ~NZLog10Functor(){}
    
    bool operator!=(const NZLog10Functor& other) const
    { return false; }
    
    TOutput operator()(const TInput& input)
    {
        TOutput result;
        if (input == 0)
            result = 0;  // Which is log(1)
        else
            result = static_cast<TOutput>(vcl_log10(static_cast<double>(input)));

        return result;
    }
};

/**
 * Image filter that calculates a base ten logarithm for non-zero image
 * values. Image values that are zero would have a log of negative infinity,
 * which is difficult to deal with. So, zero values are treated as if they
 * were 1. There is a hand-waving justification for this: for most 8-bit 
 * image sensors, one cannot really trust any value that is less than 5. So,
 * surely an image value of zero could just as easily have been one. Note
 * that this is a convenience for certain types of image processing, and
 * you should consider carefully whether it is appropriate for your purposes
 * before using this class.
 *
 * An alternative approach to the non-zero log is to simply add one to each
 * image value. Because sensor error is proportional to incident light 
 * intensity, this approach introduces a bias that is not equivalent for all
 * image values.
 *
 * It may be useful to keep track of the pixels whose value was changed by
 * this filter.
 */
template < class TInput, class TOutput >
class NonZeroLog10ImageFilter : 
public itk::UnaryFunctorImageFilter< TInput, TOutput,
    NZLog10Functor < typename TInput::PixelType, typename TOutput::PixelType > >
{
public:
    // common itk typedefs
    typedef NonZeroLog10ImageFilter Self;
    typedef itk::UnaryFunctorImageFilter< TInput, TOutput,
        NZLog10Functor < typename TInput::PixelType, typename TOutput::PixelType > > Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    
    itkNewMacro(Self);

protected:
    NonZeroLog10ImageFilter(){}
    virtual ~NonZeroLog10ImageFilter(){}

private:
    // Not implemented
    NonZeroLog10ImageFilter(const NonZeroLog10ImageFilter& other);
    void operator=(const NonZeroLog10ImageFilter& other);
};
