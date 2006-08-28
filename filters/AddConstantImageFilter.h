#pragma once

#include <limits>

#include "itkUnaryFunctorImageFilter.h"
#include "itkNumericTraits.h"

template < class TInput, class TOutput, class TConst >
class ConstantFunctor
{
public:
    typedef typename itk::NumericTraits< TOutput >::AccumulateType AccumulateType;
    
    ConstantFunctor() :
        constant(static_cast<TConst>(std::numeric_limits<TInput>::min())),
        maximum(static_cast<AccumulateType>(std::numeric_limits<TOutput>::max()))
    {
    }

    bool operator==(const ConstantFunctor& other) const
    {
        return false;
    }
    
    void SetConstant(const TConst& value)
    {
        this->constant = value;
    }
    
    TConst GetConstant() const
    {
        return this->constant;
    }
    
    TOutput operator()(const TInput& pixel)
    {
        // We must check for overflow.
        // We do this with respect to the output pixel type,
        // and avoid overflow in the check itself.
        AccumulateType accPixel = static_cast<AccumulateType>(pixel);
        
        if (accPixel + this->constant < this->maximum)
            return static_cast<TOutput>(accPixel + this->constant);
        else
            return static_cast<TOutput>(this->maximum);
    }
    
private:
    TConst constant;
    AccumulateType maximum;
};

template < class TInput, class TOutput, class TConst = typename TOutput::PixelType >
class AddConstantImageFilter :
public itk::UnaryFunctorImageFilter< TInput, TOutput, 
    ConstantFunctor< typename TInput::PixelType, typename TOutput::PixelType, TConst > >
{
public:
    typedef TConst ConstantType;
    // Standard ITK typedefs
    typedef AddConstantImageFilter Self;
    typedef itk::UnaryFunctorImageFilter< TInput, TOutput,
        ConstantFunctor< typename TInput::PixelType, typename TOutput::PixelType, TConst > > Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    
    itkNewMacro(Self);
    itkTypeMacro(AddConstantImageFilter, UnaryFunctorImageFilter);
    
    void SetConstant(const ConstantType& value)
    { 
        this->GetFunctor().SetConstant(value); 
    }
    
    ConstantType& GetConstant()
    {
        return this->GetFunctor().GetConstant();
    }
    
protected:
    AddConstantImageFilter() {}
    virtual ~AddConstantImageFilter() {}
    
private:
    // Not implemented
    AddConstantImageFilter(const Self&);
    void operator=(const Self&);
};
