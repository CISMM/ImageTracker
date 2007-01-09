#include <string>

#include "itkCastImageFilter.h"
#include "itkCompose2DVectorImageFilter.h"
#include "itkDivideImageFilter.h"
#include "itkImage.h"
#include "itkImageDuplicator.h"
#include "itkNaryFunctorImageFilter.h"
#include "itkRecursiveGaussianImageFilter.h"
#include "itkRegionOfInterestImageFilter.h"
#include "itkShiftScaleImageFilter.h"
#include "itkStatisticsImageFilter.h"
#include "itkSubtractImageFilter.h"
#include "itkThresholdImageFilter.h"

#include "CentralDifferenceImageFilter.h"
#include "DerivativesToSurfaceImageFilter.h"
#include "FilePattern.h"
#include "FileSet.h"
#include "ImageSetReader.h"
#include "ImageUtils.h"
#include "Logger.h"
#include "NaryMeanImageFilter.h"
#include "NaryMedianImageFilter.h"
#include "NonZeroLog10ImageFilter.h"
#include "PadFunctorImageFilter.h"
#include "PercentileImageMetric.h"
#include "Power10ImageFilter.h"

// Typedefs
const unsigned int Dimension = 2;
typedef itk::Image< unsigned short, Dimension > InputImageType;
typedef itk::Image< float, Dimension > InternalImageType;
typedef itk::Image< unsigned short, Dimension > OutputImageType;
typedef InputImageType::PixelType PixelType;
typedef ImageSetReader< InputImageType, InternalImageType > VideoType;

InternalImageType::Pointer ComputeTransmissionMean(VideoType& video, std::string meanLogFile);
InternalImageType::Pointer ComputeTransmissionMedian(VideoType& video, std::string mglFile, std::string surfPadFile, double padFrac);

int main(int argc, char** argv)
{
    // Check command line arguments
    if (argc < 11)
    {
        Logger::error << "Usage:" << std::endl;
        Logger::error << "\t" << argv[0] << " dir formatIn start end estLogImg surfacePadImg rawTransmitImg scaleTransmitImg shortTransmitImg formatOut [metric] [scalePct] [padFrac]" << std::endl;
        Logger::error << "\tNote: the output log, surface, and transmission images should support float vector types (e.g. mha, vtk)." << std::endl;
        Logger::error << "\tNote: the metric should be one of [mean, median]." << std::endl;
        Logger::error << "\tNote: scalePct should be between 0 and 1." << std::endl;
        Logger::error << "\tNote: padFrac is what fraction of each image dimension to use for padding, default: 0.5." << std::endl;
        exit(1);
    }
    
    // Parse command line args
    std::string dir = argv[1];
    std::string formatIn = argv[2];
    int start = atoi(argv[3]);
    int end = atoi(argv[4]);
    std::string mglFile = argv[5];
    std::string surfacePadFile = argv[6];
    std::string rawTransmitFile = argv[7];
    std::string scaleTramsmitFile = argv[8];
    std::string shortTransmitFile = argv[9];
    std::string formatOut = argv[10];
    std::string metric("mean");
    double scalePct = 1.0;
    double padFrac = 0.5;
    
    if (argc > 11)
        metric = argv[11];
    if (argc > 12)
        scalePct = atof(argv[12]);
    if (argc > 13)
        padFrac = atof(argv[13]);
    
    // Video I/O components
    Logger::verbose << "Creating video I/O components" << std::endl;
    FileSet filesIn(FilePattern(dir, formatIn, start, end));
    FileSet filesOut(FilePattern(dir, formatOut, start, end));
    VideoType video(filesIn);
    
    // Compute the raw transmission metric
    InternalImageType::Pointer rawTransmit;
    if (metric == "median")
        rawTransmit = ComputeTransmissionMedian(video, mglFile, surfacePadFile, padFrac);
    else
        rawTransmit = ComputeTransmissionMean(video, mglFile);
    
    PrintImageInfo<InternalImageType>(rawTransmit, "Raw transmission");
    WriteImage<InternalImageType>(rawTransmit, rawTransmitFile);
    
    // For rescaling transmission map
    typedef PercentileImageMetric< InternalImageType > PercentileType;
    typedef itk::ShiftScaleImageFilter< InternalImageType, InternalImageType > ScaleType;
    typedef itk::ShiftScaleImageFilter< InternalImageType, OutputImageType > ShortScaleType;
    
    // For repairing video
    typedef itk::DivideImageFilter< InternalImageType, InternalImageType, InternalImageType > DivideType;
    typedef itk::ThresholdImageFilter< InternalImageType > ThresholdType;
    typedef itk::CastImageFilter< InternalImageType, InputImageType > CastType;
    
    // Rescale transmission metric to be 1 at the given percentile value
    Logger::debug << "Scaling transmission metric to 1.0 at " << (scalePct*100) << "th percentile" << std::endl;
    PercentileType::Pointer percentile = PercentileType::New();
    ScaleType::Pointer scale = ScaleType::New();
    percentile->SetInputImage(rawTransmit);
    scale->SetInput(rawTransmit);
    scale->SetScale(1.0 / percentile->GetPercentile(scalePct));
    PrintImageInfo(scale->GetOutput(), "Scaled transmission metric.");
    WriteImage(scale->GetOutput(), scaleTramsmitFile);
    
    Logger::debug << "Percentiles" << std::endl;
    Logger::debug << "\t 70th\t" << percentile->GetPercentile(0.70) << std::endl;
    Logger::debug << "\t 75th\t" << percentile->GetPercentile(0.75) << std::endl;
    Logger::debug << "\t 80th\t" << percentile->GetPercentile(0.80) << std::endl;
    Logger::debug << "\t 85th\t" << percentile->GetPercentile(0.85) << std::endl;
    Logger::debug << "\t 90th\t" << percentile->GetPercentile(0.90) << std::endl;
    Logger::debug << "\t 95th\t" << percentile->GetPercentile(0.95) << std::endl;
    Logger::debug << "\t100th\t" << percentile->GetPercentile(1.00) << std::endl;
    
    Logger::debug << "Scaling transmission for short int output." << std::endl;
    ShortScaleType::Pointer shortScale = ShortScaleType::New();
    shortScale->SetInput(rawTransmit);
    shortScale->SetScale(itk::NumericTraits<ShortScaleType::OutputImageType::PixelType>::max() / percentile->GetPercentile(1.0));
    PrintImageInfo(shortScale->GetOutput(), "Short transmission metric.");
    WriteImage(shortScale->GetOutput(), shortTransmitFile);
        
    // Repair input video
    Logger::debug << "Repairing input video" << std::endl;
    DivideType::Pointer divide = DivideType::New();
    ThresholdType::Pointer clamp = ThresholdType::New();
    CastType::Pointer cast = CastType::New();
    
    // It is possible that a repaired pixel will have intensity over that
    // allowed by the pixel type.  So, we must threshold to avoid wrap-around
    // when casting to the output image type.
    clamp->SetOutsideValue(std::numeric_limits<PixelType>::max());
    clamp->ThresholdAbove(std::numeric_limits<PixelType>::max());
    
    divide->SetInput2(scale->GetOutput());
    clamp->SetInput(divide->GetOutput());
    cast->SetInput(clamp->GetOutput());
    for (unsigned int i = 0; i < video.size(); i++)
    {
        divide->SetInput1(video[i]);
        WriteImage(cast->GetOutput(), filesOut[i]);
    }
}

/**
 * Computes the raw transmission metric using the mean estimator.  This
 * simplifies to computing the pixel-wise geometric mean of image intensities.
 */
InternalImageType::Pointer ComputeTransmissionMean(VideoType& video, std::string meanLogFile)
{
    Logger::verbose << "ComputTransmissionMean()" << std::endl;
    
    typedef NonZeroLog10ImageFilter< InternalImageType, InternalImageType > LogType;
    typedef itk::ImageDuplicator< InternalImageType > CopyType;
    typedef NaryMeanImageFilter< InternalImageType, InternalImageType > MeanType;
    typedef Power10ImageFilter< InternalImageType, InternalImageType > PowerType;
//     typedef itk::StatisticsImageFilter< InternalImageType > StatsType;
//     typedef itk::ShiftScaleImageFilter< InternalImageType, InternalImageType > ScaleType;
//     typedef itk::DivideImageFilter< InternalImageType, InternalImageType, InternalImageType > DivideType;
//     typedef itk::ThresholdImageFilter< InternalImageType > ThresholdType;
//     typedef itk::CastImageFilter < InternalImageType, InputImageType > CastType;
    
    // Pipeline components
    Logger::verbose << "Creating pipeline components" << std::endl;
    LogType::Pointer log = LogType::New();
    CopyType::Pointer copy = CopyType::New();
    MeanType::Pointer mean = MeanType::New();
    
    // Compute mean of logarithm of all images
    Logger::verbose << "Computing mean of logarithm of all frames" << std::endl;
    for (int i = 0; i < video.size(); i++)
    {
        log->SetInput(video[i]);
        log->Update();
        copy->SetInputImage(log->GetOutput());
        copy->Update();
        mean->PushBackInput(copy->GetOutput());
    }
    PrintImageInfo(mean->GetOutput(), "Mean of Log");
    WriteImage(mean->GetOutput(), meanLogFile);
    
    // Compute exp of mean log image...this is equivalent to the geometric mean of all images
    Logger::verbose << "Computing raw transmission" << std::endl;
    PowerType::Pointer power = PowerType::New();
    power->SetInput(mean->GetOutput());
    power->Update();
    return power->GetOutput();
}

/**
 * Computes the tranmission metric using the median estimator.  This involves computing the
 * gradient of the logarithm of all input frames, estimating the gradient using the median,
 * integrating this metric over the image frame, and converting back into linear space.  The
 * logarithm images are padded to improve the behavior of the Fourier transform.
 */
InternalImageType::Pointer ComputeTransmissionMedian(VideoType& video, std::string mglFile, std::string surfPadFile, double padFrac)
{
    Logger::verbose << "ComputeTransmissionMedian()" << std::endl;
    
    // Typedefs
    typedef NonZeroLog10ImageFilter< InternalImageType, InternalImageType > LogType;
    typedef itk::StatisticsImageFilter< InternalImageType > StatsType;
    typedef PadFunctorImageFilter< InternalImageType, InternalImageType, CosFunctor > PadType;
    typedef CentralDifferenceImageFilter< InternalImageType, InternalImageType > GradType;
    // typedef itk::RecursiveGaussianImageFilter< InternalImageType, InternalImageType > GradType;
    typedef NaryMedianImageFilter< InternalImageType, InternalImageType > MedianType;
    typedef DerivativesToSurfaceImageFilter< InternalImageType > SurfaceType;
    typedef itk::RegionOfInterestImageFilter< InternalImageType, InternalImageType > ROIType;
    typedef Power10ImageFilter< InternalImageType, InternalImageType > PowerType;
    
    typedef itk::Compose2DVectorImageFilter< InternalImageType > ComposeType;
    typedef itk::ImageDuplicator< InternalImageType > CopyType;
    
    LogType::Pointer log = LogType::New();
    StatsType::Pointer logStat = StatsType::New();
    PadType::Pointer pad = PadType::New();
    GradType::Pointer dx = GradType::New();
    GradType::Pointer dy = GradType::New();
    
    MedianType::Pointer medianX = MedianType::New();
    MedianType::Pointer medianY= MedianType::New();
    CopyType::Pointer copyX = CopyType::New();
    CopyType::Pointer copyY = CopyType::New();
    
    // Collect some size and index information that we will need to handle
    // image padding properly.  We'll add a padded region of one half the size
    // of the input images in each dimension.  We'll also need a zeroing index
    // and we'll need to keep track of the offset to get back to the original
    // image region.
    InternalImageType::RegionType padRegion = video[0]->GetLargestPossibleRegion();
    InternalImageType::IndexType zeroIndex;
    InternalImageType::IndexType padOffset;
    
    for (unsigned int i = 0; i < Dimension; i++)
    {
        zeroIndex[i] = 0;
        padOffset[i] = (long int) (padFrac * padRegion.GetSize()[i]);
        pad->SetPadRadius(i, padOffset[i]);
    }
    
    // We pad each logarithm image with a smooth transition to the mean of the image.
    pad->SetInput(log->GetOutput());
    logStat->SetInput(log->GetOutput());
    
    // Then, we take the derivative in each direction
    dx->SetDirection(0);
    dy->SetDirection(1);
    // dx->SetFirstOrder();
    // dy->SetFirstOrder();
    dx->SetInput(pad->GetOutput());
    dy->SetInput(pad->GetOutput());
    
    // Compute average gradient of log intensity for all images
    Logger::verbose << "Computing derivative transmission metric" << std::endl;
    for (unsigned int i = 0; i < video.size(); i++)
    {
        // logarithm
        log->SetInput(video[i]);
        logStat->Update();
        pad->SetBoundaryValue(logStat->GetMean());
        
        // derivative
        dx->Update();
        dy->Update();
        copyX->SetInputImage(dx->GetOutput());
        copyY->SetInputImage(dy->GetOutput());
        copyX->Update();
        copyY->Update();
        
        medianX->PushBackInput(copyX->GetOutput());
        medianY->PushBackInput(copyY->GetOutput());
    }
    
    // Estimate of gradient of logarithm
    ComposeType::Pointer compose = ComposeType::New();
    PrintImageInfo(dx->GetOutput(), "Final gradX of log");
    PrintImageInfo(dy->GetOutput(), "Final gradY of log");
    PrintImageInfo(medianX->GetOutput(), "Median of gradX of log");
    PrintImageInfo(medianY->GetOutput(), "Median of gradY of log");
    compose->SetInput1(medianX->GetOutput());
    compose->SetInput2(medianY->GetOutput());
    WriteImage(compose->GetOutput(), mglFile);
    
    // We have to adjust the padded image's index for the Fourier transform to work.
    padRegion = pad->GetOutput()->GetLargestPossibleRegion();
    padRegion.SetIndex(zeroIndex);
    
    // Compute log attenuation by integrating gradient of log attenuation
    Logger::debug << "Integrate surface from derivative estimates" << std::endl;
    SurfaceType::Pointer surface = SurfaceType::New();
    
    Logger::verbose << "Adjusting padded regions" << std::endl;
    PrintRegionInfo<InternalImageType>(medianX->GetOutput()->GetLargestPossibleRegion(), "Region before");
    medianX->GetOutput()->SetRegions(padRegion);
    medianY->GetOutput()->SetRegions(padRegion);
    PrintRegionInfo<InternalImageType>(medianX->GetOutput()->GetLargestPossibleRegion(), "Region after");
    
    surface->SetInputDx(medianX->GetOutput());
    surface->SetInputDy(medianY->GetOutput());
    PrintImageInfo(surface->GetOutput(), "Log of transmission");
    WriteImage(surface->GetOutput(), surfPadFile);
    
    Logger::debug << "Extract original image region from surface" << std::endl;
    // The pad image filter extends the original image region, and we have changed the index
    // of the image...so the original input image region, with an offset, should extract the 
    // part of the surface we want.
    ROIType::Pointer roi = ROIType::New();
    padRegion = video[0]->GetLargestPossibleRegion();
    padRegion.SetIndex(padOffset);
    roi->SetRegionOfInterest(padRegion);
    roi->SetInput(surface->GetOutput());
    
    // Take exponent of log to get transmission metric
    PowerType::Pointer exp = PowerType::New();
    exp->SetInput(roi->GetOutput());
    exp->Update();
    return exp->GetOutput();
}

