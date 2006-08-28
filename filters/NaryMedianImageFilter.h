#pragma once

#include <algorithm>
#include <vector>

#include "itkArray.h"
#include "itkNaryFunctorImageFilter.h"

template < class TInput, class TOutput >
class NMedianFunctor
{
public:
    NMedianFunctor() {}
    virtual ~NMedianFunctor() {}
    
    TOutput operator()(const itk::Array< TInput > &B)
    {
        // tranfer the pixel values to a std::vector, because that supports sorting
        // TODO: Does vnl_vector support sorting?  That is what itk::Array is based on.
        // TODO: The median calculation here is "sort and pick central value."  Because 
        // it is based on sorting, this takes n log n time.  Linear time algoriths exist.
        std::vector< TInput > stdvec(B.size());
        for (unsigned int i = 0; i < B.size(); i++)
        {
            stdvec.push_back(B[i]);
        }
        
        std::sort(stdvec.begin(), stdvec.end());
        unsigned int idx = static_cast< unsigned int >(B.size() / 2);
        TOutput result;
        
        if (B.size() == 0) // no elements
        {
            result = 0;
        }
        else if (B.size() % 2 == 0) // even number of elements
        {
            // average two center elements
            result = static_cast< TOutput > ((B[idx-1]+B[idx]) / 2.0);
        }
        else // odd number of elements
        {
            // index we caclulate is the one we're after
            result = static_cast< TOutput > (B[idx]);
        }
        
        return result;
    }

    bool operator!=(const NMedianFunctor& other) const
    {
        return false;
    }
};

template < class TInput, class TOutput >
class NaryMedianImageFilter:
public itk::NaryFunctorImageFilter< TInput, TOutput, 
    NMedianFunctor< typename TInput::PixelType, typename TOutput::PixelType > >
{
public:    
    // Standard itk typedefs
    typedef NaryMedianImageFilter Self;
    typedef itk::NaryFunctorImageFilter< TInput, TOutput, NMedianFunctor< typename TInput::PixelType, typename TOutput::PixelType > > Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    
    itkNewMacro(Self);
    
protected:
    NaryMedianImageFilter() {}
    virtual ~NaryMedianImageFilter() {}
    
private:
    // purposefully not implemented.
    NaryMedianImageFilter(const Self&);
    void operator=(const Self&);
};
