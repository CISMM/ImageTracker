#pragma once

#include <complex>
#include "itkImageToImageFilter.h"
#include "itkImage.h"

namespace itk
{
/**
 * /class FFTComplexToComplexImageFilter
 * /brief Computes the discrete Fourier transform of a set of complex data.
 *
 * Computes the DFT of complex data, e.g. an image with complex-value pixel
 * data. By default, the forward DFT is used (and by forward, we mean the sign
 * of the exponent in the DFT formulation is negative, after the convention used
 * by the FFTW library).  The result of a DFT on complex data is, in general,
 * also complex.  The result has the same dimensionality and size as the input.	
 */
template < class TPixel, unsigned int Dimension = 3 >
class FFTComplexToComplexImageFilter :
public ImageToImageFilter< 	Image< std::complex< TPixel >, Dimension >, 
							Image< std::complex< TPixel >, Dimension > >
{
public:
	// standard itk class typedefs
	typedef Image< std::complex< TPixel >, Dimension > TInputImageType;
	typedef Image< std::complex< TPixel >, Dimension > TOutputImageType;

	typedef FFTComplexToComplexImageFilter Self;
	typedef ImageToImageFilter< TInputImageType, TOutputImageType > Superclass;
	typedef SmartPointer< Self > Pointer;
	typedef SmartPointer< const Self > ConstPointer;

	itkNewMacro(Self);
	itkTypeMacro(FFTComplexToComplexImageFilter, ImageToImageFilter);
	itkStaticConstMacro(ImageDimension, unsigned int, TInputImageType::ImageDimension);
	
	/**
	 * Specify that the DFT routine should use a forward discrete Fourier transform.
	 */
	void SetForward()
	{
		this->m_Forward = true;
		this->Modified();
	}
	/**
 	 * Specify that the DFT routine should use a backward discrete Fourier transform.
	 */
	void SetBackward()
	{
		this->m_Forward = false;
		this->Modified();
	}
	
	bool IsForward()
	{
		return this->m_Forward;
	}

protected:
	FFTComplexToComplexImageFilter() 
		: m_Forward(true)
	{}
	
	virtual ~FFTComplexToComplexImageFilter() {}

private:
	bool m_Forward;

	// Purposefully not implemented.
	FFTComplexToComplexImageFilter(const Self& other);
	void operator=(const Self& other);
};
	
} // end itk namespace
