#ifndef USE_FFTWF
#define USE_FFTWF
#endif

#ifndef USE_FFTWD
#define USE_FFTWD
#endif

#include <string>

#include "itkCastImageFilter.h"
#include "itkImage.h"
#include "itkRecursiveGaussianImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkStatisticsImageFilter.h"

#include "CentralDifferenceImageFilter.h"
#include "DerivativesToSurfaceImageFilter.h"
#include "ImageUtils.h"
#include "Logger.h"

/**
 * Given an image, this test case computes the derivative in both directions (x, y). Then, the
 * original image is reconstructed from these derivatives using Fourier integration.
 */
int main(int argc, char** argv)
{
    // Check command line arguments
    if (argc < 5)
    {
        Logger::error << "Usage:\n\t" << argv[0] << " input outDx outDy outSurface outShort" << std::endl;
        Logger::error << "\tNote: output files should support float pixel type (e.g. .vtk, .mha)" << std::endl;
        exit(1);
    }
    
    std::string inFile = argv[1];
    std::string dxFile = argv[2];
    std::string dyFile = argv[3];
    std::string outFile = argv[4];
    std::string shortFile = argv[5];
    
    typedef float NumericType;
    typedef itk::Image< unsigned short, 2 > InputImageType;
    typedef itk::Image< NumericType, 2 > FloatImageType;
    typedef itk::Image< std::complex< NumericType >, 2 > ComplexImageType;
    
    typedef itk::CastImageFilter< InputImageType, FloatImageType > FloatCasterType;
    typedef itk::CastImageFilter< FloatImageType, InputImageType > ShortCasterType;
    
    typedef itk::RecursiveGaussianImageFilter< FloatImageType, FloatImageType > GaussType;
    typedef CentralDifferenceImageFilter< FloatImageType, FloatImageType > DerivType;
    typedef DerivativesToSurfaceImageFilter< FloatImageType > SurfaceType;
    
    typedef itk::StatisticsImageFilter< InputImageType > StatsType;
    typedef itk::RescaleIntensityImageFilter< FloatImageType, InputImageType > RescaleType;
    
    FloatCasterType::Pointer floatCast = FloatCasterType::New();
    // GaussType::Pointer dx = GaussType::New();
    // GaussType::Pointer dy = GaussType::New();
    DerivType::Pointer dx = DerivType::New();
    DerivType::Pointer dy = DerivType::New();
    
    SurfaceType::Pointer surf = SurfaceType::New();
    StatsType::Pointer stats = StatsType::New();
    RescaleType::Pointer rescale = RescaleType::New();
    
    dx->SetDirection(0);	
    dy->SetDirection(1);	
    // dx->SetFirstOrder();
    // dy->SetFirstOrder();
    // dx->SetSigma(1.0);
    // dy->SetSigma(1.0);
    
    // Construct pipeline
    dx->SetInput(floatCast->GetOutput());
    dy->SetInput(floatCast->GetOutput());
    surf->SetInputDx(dx->GetOutput());
    surf->SetInputDy(dy->GetOutput());
    rescale->SetInput(surf->GetOutput());
    
    // Read input
    InputImageType::Pointer inImage = ReadImage<InputImageType>(inFile);    
    floatCast->SetInput(inImage);
    stats->SetInput(inImage);
    stats->Update();
    // we want to rescale to the original image's intensity values
    rescale->SetOutputMaximum(stats->GetMaximum());
    rescale->SetOutputMinimum(stats->GetMinimum());
    
    // Print stats
    PrintImageInfo(floatCast->GetOutput(), "Original image");
    PrintImageInfo(dx->GetOutput(), "Image dx");
    PrintImageInfo(dy->GetOutput(), "Image dy");
    PrintImageInfo(surf->GetOutput(), "Reconstructed image");
    PrintImageInfo(rescale->GetOutput(), "Surface, rescaled");
    
    // Save output
    WriteImage(dx->GetOutput(), dxFile);
    WriteImage(dy->GetOutput(), dyFile);
    WriteImage(surf->GetOutput(), outFile);
    WriteImage(rescale->GetOutput(), shortFile);
}
