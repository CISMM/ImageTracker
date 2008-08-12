#include "RemovePartialOcclusionsPipeline.h"

#include "itkCastImageFilter.h"
#include "itkDivideImageFilter.h"
#include "itkImageDuplicator.h"
#include "itkRegionOfInterestImageFilter.h"
#include "itkShiftScaleImageFilter.h"
#include "itkStatisticsImageFilter.h"
#include "itkThresholdImageFilter.h"

#include "CentralDifferenceImageFilter.h"
#include "DerivativesToSurfaceImageFilter.h"
#include "ImageUtils.h"
#include "Logger.h"
#include "NaryMeanImageFilter.h"
#include "NaryMedianImageFilter.h"
#include "NonZeroLog10ImageFilter.h"
#include "PadFunctorImageFilter.h"
#include "PercentileImageMetric.h"
#include "Power10ImageFilter.h"

void RemovePartialOcclusionsPipeline::Update()
{
    std::string function("RemovePartialOcclusionsPipeline::Update()");
    
    // Check inputs
    if (this->input == NULL ||
        this->input->GetImageCount() == 0 ||
        this->outputFiles.size() == 0)
    {
        Logger::error << function << ": Input or output files not set...aborting" << std::endl;
        return;
    }
    
    // Video I/O components
    Logger::verbose << function << ": Creating input video" << std::endl;
    // VideoType video(this->GetInputFiles());
    
    this->NotifyProgress(0.0, "Initializing");
    
    // Compute the raw transmission metric
    ImageType::Pointer rawTransmit;
    switch (this->GetMetric())
    {
    case Median:
        rawTransmit = this->ComputeTransmissionMedian(this->input);
        break;
    case Mean:
    default:
        rawTransmit = this->ComputeTransmissionMean(this->input);
    }
    
    PrintImageInfo<ImageType>(rawTransmit, "Raw transmission");
    
    // For rescaling transmission map
    typedef PercentileImageMetric< ImageType > PercentileType;
    typedef itk::ShiftScaleImageFilter< ImageType, ImageType > ScaleType;
    
    // For repairing video
    typedef itk::DivideImageFilter< ImageType, ImageType, ImageType > DivideType;
    typedef itk::ThresholdImageFilter< ImageType > ThresholdType;
    
    // Rescale transmission metric to be 1 at the given percentile value
    Logger::info << function << ": Scaling transmission metric to 1.0 at " << this->GetTransmitPercentile() << "th percentile" << std::endl;
    PercentileType::Pointer percentile = PercentileType::New();
    ScaleType::Pointer scale = ScaleType::New();
    percentile->SetInputImage(rawTransmit);
    scale->SetInput(rawTransmit);
    scale->SetScale(1.0 / percentile->GetPercentile(0.01 * this->GetTransmitPercentile()));
    WriteImage(scale->GetOutput(), this->GetTransmissionFile());
    
    // Repair input video
    Logger::verbose << function << ": Repairing input video" << std::endl;
    
    double done = 0.75;
    this->NotifyProgress(done, "Reparing");
    
    DivideType::Pointer divide = DivideType::New();
    ThresholdType::Pointer clamp = ThresholdType::New();
    
    // It is possible that a repaired pixel will have intensity over that
    // allowed by the pixel type.  So, we must threshold to avoid wrap-around
    // when casting to the output image type.
    typedef OutputImageType::PixelType PixelType;
    clamp->SetOutsideValue(std::numeric_limits<PixelType>::max());
    clamp->ThresholdAbove(std::numeric_limits<PixelType>::max());
    
    divide->SetInput2(scale->GetOutput());
    clamp->SetInput(divide->GetOutput());
    
    int count = this->input->GetImageCount();
    for (unsigned int i = 0; 
         i < count && i < this->GetOutputFiles().size(); 
         i++)
    {
        divide->SetInput1(this->input->GetImage(i));
        WriteImage<ImageType, OutputImageType>(clamp->GetOutput(), this->GetOutputFiles()[i], false);
        this->NotifyProgress(done + (1.0-done) * (double(i+1)/count));
    }
}

/**
 * Computes the raw transmission metric using the mean estimator.  This
 * simplifies to computing the pixel-wise geometric mean of image intensities.
 */
RemovePartialOcclusionsPipeline::ImageType::Pointer
     RemovePartialOcclusionsPipeline::ComputeTransmissionMean(ImageFileSet* images)
{
    std::string function("RemovePartialOcclusionsPipeline::ComputeTransmissionMean()");
    Logger::verbose << function << std::endl;
    
    double contrib = 0.75;
    
    typedef NonZeroLog10ImageFilter< ImageType, ImageType > LogType;
    typedef itk::ImageDuplicator< ImageType > CopyType;
    typedef NaryMeanImageFilter< ImageType, ImageType > MeanType;
    typedef Power10ImageFilter< ImageType, ImageType > PowerType;
    
    // Pipeline components
    LogType::Pointer log = LogType::New();
    CopyType::Pointer copy = CopyType::New();
    MeanType::Pointer mean = MeanType::New();
    
    // Compute mean of logarithm of all images
    int size = images->GetImageCount();
    Logger::verbose << function << ": Computing mean of logarithm of all frames" << std::endl;
    for (int i = 0; i < size; i++)
    {
        log->SetInput(images->GetImage(i));
        log->Update();
        copy->SetInputImage(log->GetOutput());
        copy->Update();
        mean->PushBackInput(copy->GetOutput());
        this->NotifyProgress(contrib * double(i+1)/size, "Computing mean...");
    }
    
    // Compute exp of mean log image...this is equivalent to the geometric mean of all images
    Logger::verbose << function << ": Computing raw transmission map" << std::endl;
    PowerType::Pointer power = PowerType::New();
    power->SetInput(mean->GetOutput());
    power->Update();
    
    this->NotifyProgress(contrib, "Computing constant transmission");
    
    return power->GetOutput();
}

/**
 * Computes the tranmission metric using the median estimator.  This involves computing the
 * gradient of the logarithm of all input frames, estimating the gradient using the median,
 * integrating this metric over the image frame, and converting back into linear space.  The
 * logarithm images are padded to improve the behavior of the Fourier transform.
 */
RemovePartialOcclusionsPipeline::ImageType::Pointer
    RemovePartialOcclusionsPipeline::ComputeTransmissionMedian(ImageFileSet* images)
{
    std::string function("RemovePartialOcclusionsPipeline::ComputeTransmissionMedian()");
    Logger::verbose << function << std::endl;
    
    double contribDeriv = 0.50, contribIntegrate = 0.25;
    
    // Typedefs
    typedef NonZeroLog10ImageFilter< ImageType, ImageType > LogType;
    typedef itk::StatisticsImageFilter< ImageType > StatsType;
    typedef PadFunctorImageFilter< ImageType, ImageType, CosFunctor > PadType;
    typedef CentralDifferenceImageFilter< ImageType, ImageType > GradType;
    typedef NaryMedianImageFilter< ImageType, ImageType > MedianType;
    typedef DerivativesToSurfaceImageFilter< ImageType > SurfaceType;
    typedef itk::RegionOfInterestImageFilter< ImageType, ImageType > ROIType;
    typedef Power10ImageFilter< ImageType, ImageType > PowerType;
    typedef itk::ImageDuplicator< ImageType > CopyType;
    
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
    ImageType::RegionType padRegion = 
        images->GetImage(0)->GetLargestPossibleRegion();
    ImageType::IndexType zeroIndex;
    ImageType::IndexType padOffset;
    
    for (unsigned int i = 0; i < ImageDimension; i++)
    {
        zeroIndex[i] = 0;
        padOffset[i] = (long int) (this->GetFourierPadding() * padRegion.GetSize()[i]);
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
    Logger::verbose << function << ": Computing derivative transmission metric" << std::endl;
    unsigned int size = images->GetImageCount();
    for (unsigned int i = 0; i < size; i++)
    {
        // logarithm
        log->SetInput(images->GetImage(i));
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
        this->NotifyProgress(contribDeriv * double(i+1)/size, "Computing median gradient");
    }
    
    medianX->Update();
    medianY->Update();
    
//     PrintImageInfo(dx->GetOutput(), "Final gradX of log");
//     PrintImageInfo(dy->GetOutput(), "Final gradY of log");
//     PrintImageInfo(medianX->GetOutput(), "Median of gradX of log");
//     PrintImageInfo(medianY->GetOutput(), "Median of gradY of log");
    
    // We have to adjust the padded image's index for the Fourier transform to work.
    padRegion = pad->GetOutput()->GetLargestPossibleRegion();
    padRegion.SetIndex(zeroIndex);
    PrintRegionInfo<ImageType>(padRegion, "Padded image region");
    
    // Compute log attenuation by integrating gradient of log attenuation
    Logger::debug << function << ": Integrate surface from derivative estimates" << std::endl;
    SurfaceType::Pointer surface = SurfaceType::New();
    
    // Re-adjust padded regions
    medianX->GetOutput()->SetRegions(padRegion);
    medianY->GetOutput()->SetRegions(padRegion);
    
    this->NotifyProgress(contribDeriv + contribIntegrate * 0.5);
    
    surface->SetInputDx(medianX->GetOutput());
    surface->SetInputDy(medianY->GetOutput());
    
    Logger::debug << function << ": Extract original image region from surface" << std::endl;
    // The pad image filter extends the original image region, and we have changed the index
    // of the image...so the original input image region, with an offset, should extract the 
    // part of the surface we want.
    ROIType::Pointer roi = ROIType::New();
    padRegion = images->GetImage(0)->GetLargestPossibleRegion();
    padRegion.SetIndex(padOffset);
    roi->SetRegionOfInterest(padRegion);
    roi->SetInput(surface->GetOutput());
    
    // Take exponent of log to get transmission metric
    PowerType::Pointer exp = PowerType::New();
    exp->SetInput(roi->GetOutput());
    exp->Update();
    
    this->NotifyProgress(contribDeriv + contribIntegrate, "Computing constant transmission");
    
    return exp->GetOutput();
}
