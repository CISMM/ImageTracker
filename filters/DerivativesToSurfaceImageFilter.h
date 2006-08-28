#pragma once

#include "itkImageToImageFilter.h"

/**
 * Computes the surface of a function given estimates of the surface's
 * partial derivatives.  The surface computed is the closest smooth
 * surface to that described by the derivatives.  This is accomplished
 * using Fourier analysis as described in Frankot & Chellappa, "A Method
 * for Enforcing Integrability in Shape from Shading Algorithms", IEEE
 * PAMI 1988.  Briefly: given the estimates of the partial derivatives 
 * (Ix, Iy) of a function I, we compute I.  First, compute the fast Fourier
 * transform (FFT) of Ix and Iy, yielding a set of coefficients Cx(wx, wy)
 * and Cy(wx, wy).  Compute a new set of coefficients from Cx and Cy by
 * C(wx, wy) = (-iwx * Cx(wx, wy) - iwy * Cy(wx, wy)) / (wx^2 + wy^2).  The
 * inverse Fourier tranform (IFFT) of C is the surface I.
 *
 * For simplicity in dealing with complex image types in the frequency domain,
 * this filter generates an image of the same type as the input, and so has
 * only one template parameter--the real image type.
 */
template < class TInput >
class DerivativesToSurfaceImageFilter :
public itk::ImageToImageFilter< TInput, TInput >
{
public:
    // Standard ITK typedefs
    typedef DerivativesToSurfaceImageFilter Self;
    typedef itk::ImageToImageFilter< TInput, TInput > Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    
    itkNewMacro(Self);
    itkTypeMacro(DerivativesToSurfaceImageFilter, ImageToImageFilter);
    
    // Helpful typedefs
    typedef TInput InputDxImageType;
    typedef TInput InputDyImageType;
    typedef TInput OutputImageType;
    typedef typename InputDxImageType::PixelType DerivPixelType;
    typedef typename OutputImageType::PixelType OutputPixelType;
    typedef typename InputDxImageType::Pointer InputDxPointer;
    typedef typename InputDyImageType::Pointer InputDyPointer;
    typedef typename OutputImageType::Pointer OutputPointer;
    
    itkStaticConstMacro(ImageDimension, unsigned int, InputDxImageType::ImageDimension);
    
    /**
     * Returns the input x-derivative estimate.
     */
    TInput * GetInputDx()
    {
        return const_cast<TInput *>(this->GetInput(0));
    }
    
    /**
     * Sets the estimate of the x-derivative.
     */
    void SetInputDx(const TInput * image)
    {
        this->SetNthInput(0, const_cast<TInput *>(image));
    }
    
    /**
     * Returns the input y-derivative estimate.
     */
    TInput * GetInputDy()
    {
        return const_cast<TInput *>(this->GetInput(1));
    }
    
    /**
     * Sets the estimate of the y-derivative.
     */
    void SetInputDy(const TInput * image)
    {
        this->SetNthInput(1, const_cast<TInput *>(image));
    }
    
protected:
    DerivativesToSurfaceImageFilter(){}
    virtual ~DerivativesToSurfaceImageFilter(){}
    
    void GenerateData();
    void GenerateInputRequestedRegion() throw (itk::InvalidRequestedRegionError);
    
private:
    // Purposefully not implemented
    DerivativesToSurfaceImageFilter(const Self& other);
    void operator=(const Self& other);
};


/** Implementation **/
// We need to use the fftw libraries instead of ITK's vxl libraries.
#ifndef USE_FFTWF
#define USE_FFTWF
#endif
#include <cmath>
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageRegionIterator.h"
#include "itkFFTWComplexConjugateToRealImageFilter.h"
#include "itkFFTWRealToComplexConjugateImageFilter.h"

#include "ImageUtils.h"
#include "Logger.h"

template <class TInput>
void DerivativesToSurfaceImageFilter<TInput>
::GenerateInputRequestedRegion() throw (itk::InvalidRequestedRegionError)
{
    // This filter should act on the whole of the input images at once, since
    // we're dealing with Fourier transforms
    Superclass::GenerateInputRequestedRegion();
    this->GetInputDx()->SetRequestedRegion(this->GetInputDx()->GetLargestPossibleRegion());
    this->GetInputDy()->SetRequestedRegion(this->GetInputDy()->GetLargestPossibleRegion());
}

template <class TInput>
void DerivativesToSurfaceImageFilter<TInput>
::GenerateData()
{
    Logger::verbose << "DerivativesToSurfaceImageFilter::GenerateData(): begin." << std::endl;
    
    // Define types
    typedef itk::FFTWRealToComplexConjugateImageFilter<DerivPixelType, ImageDimension> FFTDerivType;
    typedef itk::FFTWComplexConjugateToRealImageFilter<OutputPixelType, ImageDimension> IFFTType;
    typedef typename FFTDerivType::OutputImageType::IndexType DerivIndexType;
    typedef itk::ImageRegionConstIteratorWithIndex<typename FFTDerivType::OutputImageType> DerivIteratorType;
    typedef typename FFTDerivType::OutputImageType ComplexImageType;
    typedef typename ComplexImageType::PixelType ComplexPixelType;
    typedef itk::ImageRegionIterator<ComplexImageType> ComplexIteratorType;
    
    // Allocate the output buffer
    Logger::verbose << "\tAllocating output buffer..." << std::endl;
    Superclass::AllocateOutputs();
    
    // Compute FFT of the input derivative estimates
    Logger::verbose << "\tComputing FFT of derivative estimates...";
    typename FFTDerivType::Pointer fftDx = FFTDerivType::New();
    typename FFTDerivType::Pointer fftDy = FFTDerivType::New();
    fftDx->SetInput(this->GetInputDx());
    fftDy->SetInput(this->GetInputDy());
    fftDx->Update();
    fftDy->Update();
    Logger::verbose << "done." << std::endl;
    
    // Setup fft iterators
    DerivIteratorType dxIt(fftDx->GetOutput(), fftDx->GetOutput()->GetLargestPossibleRegion());
    DerivIteratorType dyIt(fftDy->GetOutput(), fftDy->GetOutput()->GetLargestPossibleRegion());
    
    // Find fundamental frequencies for the images
    typename InputDxImageType::SizeType size = this->GetInputDx()->GetLargestPossibleRegion().GetSize();
    float fundFreqX = 2.0 * M_PI / (float)size[0];
    float fundFreqY = 2.0 * M_PI / (float)size[1];
    Logger::verbose << "\tPI: " << M_PI << std::endl;
    Logger::verbose << "\tFundamental frequency: [" << fundFreqX << ", " << fundFreqY << "]" << std::endl;
    
    // Create new complex image and iterator
    Logger::verbose << "\tCreating frequency domain output image..." << std::endl;
    typename ComplexImageType::RegionType region;
    CopyRegion<ComplexImageType, ComplexImageType>(
        fftDx->GetOutput()->GetLargestPossibleRegion(), region);
    typename ComplexImageType::Pointer complexOut = ComplexImageType::New();
    complexOut->SetRegions(region);
    complexOut->Allocate();
    ComplexIteratorType outIt(complexOut, region);
    
    // Compute complex coefficients for the output image
    Logger::verbose << "\tComputing coefficients for output frequency image..." << std::endl;
    for (dxIt.GoToBegin(), dyIt.GoToBegin(), outIt.GoToBegin();
        !(dxIt.IsAtEnd() || dyIt.IsAtEnd() || outIt.IsAtEnd());
        ++dxIt, ++dyIt, ++outIt)
    {
        DerivIndexType index = dxIt.GetIndex();
        float freqX(fundFreqX * index[0]);
        float freqY(fundFreqY * index[1]);        
        float denom = freqX * freqX + freqY * freqY;
        if (denom == 0) // avoid divide by zero
        {
            outIt.Set(ComplexPixelType(0, 0));
        }
        else
        {
            outIt.Set((ComplexPixelType(0,-freqX) * dxIt.Get() + ComplexPixelType(0, -freqY) * dyIt.Get()) / denom);
        }
    }
    
    // Inverse Fourier transform to find the surface
    Logger::verbose << "\tComputing IFFT of frequency image to find surface..." << std::endl;
    typename IFFTType::Pointer ifft = IFFTType::New();
    ifft->SetInput(complexOut);
    ifft->Update();
    
    // Graft output
    Logger::verbose << "\tGrafting output..." << std::endl;
    this->GraftOutput(ifft->GetOutput());
    Logger::verbose << "DerivativesToSurfaceImageFilter::GenerateData(): done." << std::endl;    
}
