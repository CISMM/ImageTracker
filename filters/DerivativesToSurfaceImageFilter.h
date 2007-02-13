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
 * C(wx, wy) = (-i sin(wx) * Cx(wx, wy) - i sin(wy) * Cy(wx, wy)) / (wx^2 + wy^2).
 * The inverse Fourier tranform (IFFT) of C is the surface I.
 *
 * For simplicity in dealing with complex image types in the frequency domain,
 * this filter generates an image of the same type as the input, and so has
 * only one template parameter--the real image type.
 * 
 * Note that the Input image class should have a floating point pixel type. This
 * is because under the covers we're computing the FFT of the image data, and the
 * FFT libraries require either float or double data types.
 */
template < class TInput >
class DerivativesToSurfaceImageFilter :
            public itk::ImageToImageFilter< TInput, TInput >
{
public:
    // Standard ITK typedefs
    typedef TInput TInputImageType;
    typedef TInput TOutputImageType;

    typedef DerivativesToSurfaceImageFilter Self;
    typedef itk::ImageToImageFilter< TInput, TInput > Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;

    itkNewMacro(Self);
    itkTypeMacro(DerivativesToSurfaceImageFilter, ImageToImageFilter);

    // Helpful typedefs
    typedef TInput InputImageType;
    typedef TInput OutputImageType;
    typedef typename InputImageType::PixelType PixelType;
    typedef typename InputImageType::Pointer InputPointer;
    typedef typename OutputImageType::Pointer OutputPointer;

    itkStaticConstMacro(ImageDimension, unsigned int, InputImageType::ImageDimension);

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

#include "vnl/vnl_math.h"
#include "itkComplexToRealImageFilter.h"
#include "itkComplexToImaginaryImageFilter.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageRegionIterator.h"
#include "itkShiftScaleImageFilter.h"

#include "itkFFTWComplexToComplexImageFilter.h"
#include "itkRealToComplexImageFilter.h"
#include "ImageUtils.h"
#include "Logger.h"
#include "MathUtils.h"

// visual studio's math library does not define pi

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
    typedef itk::RealToComplexImageFilter< PixelType, ImageDimension > R2CType;
    typedef itk::FFTWComplexToComplexImageFilter< PixelType, ImageDimension > DFTType;
    typedef typename DFTType::OutputImageType ComplexImageType;
    typedef typename ComplexImageType::PixelType ComplexPixelType;
    typedef typename ComplexImageType::IndexType IndexType;

    typedef itk::ComplexToRealImageFilter< ComplexImageType, OutputImageType > C2RType;
    typedef itk::ComplexToImaginaryImageFilter< ComplexImageType, OutputImageType > C2IType;
    typedef itk::ShiftScaleImageFilter< OutputImageType, OutputImageType > ScaleType;
    typedef itk::ImageRegionConstIteratorWithIndex< ComplexImageType > ConstIteratorType;
    typedef itk::ImageRegionIterator< ComplexImageType > IteratorType;

    // Allocate the output buffer
    Logger::verbose << "\tAllocating output buffer..." << std::endl;
    OutputPointer output = this->GetOutput();
    output->SetBufferedRegion(output->GetRequestedRegion());
    output->Allocate();

    // Compute FFT of the input derivative estimates
    typename R2CType::Pointer r2cX = R2CType::New();
    typename R2CType::Pointer r2cY = R2CType::New();
    typename DFTType::Pointer fftDx = DFTType::New();
    typename DFTType::Pointer fftDy = DFTType::New();
    typename C2RType::Pointer c2r = C2RType::New();
    typename C2IType::Pointer c2i = C2IType::New();

    Logger::verbose << "\tComputing FFT of derivative estimates...";
    fftDx->SetForward();
    fftDy->SetForward();
    r2cX->SetInput(this->GetInputDx());
    r2cY->SetInput(this->GetInputDy());
    fftDx->SetInput(r2cX->GetOutput());
    fftDy->SetInput(r2cY->GetOutput());
    fftDx->Update();
    fftDy->Update();
    Logger::verbose << "done." << std::endl;

    c2r->SetInput(fftDx->GetOutput());
    c2i->SetInput(fftDx->GetOutput());
    PrintImageInfo(c2r->GetOutput(), "Real dft of dIdx");
    PrintImageInfo(c2i->GetOutput(), "Imaginary dft of dIdx");
    c2r->SetInput(fftDy->GetOutput());
    c2i->SetInput(fftDy->GetOutput());
    PrintImageInfo(c2r->GetOutput(), "Real dft of dIdy");
    PrintImageInfo(c2i->GetOutput(), "Imaginary dft of dIdy");

    // Setup fft iterators
    ConstIteratorType dxIt(fftDx->GetOutput(), fftDx->GetOutput()->GetLargestPossibleRegion());
    ConstIteratorType dyIt(fftDy->GetOutput(), fftDy->GetOutput()->GetLargestPossibleRegion());

    // Find fundamental frequencies for the images
    typename InputImageType::SizeType size = this->GetInputDx()->GetLargestPossibleRegion().GetSize();
    float fundFreqX = 2.0 * M_PI / (float)size[0];
    float fundFreqY = 2.0 * M_PI / (float)size[1];
    // Logger::verbose << "\tPI: " << M_PI << std::endl;
    Logger::verbose << "\tFundamental frequency: [" << fundFreqX << ", " << fundFreqY << "]" << std::endl;

    // Create new complex image and iterator
    Logger::verbose << "\tCreating frequency domain output image..." << std::endl;
    typename ComplexImageType::RegionType region;
    CopyRegion<ComplexImageType, ComplexImageType>(
        fftDx->GetOutput()->GetLargestPossibleRegion(), region);
    typename ComplexImageType::Pointer complexOut = ComplexImageType::New();
    complexOut->SetRegions(region);
    complexOut->Allocate();
    IteratorType outIt(complexOut, region);

    // Compute complex coefficients for the output image
    Logger::verbose << "\tComputing coefficients for output frequency image..." << std::endl;
    for (dxIt.GoToBegin(), dyIt.GoToBegin(), outIt.GoToBegin();
            !(dxIt.IsAtEnd() || dyIt.IsAtEnd() || outIt.IsAtEnd());
            ++dxIt, ++dyIt, ++outIt)
    {
        IndexType index = dxIt.GetIndex();
        PixelType aX(vcl_sin(fundFreqX * index[0]));
        PixelType aY(vcl_sin(fundFreqY * index[1]));
        PixelType denom = aX * aX + aY * aY;
        if (denom == 0) // avoid divide by zero
        {
            outIt.Set(ComplexPixelType(0, 0));
        }
        else
        {
            //TODO: Why does it seem ax and ay are switched here?
            // outIt.Set((ComplexPixelType(0,-aY) * dxIt.Get() + ComplexPixelType(0, -aX) * dyIt.Get()) / denom);
            outIt.Set((ComplexPixelType(0,-aX) * dxIt.Get() + ComplexPixelType(0, -aY) * dyIt.Get()) / denom);
        }
    }

    c2r->SetInput(complexOut);
    c2i->SetInput(complexOut);
    PrintImageInfo(c2r->GetOutput(), "Real coefficients");
    PrintImageInfo(c2i->GetOutput(), "Imaginary coefficients");

    // Inverse Fourier transform to find the surface
    Logger::verbose << "\tComputing IFFT of frequency image to find surface..." << std::endl;
    typename DFTType::Pointer ifft = DFTType::New();
    typename ScaleType::Pointer scale = ScaleType::New();
    ifft->SetBackward();
    ifft->SetInput(complexOut);

    // Only take the real component...throw away the imaginary
    c2r->SetInput(ifft->GetOutput());

    // We need to scale the output intensity by the product of the image size
    // this is because the fftw library does not account for rescaling DFT data
    // and we've performed two DFTs (forward and backward).
    scale->SetInput(c2r->GetOutput());
    scale->SetScale(1.0 / (size[0] * size[1]));
    scale->Update();

    // Graft output
    Logger::verbose << "\tGrafting output..." << std::endl;
    this->GraftOutput(scale->GetOutput());
    Logger::verbose << "DerivativesToSurfaceImageFilter::GenerateData(): done." << std::endl;
}
