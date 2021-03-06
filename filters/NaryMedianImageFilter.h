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
    
    TOutput operator()(const std::vector< TInput > &B)
    {
        // tranfer the pixel values to a std::vector, because that supports sorting
        // TODO: Does vnl_vector support sorting?  That is what itk::Array is based on.
        // ITK changed the NaryFunctorImageFilter to be based on std::vectors.
        // TODO: The median calculation here is "sort and pick central value."  Because 
        // it is based on sorting, this takes n log n time.  Amortized linear time 
        // algorithms exist.
        // We sort a working copy of B
        std::vector< TInput > theVector(B);
        
        std::sort(theVector.begin(), theVector.end());
        
        // Choose the middle sorted value
        return static_cast< TOutput >(theVector[static_cast< unsigned int >(B.size() / 2)]);
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
