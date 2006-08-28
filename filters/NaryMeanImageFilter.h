#pragma once

#include "itkNaryFunctorImageFilter.h"
#include "itkNumericTraits.h"

template< class TInput, class TOutput>
class NMeanFunctor
{
public:
	// An accumulator type should be large enough to hold the sum of many input images.
	typedef typename itk::NumericTraits< TInput>::AccumulateType AccumulatorType;

	NMeanFunctor() {}
	~NMeanFunctor() {}

	inline TOutput operator()(const itk::Array< TInput > &B)
	{
		// Compute the mean of all input.
		AccumulatorType sum = itk::NumericTraits< TOutput >::Zero;
		for (unsigned int i = 0; i < B.size(); i++)
		{
			sum += static_cast< TOutput > (B[i]);
		}

		return static_cast< TOutput > ( sum / B.size() );
	}

	bool operator!=(const NMeanFunctor& ) const
	{
		return false;
	}
};

template <class TInputImage, class TOutputImage>
class NaryMeanImageFilter : 
	public itk::NaryFunctorImageFilter< TInputImage, TOutputImage, 
		NMeanFunctor< typename TInputImage::PixelType, typename TOutputImage::PixelType > >
{
public:
	// Standard itk class typedefs
	typedef NaryMeanImageFilter Self;
	typedef itk::NaryFunctorImageFilter<TInputImage, TOutputImage,
		NMeanFunctor< typename TInputImage::PixelType, typename TOutputImage::PixelType > > SuperClass;
	typedef itk::SmartPointer<Self> Pointer;
	typedef itk::SmartPointer<const Self> ConstPointer;

	itkNewMacro(Self);

protected:
	NaryMeanImageFilter() {}
	virtual ~NaryMeanImageFilter() {}

private:
	NaryMeanImageFilter(const Self&); // not implemented.
	void operator=(const Self&); // not implemented.
};
