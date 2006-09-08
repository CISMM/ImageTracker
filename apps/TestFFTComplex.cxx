#ifndef USE_FFTWD
#define USE_FFTWD
#endif

#ifndef USE_FFTWF
#define USE_FFTWF
#endif

#include <complex>
#include <string>

#include "itkCastImageFilter.h"
#include "itkComplexToImaginaryImageFilter.h"
#include "itkComplexToRealImageFilter.h"
#include "itkImage.h"
#include "itkShiftScaleImageFilter.h"

#include "itkFFTWComplexToComplexImageFilter.h"
#include "itkRealToComplexImageFilter.h"
#include "ImageUtils.h"
#include "Logger.h"

/**
 * Tests if the Complex-to-Complex FFT implementation is working. Given an input image,
 * this test case computes the full Fourier transform of it, treating it as a complex
 * image (granted, with zero imaginary components). The FFT of the result is then taken
 * to recover the original image.
 */
int main(int argc, char** argv)
{
    // Check command line arguments
    if (argc < 5)
    {
        Logger::error << "Usage:\n\t" << argv[0] << " input outReal outImaginary output" << std::endl;
        Logger::error << "\tNote: the output files should support float pixel types (e.g. vtk, mha)." << std::endl;
        exit(1);
    }
    
    std::string inFile = argv[1];
    std::string realFile = argv[2];
    std::string imagFile = argv[3];
    std::string outFile = argv[4];
    
    typedef double NumericType;
    typedef itk::Image< unsigned short, 2 > InputImageType;
    typedef itk::Image< NumericType, 2 > FloatImageType;
    typedef itk::Image< std::complex< NumericType >, 2 > ComplexImageType;
    
    typedef itk::CastImageFilter< InputImageType, FloatImageType > FloatCasterType;
    typedef itk::CastImageFilter< FloatImageType, InputImageType > ShortCasterType;

    typedef itk::RealToComplexImageFilter< NumericType, 2 > ComplexConvertType;
    typedef itk::FFTWComplexToComplexImageFilter< NumericType, 2 > FourierType;

    typedef itk::ComplexToRealImageFilter< ComplexImageType, FloatImageType > RealPartType;
    typedef itk::ComplexToImaginaryImageFilter< ComplexImageType, FloatImageType > ImaginaryPartType;
    typedef itk::ShiftScaleImageFilter< FloatImageType, FloatImageType > ScaleType;
    
    FloatCasterType::Pointer floatCast = FloatCasterType::New();
    ComplexConvertType::Pointer complexCast = ComplexConvertType::New();
    FourierType::Pointer fwdDFT = FourierType::New();
    FourierType::Pointer backDFT = FourierType::New();
    RealPartType::Pointer realPart = RealPartType::New();
    ImaginaryPartType::Pointer imagPart = ImaginaryPartType::New();
    RealPartType::Pointer realResult = RealPartType::New();
    ScaleType::Pointer scale = ScaleType::New();
    
    // Construct pipeline
    fwdDFT->SetForward();
    backDFT->SetBackward();
    
    complexCast->SetInput(floatCast->GetOutput());
    fwdDFT->SetInput(complexCast->GetOutput());
    realPart->SetInput(fwdDFT->GetOutput());
    imagPart->SetInput(fwdDFT->GetOutput());
    backDFT->SetInput(fwdDFT->GetOutput());
    realResult->SetInput(backDFT->GetOutput());
    scale->SetInput(realResult->GetOutput());
    
    // Read input
    InputImageType::Pointer inImage = ReadImage<InputImageType>(inFile);
    floatCast->SetInput(inImage);
    
    // Setup intensity scaling for two DFTs.
    InputImageType::SizeType size = inImage->GetLargestPossibleRegion().GetSize();
    double area = size[0] * size[1];
    scale->SetScale((1.0/area));
    
    // Print stats
    PrintImageInfo(floatCast->GetOutput(), "Input image as float");
    PrintImageInfo(realPart->GetOutput(), "DFT real component");
    PrintImageInfo(imagPart->GetOutput(), "DFT imaginary component");
    PrintImageInfo(scale->GetOutput(), "Two DFT result");
    
    // Save output
    WriteImage(realPart->GetOutput(), realFile);
    WriteImage(imagPart->GetOutput(), imagFile);
    WriteImage(scale->GetOutput(), outFile);
}
