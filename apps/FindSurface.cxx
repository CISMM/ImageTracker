#ifndef USE_FFTWF
#define USE_FFTWF
#endif

#include "itkImage.h"
#include "itkCastImageFilter.h"
#include "itkComplexToImaginaryImageFilter.h"
#include "itkComplexToModulusImageFilter.h"
#include "itkComplexToPhaseImageFilter.h"
#include "itkComplexToRealImageFilter.h"
#include "itkDerivativeImageFilter.h"
#include "itkFFTWRealToComplexConjugateImageFilter.h"
#include "itkFFTWComplexConjugateToRealImageFilter.h"
#include "itkNumericTraits.h"
#include "itkRecursiveGaussianImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"

#include "DerivativesToSurfaceImageFilter.h"
#include "ImageUtils.h"
#include "Logger.h"

int main(int argc, char** argv)
{
    if (argc < 10)
    {
        Logger::error << "Usage:\n\t" << argv[0] << " imageIn modOut phaseOut realOut imagOut ifftOut dxOut dyOut surfOut" << std::endl;
        Logger::error << "\tNote: The derivative images should support floating point pixels (e.g. mha)." << std::endl;
        exit(1);
    }
    
    Logger::verbose << "Parsing command line arguments." << std::endl;
    std::string fileIn      = argv[1];
    std::string modOut      = argv[2];
    std::string phaseOut    = argv[3];
    std::string realOut     = argv[4];
    std::string imagOut     = argv[5];
    std::string ifftOut     = argv[6];
    std::string dxOut       = argv[7];
    std::string dyOut       = argv[8];
    std::string surfOut     = argv[9];
    
    const unsigned int Dimension = 2;
    typedef itk::Image< unsigned short, Dimension > InputImageType;
    typedef itk::Image< unsigned char, Dimension > CharImageType;
    typedef itk::ImageRegionIterator< InputImageType > IteratorType;
    typedef itk::Image< float, Dimension > FloatImageType;
    
    typedef itk::FFTWRealToComplexConjugateImageFilter< FloatImageType::PixelType, Dimension > FFTType;
    typedef itk::FFTWComplexConjugateToRealImageFilter< FloatImageType::PixelType, Dimension > IFFTType;
    typedef itk::ComplexToModulusImageFilter< FFTType::OutputImageType, FloatImageType > ModulusType;
    typedef itk::ComplexToPhaseImageFilter< FFTType::OutputImageType, FloatImageType > PhaseType;
    typedef itk::ComplexToRealImageFilter< FFTType::OutputImageType, FloatImageType > RealType;
    typedef itk::ComplexToImaginaryImageFilter< FFTType::OutputImageType, FloatImageType > ImaginaryType;
    
    // typedef itk::RecursiveGaussianImageFilter< FloatImageType, FloatImageType > DerivType;
    typedef itk::DerivativeImageFilter< FloatImageType, FloatImageType > DerivType;
    typedef DerivativesToSurfaceImageFilter< FloatImageType > SurfaceType;
    
    typedef itk::CastImageFilter< InputImageType, FloatImageType > FloatCastType;
    typedef itk::RescaleIntensityImageFilter< FloatImageType, CharImageType > RescaleType;
    
    FloatCastType::Pointer castToFloat = FloatCastType::New();
    
    FFTType::Pointer fft = FFTType::New();
    ModulusType::Pointer modulus = ModulusType::New();
    PhaseType::Pointer phase = PhaseType::New();
    RealType::Pointer real = RealType::New();
    ImaginaryType::Pointer imaginary = ImaginaryType::New();
    IFFTType::Pointer ifft = IFFTType::New();
    
    DerivType::Pointer dx = DerivType::New();
    DerivType::Pointer dy = DerivType::New();
    SurfaceType::Pointer surf = SurfaceType::New();
    
    RescaleType::Pointer rescale = RescaleType::New();
    rescale->SetOutputMinimum(itk::NumericTraits< RescaleType::OutputImageType::PixelType >::min());
    rescale->SetOutputMaximum(itk::NumericTraits< RescaleType::OutputImageType::PixelType >::max());
    
    try
    {
        Logger::verbose << "Testing FFT library." << std::endl;
        InputImageType::Pointer image = ReadImage<InputImageType>(fileIn);
        castToFloat->SetInput(image);
        PrintImageInfo(castToFloat->GetOutput(), "Input image as float");
        Logger::verbose << "Computing FFT...";
        fft->SetInput(castToFloat->GetOutput());
        fft->Update();
        Logger::verbose << "done." << std::endl;
        
        Logger::verbose << "Writing FFT images..." << std::endl;
        
        Logger::verbose << "\tModulus image" << std::endl;
        modulus->SetInput(fft->GetOutput());        
        rescale->SetInput(modulus->GetOutput());
        WriteImage(rescale->GetOutput(), modOut);
        
        Logger::verbose << "\tPhase image" << std::endl;
        phase->SetInput(fft->GetOutput());
        rescale->SetInput(phase->GetOutput());
        WriteImage(rescale->GetOutput(), phaseOut);
        
        Logger::verbose << "\tReal image" << std::endl;
        real->SetInput(fft->GetOutput());
        rescale->SetInput(real->GetOutput());
        WriteImage(rescale->GetOutput(), realOut);
        
        Logger::verbose << "\tImaginary image" << std::endl;
        imaginary->SetInput(fft->GetOutput());
        rescale->SetInput(imaginary->GetOutput());
        WriteImage(rescale->GetOutput(), imagOut);
        
        Logger::verbose << "\tIFFT of frequency image" << std::endl;
        ifft->SetInput(fft->GetOutput());
        rescale->SetInput(ifft->GetOutput());
        WriteImage(rescale->GetOutput(), ifftOut);
        PrintImageInfo(ifft->GetOutput(), "IFFT image as float");
    }
    catch (itk::ExceptionObject &err)
    {
        std::cerr << "Error!  " << err.what() << std::endl;
        std::cerr << err << std::endl;
        exit(1);
    }

    Logger::verbose << "Finding derivative of input image" << std::endl;
    dx->SetDirection(0);
    dx->SetOrder(1);
    dy->SetDirection(1);
    dy->SetOrder(1);
    // dx->SetSigma(2.0);
    // dy->SetSigma(2.0);
    dx->SetInput(castToFloat->GetOutput());
    dy->SetInput(castToFloat->GetOutput());
    WriteImage(dx->GetOutput(), dxOut);
    WriteImage(dy->GetOutput(), dyOut);
    
    Logger::verbose << "Finding surface from derivative estimates" << std::endl;
    surf->SetInputDx(dx->GetOutput());
    surf->SetInputDy(dy->GetOutput());
    PrintImageInfo(surf->GetOutput(), "Calculated surface as float");
    rescale->SetInput(surf->GetOutput());
    WriteImage(rescale->GetOutput(), surfOut);
}
