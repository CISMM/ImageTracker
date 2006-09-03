/***************************************************************************
 *            RealToComplexImageFilter.h
 *
 *  Thu Aug 31 11:51:33 2006
 *  Copyright  2006  Brian Eastwood
 ****************************************************************************/

#include <complex>

#include "itkUnaryFunctorImageFilter.h"

namespace itk
{
template < class TPixel >
class RealToComplexFunctor
{
public:
	RealToComplexFunctor(){}
	
	bool operator==(const RealToComplexFunctor& other) const
	{
		return false;
	}
	
	typename std::complex< TPixel > operator()(const TPixel& real)
	{
		return std::complex< TPixel >(real, 0);
	}
};

/**
 * /class RealToComplexImageFilter
 * /brief Creates a complex image from a real-valued image. Each real-valued
 * pixel is used as the real part of the resultant image.  The imaginary part 
 * of each pixel is zero.
 */
template < class TPixel, unsigned int Dimension = 3 >
class RealToComplexImageFilter :
public UnaryFunctorImageFilter< Image< TPixel, Dimension >, 
	Image< std::complex< TPixel >, Dimension >, 
	RealToComplexFunctor< TPixel > >
{
public:
	// helpful typedefs
	typedef TPixel TPixelType;
	typedef Image< TPixelType, Dimension > TInputImageType;
	typedef Image< std::complex< TPixelType >, TInputImageType::ImageDimension > TOutputImageType;
	typedef RealToComplexFunctor< TPixelType > FunctorType;
	
	// Standard itk typedefs
	typedef RealToComplexImageFilter Self;
	typedef UnaryFunctorImageFilter< TInputImageType, TOutputImageType, FunctorType > Superclass;
	typedef SmartPointer< Self > Pointer;
	typedef SmartPointer< const Self > ConstPointer;

	// Standard itk macros
	itkNewMacro(Self);
	itkTypeMacro(RealToComplexImageFilter, UnaryFunctorImageFilter);
	itkStaticConstMacro(ImageDimension, unsigned int, TInputImageType::ImageDimension);

protected:
	RealToComplexImageFilter(){}
	virtual ~RealToComplexImageFilter(){}
		
private:
	// Purposefully not implemeted
	RealToComplexImageFilter(const Self& other);
	void operator=(const Self& other);
};
} // end itk
