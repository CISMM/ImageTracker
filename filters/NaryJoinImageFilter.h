
#pragma once
#include <vector>

#include "itkImage.h"
#include "itkNaryFunctorImageFilter.h"
#include "itkNumericTraits.h"
#include "itkVector.h"

/** 
 * \class NVectorFunctor
 * \brief Combines N components into an N-component vector.
 */
template< class TInput, class TOutput>
class NVectorFunctor
{
public:

    NVectorFunctor() {}
    ~NVectorFunctor() {}

    inline TOutput operator()(const std::vector< TInput > &B)
    {
        TOutput nvec;
        for (unsigned int i = 0; 
             i < B.size() &&
             i < TOutput::GetVectorDimension(); 
             i++)
        {
            nvec[i] = B[i];
        }

        return nvec;
    }

    bool operator!=(const NVectorFunctor& ) const
    {
        return false;
    }
};

/**
 * \class NaryJoinImageFilter
 * \brief Joins N component scalar images to an N-vector-pixel image.
 */
template< class TInputImage, unsigned int NComponents >
class NaryJoinImageFilter
    : public itk::NaryFunctorImageFilter< 
        TInputImage, 
        itk::Image< itk::Vector< typename TInputImage::PixelType, NComponents >, TInputImage::ImageDimension >,
        NVectorFunctor< typename TInputImage::PixelType, itk::Vector< typename TInputImage::PixelType, NComponents > > >
{
public:
// Standard itk class typedefs
    typedef NaryJoinImageFilter Self;
    typedef itk::NaryFunctorImageFilter< 
            TInputImage, 
            itk::Image< itk::Vector< typename TInputImage::PixelType, NComponents >, TInputImage::ImageDimension >,
            NVectorFunctor< typename TInputImage::PixelType, itk::Vector< typename TInputImage::PixelType, NComponents > > > Superclass;
    typedef itk::SmartPointer<Self> Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;

    itkNewMacro(Self);
    itkTypeMacro(NaryJoinImageFilter, NaryFunctorImageFilter);
    
    /**
     * Sets the Nth component input image.
     */
    void SetNthInput(unsigned int index, const TInputImage* input)
    {
        Superclass::SetNthInput(index, const_cast<TInputImage*>(input));
    }

protected:
    NaryJoinImageFilter() {}
    virtual ~NaryJoinImageFilter() {}

private:
    NaryJoinImageFilter(const Self&); // not implemented.
    void operator=(const Self&); // not implemented.
};
