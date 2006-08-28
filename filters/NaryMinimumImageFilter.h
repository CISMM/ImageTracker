#pragma once

#include "itkNaryFunctorImageFilter.h"

template < class TInput, class TOutput >
class NMinimumFunctor
{
public:
    NMinimumFunctor() {}
    ~NMinimumFunctor() {}
    
    inline TOutput operator()(const itk::Array< TInput > &B)
    {
        // Compute the minimum of all input.
        TOutput min = static_cast< TOutput >(B[0]);
        for (unsigned int i = 0; i < B.size(); i++)
        {
            min = static_cast< TOutput >(B[i]) < min ?
                static_cast< TOutput >(B[i]) : min;
        }
        
        return min;
    }
    
    bool operator!=(const NMinimumFunctor& ) const
    {
        return false;
    }
};

template < class TInput, class TOutput >
class NaryMinimumImageFilter :
public itk::NaryFunctorImageFilter< TInput, TOutput, 
    NMinimumFunctor< typename TInput::PixelType, typename TOutput::PixelType > >
{
public:
    // Standard ITK typedefs
    typedef NaryMinimumImageFilter Self;
    typedef itk::NaryFunctorImageFilter< TInput, TOutput, 
        NMinimumFunctor< typename TInput::PixelType, typename TOutput::PixelType > > Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    
    itkNewMacro(Self);
    itkTypeMacro(NaryMinimumImageFilter, NaryFunctorImageFilter);
    
protected:
    NaryMinimumImageFilter(){}
    virtual ~NaryMinimumImageFilter(){}

private:
    // Purposefully not implemented
    NaryMinimumImageFilter(const Self&);
    void operator=(const Self&);
};
