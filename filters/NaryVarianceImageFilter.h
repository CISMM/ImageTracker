#pragma once

#include "itkNaryFunctorImageFilter.h"
#include "itkNumericTraits.h"

/**
 * An N-ary functor that computes the variance of all the elements in
 * a data array.
 */
template < class TInput, class TOutput >
class NVarianceFunctor
{
public:
    typedef typename itk::NumericTraits< TInput >::AccumulateType AccumulatorType;
    typedef typename itk::NumericTraits< TInput >::FloatType FloatType;

    NVarianceFunctor() {}
    ~NVarianceFunctor() {}

    /**
     * Computes the variance of the elements in a data array. For N elements with
     * mean u, variance is computed by: 
     *      v = 1/(N-1) * SUM[(x_i - u)^2]
     * Note: we use N-1 as the unbiased sample correction factor; this is because we
     * presume that the set of images for which we are computing the variance is only
     * a sample.
     */
    inline TOutput operator()(const itk::Array< TInput > &data)
    {
        FloatType sum = itk::NumericTraits< TOutput >::Zero;

        // compute the sum of all elements in the array
        for (unsigned int i = 0; i < data.size(); i++)
        {
            sum += static_cast< TOutput > (data[i]);
        }
        FloatType mean = sum / data.size();

        // compute the variance of all elements in the array
        sum = itk::NumericTraits< TOutput >::Zero;
        for (unsigned int i = 0; i < data.size(); i++)
        {
            sum += (static_cast< TOutput > (data[i]) - mean) * (static_cast< TOutput > (data[i]) - mean);
        }

        return static_cast< TOutput > ( sum / (data.size()-1) );
    }

    bool operator!=(const NVarianceFunctor&) const
    {
        return false;
    }
};

template <class TInputImage, class TOutputImage>
class NaryVarianceImageFilter :
    public itk::NaryFunctorImageFilter< TInputImage, TOutputImage,
        NVarianceFunctor< typename TInputImage::PixelType, typename TOutputImage::PixelType > >
{
public:
    // Standard itk class typedefs
    typedef NaryVarianceImageFilter Self;
    typedef itk::NaryFunctorImageFilter< TInputImage, TOutputImage,
        NVarianceFunctor< typename TInputImage::PixelType, typename TOutputImage::PixelType > > Superclass;
    typedef itk::SmartPointer<Self> Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;

    itkNewMacro(Self);

    void AddInput(const TInputImage* input)
    {
        Superclass::SetInput(this->inputIdx, const_cast<TInputImage *>(input));
        this->inputIdx++;
    }

protected:
    NaryVarianceImageFilter()
    {
        this->inputIdx = 0;
    }
    virtual ~NaryVarianceImageFilter() {}

private:
    NaryVarianceImageFilter(const Self&);   // not implemented
    void operator=(const Self&);            // not implemented
    unsigned int inputIdx;
};
