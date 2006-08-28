#pragma once
#if defined(USE_FFTWF) || defined(USE_FFTWD)
#include "itkFFTComplexToComplexImageFilter.h"
#include "fftw3.h"

namespace itk
{

/**
 * /class FFTWComplexToComplexImageFilter
 * /brief Uses the FFTW library to implement a FFTComplexToComplexImageFilter
 */
template < class TPixel, unsigned int Dimension = 3 >
class FFTWComplexToComplexImageFilter :
public FFTComplexToComplexImageFilter< TPixel, Dimension >
{
	// This is an empty, general definintion. The FFTW API requires that
	// we create separate specialized classes for float and double implementations.
}

#if defined(USE_FFTWF)
template < unsigned int Dimension >
class FFTWComplexToComplexImageFilter< float, Dimension >:
public FFTComplexToComplexImageFilter< float, Dimension >
{
public:
	// standard itk typedefs
	typedef float TPixel;
	typedef FFTWComplexToComplexImageFilter Self;
	typedef FFTComplexToComplexImageFilter< float, Dimension > Superclass;
	typedef SmartPointer< Self > Pointer;
	typedef SmartPointer< const Self > ConstPointer;

	itkNewMacro(Self);
	itkTypeMacro(FFTWComplexToComplexImageFilter, FFTComplexToComplexImageFilter);

	virtual void GenerateData(); // generate data from input

protected:
	FFTWComplexToComplexImageFilter()
		: m_PlanComputed(false) 
	{}
		
	virtual ~FFTWComplexToComplexImageFilter() 
	{
		if (this->m_PlanComputed)
			fftwf_destroy_plan(this->m_Plan);
	}
	
private:
	bool m_PlanComputed;	
	fftwf_plan m_Plan;

	// Purposefully not implemented
	FFTWComplexToComplexImageFilter(const Self& other);
	void operator=(const Self& other);
};

#endif // defined(FFTWF)

#if defined(FFTWD)
template < unsigned int Dimension >
class FFTWComplexToComplexImageFilter< double, Dimension > :
public FFTComplexToComplexImageFilter< double, Dimension >
{
public:
	// standard itk typedefs
	typedef double TPixel;
	typedef FFTWComplexToComplexImageFilter Self;
	typedef FFTComplexToComplexImageFilter< double, Dimension > Superclass;
	typedef SmartPointer< Self > Pointer;
	typedef SmartPointer< const Self > ConstPointer;

	itkNewMacro(Self);
	itkTypeMacro(FFTWComplexToComplexImageFilter, FFTComplexToComplexImageFilter);

	virtual void GenerateData(); // generate data from input

protected:
	FFTWComplexToComplexImageFilter()
		: m_PlanComputed(false) 
	{}
		
	virtual ~FFTWComplexToComplexImageFilter() 
	{
		if (this->m_PlanComputed)
			fftw_destroy_plan(this->m_Plan);
	}
	
private:
	bool m_PlanComputed;	
	fftw_plan m_Plan;

	// Purposefully not implemented
	FFTWComplexToComplexImageFilter(const Self& other);
	void operator=(const Self& other);	
};
#endif // defined(FFTWD)

} // end itk namespace

#endif // FFTWF or FFTWD

//----- Implementation -----//

// Borrowed heavily from itkFFTWComplexConjugateToRealImageFilter and
// itkFFTWRealToComplexConjugateImageFilter

#if defined(USE_FFTWF) || defined(USE_FFTWD)
namespace itk
{
#if defined(USE_FFTWF)
	
template < unsigned int Dimension >
void FFTWComplexToComplexImageFilter< float, Dimension >
::GenerateData()
{
	// Get input and output
	TInputImageType::ConstPointer input = this->GetInput();
	TOutputImageType::Pointer output = this->GetOutput();
	
	// Validate that we can run
	if (!(input && output))
		return;
	
	// Get size and dimensionality
	const typename TInputImageType::SizeType& inputSize = input->GetLargestPossibleRegion().GetSize();
	const unsigned int dims = input->GetImageDimension();
	
	// Allocate output
	output->SetBufferedRegion(output->GetRequestedRegion());
	output->Allocate();
	
	// Setup FFTW parameters
	unsigned int total_size = 1;
	int direction = this->m_Forward ? FFTW_FORWARD : FFTW_BACKWARD;
	unsigned int flags = FFTW_ESTIMATE | PRESERVE_INPUT;
	
	// Recast input and output for FFTW
	std::complex< TPixel > *pIn = const_cast< std::complex<TPixel> *>(input->GetBufferPointer());
	fftwf_complex *pInFftw = reinterpret_cast< fftwf_complex * >(pIn);
	std::complex< TPixel > *pOut = output->GetBufferPointer();
	fftwf_complex *pOutFftw = reinterpret_cast< fftwf_complex *>(pOut);
	
	// Create plan, based on data dimensionality
	switch(dims)
	{
	case 1:
		this->m_Plan = fftwf_plan_dft_1d(
			inputSize[0], 
			pInFftw, pOutFftw, 
			direction, flags);
		break;
	case 2:
		this->m_Plan = fftwf_plan_dft_2d(
			inputSize[1], inputSize[0],
			pInFftw, pOutFftw,
			direction, flags);
		break;
	case 3:
		this->m_Plan = fftwf_plan_dft_3d(
			inputSize[2], inputSize[1], inputSize[0],
			pInFftw, pOutFftw,
			direction, flags);
		break;
	default:
		int *pSizes = new int[dims];
		for (unsigned int i = 0; i < dims; i++)
		{
			pSizes[(dims-1)-i] = inputSize[i];
		}
		this->m_Plan = fftwf_plan_dft(
			dims, pSizes,
			pInFftw, pOutFftw,
			direction, flags);
		delete []pSizes;
	} // end switch
	
	this->m_PlanComputed = true;
	
	// Compute the DFT!
	fftwf_execute(this->m_Plan);
}	
	
#endif // defined(USE_FFTWF)

#if defined(USE_FFTWD)
	
template < unsigned int Dimension >
void FFTWComplexToComplexImageFilter< double, Dimension >
::GenerateData()
{
	// Get input and output
	TInputImageType::ConstPointer input = this->GetInput();
	TOutputImageType::Pointer output = this->GetOutput();
	
	// Validate that we can run
	if (!(input && output))
		return;
	
	// Get size and dimensionality
	const typename TInputImageType::SizeType& inputSize = input->GetLargestPossibleRegion().GetSize();
	const unsigned int dims = input->GetImageDimension();
	
	// Allocate output
	output->SetBufferedRegion(output->GetRequestedRegion());
	output->Allocate();
	
	// Setup FFTW parameters
	unsigned int total_size = 1;
	int direction = this->m_Forward ? FFTW_FORWARD : FFTW_BACKWARD;
	unsigned int flags = FFTW_ESTIMATE | PRESERVE_INPUT;
	
	// Recast input and output for FFTW
	std::complex< TPixel > *pIn = const_cast< std::complex<TPixel> *>(input->GetBufferPointer());
	fftwf_complex *pInFftw = reinterpret_cast< fftw_complex * >(pIn);
	std::complex< TPixel > *pOut = output->GetBufferPointer();
	fftwf_complex *pOutFftw = reinterpret_cast< fftw_complex *>(pOut);
	
	// Create plan, based on data dimensionality
	switch(dims)
	{
	case 1:
		this->m_Plan = fftw_plan_dft_1d(
			inputSize[0], 
			pInFftw, pOutFftw, 
			direction, flags);
		break;
	case 2:
		this->m_Plan = fftw_plan_dft_2d(
			inputSize[1], inputSize[0],
			pInFftw, pOutFftw,
			direction, flags);
		break;
	case 3:
		this->m_Plan = fftw_plan_dft_3d(
			inputSize[2], inputSize[1], inputSize[0],
			pInFftw, pOutFftw,
			direction, flags);
		break;
	default:
		int *pSizes = new int[dims];
		for (unsigned int i = 0; i < dims; i++)
		{
			pSizes[(dims-1)-i] = inputSize[i];
		}
		this->m_Plan = fftw_plan_dft(
			dims, pSizes,
			pInFftw, pOutFftw,
			direction, flags);
		delete []pSizes;
	} // end switch
	
	this->m_PlanComputed = true;
	
	// Compute the DFT!
	fftw_execute(this->m_Plan);
}	
	
#endif // defined(USE_FFTWD)

} // end itk namespace
#endif // defined(USE_FFTWF) || defined(USE_FFTWD)
