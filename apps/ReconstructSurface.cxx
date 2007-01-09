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
#include "itkRegionOfInterestImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkStatisticsImageFilter.h"

#include "CentralDifferenceImageFilter.h"
#include "DerivativesToSurfaceImageFilter.h"
#include "ImageUtils.h"
#include "Logger.h"
#include "PadFunctorImageFilter.h"
#include "PeakSNRImageToImageMetric.h"

/**
 * Given an image, this test case computes the derivative in both directions (x, y). Then, the
 * original image is reconstructed from these derivatives using Fourier integration.
 */
int main(int argc, char** argv)
{
    // Check command line arguments
    if (argc < 8)
    {
        Logger::error << "Usage:\n\t" << argv[0] << " input padInput outDx outDy outSurface outShort padRadius" << std::endl;
        Logger::error << "\tNote: output files should support float pixel type (e.g. .vtk, .mha)" << std::endl;
        exit(1);
    }
    
    std::string inFile = argv[1];
    std::string padInFile = argv[2];
    std::string dxFile = argv[3];
    std::string dyFile = argv[4];
    std::string outFile = argv[5];
    std::string shortFile = argv[6];
    int padRadius = atoi(argv[7]);
    
    const unsigned int Dimension = 2;
    typedef itk::Image< float, Dimension > InputImageType;
    typedef itk::Image< float, Dimension > OutputImageType;
    typedef itk::Image< std::complex< float >, Dimension > ComplexImageType;
    
    typedef itk::CastImageFilter< InputImageType, OutputImageType > FloatCasterType;
    typedef itk::CastImageFilter< OutputImageType, InputImageType > ShortCasterType;
    
    typedef itk::RecursiveGaussianImageFilter< OutputImageType, OutputImageType > GaussType;
    typedef CentralDifferenceImageFilter< OutputImageType, OutputImageType > DerivType;
    typedef PadFunctorImageFilter< OutputImageType, OutputImageType, CosFunctor > PadType;
    typedef DerivativesToSurfaceImageFilter< OutputImageType > SurfaceType;
    typedef itk::RegionOfInterestImageFilter< OutputImageType, OutputImageType > ROIType;
    
    typedef itk::StatisticsImageFilter< InputImageType > StatsType;
    typedef itk::RescaleIntensityImageFilter< OutputImageType, InputImageType > RescaleType;
    
    typedef PeakSNRImageToImageMetric< InputImageType, InputImageType > PSNRType;
    
    FloatCasterType::Pointer floatCast = FloatCasterType::New();
    PadType::Pointer pad = PadType::New();
    // GaussType::Pointer dx = GaussType::New();
    // GaussType::Pointer dy = GaussType::New();
    DerivType::Pointer dx = DerivType::New();
    DerivType::Pointer dy = DerivType::New();
    
    SurfaceType::Pointer surf = SurfaceType::New();
    ROIType::Pointer roi = ROIType::New();
    StatsType::Pointer stats = StatsType::New();
    RescaleType::Pointer rescale = RescaleType::New();
    
    // Indices we'll need later....
    OutputImageType::IndexType zeroIndex;
    OutputImageType::IndexType offset;
    
    for (unsigned int i = 0; i < Dimension; i++)
    {
        pad->SetPadRadius(i, padRadius);
        zeroIndex[i] = 0;
        offset[i] = padRadius;
    }
    pad->SetBoundaryValue(0);
    
    dx->SetDirection(0);	
    dy->SetDirection(1);	
    // dx->SetFirstOrder();
    // dy->SetFirstOrder();
    // dx->SetSigma(1.0);
    // dy->SetSigma(1.0);
    
    Logger::verbose << "Reading input file" << std::endl;
    InputImageType::Pointer inImage = ReadImage<InputImageType>(inFile);
    floatCast->SetInput(inImage);
    
    Logger::verbose << "Padding input image for better Fourier behavior" << std::endl;
    pad->SetInput(floatCast->GetOutput());
    pad->Update();
    WriteImage(pad->GetOutput(), padInFile);
    
    Logger::verbose << "Adjusting padded image's index" << std::endl;
    PrintRegionInfo<OutputImageType>(pad->GetOutput()->GetLargestPossibleRegion(), "Padded region before");
    OutputImageType::RegionType region = pad->GetOutput()->GetLargestPossibleRegion();
    region.SetIndex(zeroIndex);
    pad->GetOutput()->SetRegions(region);
    PrintRegionInfo<OutputImageType>(pad->GetOutput()->GetLargestPossibleRegion(), "Padded region after");
    
    Logger::verbose << "Taking image derivatives" << std::endl;
    dx->SetInput(pad->GetOutput());
    dy->SetInput(pad->GetOutput());
    WriteImage(dx->GetOutput(), dxFile);
    WriteImage(dy->GetOutput(), dyFile);
        
    Logger::verbose << "Reconstructing image" << std::endl;
    surf->SetInputDx(dx->GetOutput());
    surf->SetInputDy(dy->GetOutput());
    WriteImage(surf->GetOutput(), outFile);
    
    Logger::verbose << "Extracting input image region" << std::endl;
    region = inImage->GetLargestPossibleRegion();
    region.SetIndex(offset);            // the index changed from padding the image
    roi->SetRegionOfInterest(region);
    roi->SetInput(surf->GetOutput());
    
    Logger::verbose << "Rescaling to original image intensities" << std::endl;
    stats->SetInput(inImage);
    stats->Update();
    rescale->SetInput(roi->GetOutput());
    rescale->SetOutputMaximum(stats->GetMaximum());
    rescale->SetOutputMinimum(stats->GetMinimum());
    WriteImage(rescale->GetOutput(), shortFile);
    
    // Print stats
    PrintImageInfo(floatCast->GetOutput(), "Original image");
    PrintImageInfo(pad->GetOutput(), "Padded input image");
    PrintImageInfo(dx->GetOutput(), "Image dx");
    PrintImageInfo(dy->GetOutput(), "Image dy");
    PrintImageInfo(surf->GetOutput(), "Reconstructed image padded");
    PrintImageInfo(roi->GetOutput(), "Reconstructed image ROI");
    PrintImageInfo(rescale->GetOutput(), "Surface, rescaled");
    
    PSNRType::Pointer psnr = PSNRType::New();
    psnr->SetSourceImage(inImage);
    psnr->SetReconstructImage(rescale->GetOutput());
    Logger::info << "RMSE:\t" << psnr->GetRMSE() << "\tPSNR:\t" << psnr->GetPSNR() << std::endl;
    
    PrintRegionInfo<OutputImageType>(floatCast->GetOutput()->GetLargestPossibleRegion(), "Input image region");
    PrintRegionInfo<OutputImageType>(surf->GetOutput()->GetLargestPossibleRegion(), "Reconstructed image region");
    PrintRegionInfo<OutputImageType>(roi->GetOutput()->GetLargestPossibleRegion(), "ROI output region");    
}
