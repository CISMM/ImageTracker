#include <string>

#include "itkCastImageFilter.h"
#include "itkCompose2DVectorImageFilter.h"
#include "itkDivideImageFilter.h"
#include "itkImage.h"
#include "itkImageDuplicator.h"
#include "itkNaryFunctorImageFilter.h"
#include "itkRecursiveGaussianImageFilter.h"
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
#include "Power10ImageFilter.h"

int main(int argc, char** argv)
{
/*    if (argc < 8)
    {
        Logger::error << "Usage:\n\t" << argv[0] << " dir formatIn start end meanLogImg attenuateImg formatOut" << std::endl;
        exit(1);
    }
    
    // Parse command line args
    std::string dir = argv[1];
    std::string formatIn = argv[2];
    int start = atoi(argv[3]);
    int end = atoi(argv[4]);
    std::string meanLogFile = argv[5];
    std::string attenuateFile = argv[6];
    std::string formatOut = argv[7];
    
    // Typedefs
    typedef itk::Image< unsigned short, 2 > InputImageType;
    typedef itk::Image< float, 2 > InternalImageType;
    typedef InputImageType::PixelType PixelType;
    
    typedef NonZeroLog10ImageFilter< InternalImageType, InternalImageType > LogType;
    typedef itk::ImageDuplicator< InternalImageType > CopyType;
    typedef NaryMeanImageFilter< InternalImageType, InternalImageType > MeanType;
    typedef Power10ImageFilter< InternalImageType, InternalImageType > PowerType;
    typedef itk::StatisticsImageFilter< InternalImageType > StatsType;
    typedef itk::ShiftScaleImageFilter< InternalImageType, InternalImageType > ScaleType;
    typedef itk::DivideImageFilter< InternalImageType, InternalImageType, InternalImageType > DivideType;
    typedef itk::ThresholdImageFilter< InternalImageType > ThresholdType;
    typedef itk::CastImageFilter < InternalImageType, InputImageType > CastType;
    
    // Pipeline components
    Logger::verbose << "Creating pipeline components" << std::endl;
    FileSet filesIn(FilePattern(dir, formatIn, start, end));
    FileSet filesOut(FilePattern(dir, formatOut, start, end));
    ImageSetReader<InputImageType, InternalImageType> video(filesIn);
    
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
    Logger::verbose << "Computing and rescaling attenuation" << std::endl;
    PowerType::Pointer power = PowerType::New();
    StatsType::Pointer stats = StatsType::New();
    ScaleType::Pointer scale = ScaleType::New();
    
    power->SetInput(mean->GetOutput());
    stats->SetInput(power->GetOutput());
    stats->Update();
    scale->SetScale(1.0 / stats->GetMaximum());
    scale->SetInput(power->GetOutput());
    
    PrintImageInfo(power->GetOutput(), "Raw attenuation");
    PrintImageInfo(scale->GetOutput(), "Scaled attenuation");
    WriteImage(scale->GetOutput(), attenuateFile);
    
    // Repairing video
    Logger::verbose << "Repairing video with computed attenuation" << std::endl;
    DivideType::Pointer divide = DivideType::New();
    ThresholdType::Pointer threshold = ThresholdType::New();
    CastType::Pointer cast = CastType::New();
    
    threshold->SetOutsideValue(std::numeric_limits<PixelType>::max());
    // threshold->ThresholdBelow(std::numeric_limits<PixelType>::min());
    threshold->ThresholdAbove(std::numeric_limits<PixelType>::max());

    divide->SetInput2(scale->GetOutput());
    threshold->SetInput(divide->GetOutput());
    cast->SetInput(threshold->GetOutput());
    
    for (int i = 0; i < video.size(); i++)
    {
        divide->SetInput1(video[i]);
        WriteImage(cast->GetOutput(), filesOut[i]);
    }*/
    
    // Check command line arguments
    if (argc < 10)
    {
        Logger::error << "Usage:" << std::endl;
        Logger::error << "\t" << argv[0] << " dir formatIn start end meangradlogImg rawattenImg attenuateImg formatOut metric" << std::endl;
        Logger::error << "\tNote: the output mean and attenuation images should support float vector types (e.g. mha, vtk)." << std::endl;
        Logger::error << "\tNote: the metric should be one of [mean, median]." << std::endl;
        exit(1);
    }
    
    // Parse command line args
    std::string dir = argv[1];
    std::string formatIn = argv[2];
    int start = atoi(argv[3]);
    int end = atoi(argv[4]);
    std::string meanGradLogFile = argv[5];
    std::string rawAttenuateFile= argv[6];
    std::string attenuateFile = argv[7];
    std::string formatOut = argv[8];
    std::string metric = argv[9];
    
    // typedefs
    const unsigned int Dimension = 2;
    typedef itk::Image< unsigned short, Dimension > InputImageType;
    typedef itk::Image< float, Dimension > InternalImageType;
    typedef InputImageType::PixelType PixelType;
    
    typedef NonZeroLog10ImageFilter< InternalImageType, InternalImageType > LogType;
    typedef CentralDifferenceImageFilter< InternalImageType, InternalImageType > GradType;
    // typedef itk::RecursiveGaussianImageFilter< InternalImageType, InternalImageType > GradType;
    typedef NaryMedianImageFilter< InternalImageType, InternalImageType > MedianType;
    typedef NaryMeanImageFilter< InternalImageType, InternalImageType > MeanType;
    typedef DerivativesToSurfaceImageFilter< InternalImageType > SurfaceType;
    
    // For high-pass filter
    // typedef itk::RecursiveGaussianImageFilter< InternalImageType, InternalImageType > SmoothType;
    // typedef itk::SubtractImageFilter< InternalImageType, InternalImageType, InternalImageType > DifferenceType;
    
    typedef itk::Compose2DVectorImageFilter< InternalImageType > ComposeType;
    typedef itk::ImageDuplicator< InternalImageType > CopyType;
    typedef itk::StatisticsImageFilter< InternalImageType > StatsType;
    
    typedef Power10ImageFilter< InternalImageType, InternalImageType > PowerType;
    typedef itk::ShiftScaleImageFilter< InternalImageType, InternalImageType > ScaleType;
    typedef itk::DivideImageFilter< InternalImageType, InternalImageType, InternalImageType > DivideType;
    typedef itk::ThresholdImageFilter< InternalImageType > ThresholdType;
    typedef itk::CastImageFilter< InternalImageType, InputImageType > CastType;

    // Pipeline components
    Logger::verbose << "Creating pipeline components" << std::endl;
    FileSet filesIn(FilePattern(dir, formatIn, start, end));
    FileSet filesOut(FilePattern(dir, formatOut, start, end));
    ImageSetReader<InputImageType, InternalImageType> video(filesIn);
    
    LogType::Pointer log = LogType::New();
    GradType::Pointer dx = GradType::New();
    GradType::Pointer dy = GradType::New();
    
    Logger::debug << "Metric type:\t" << metric << std::endl;
    MedianType::Pointer medianX = MedianType::New();
    MedianType::Pointer medianY= MedianType::New();
    MeanType::Pointer meanX = MeanType::New();
    MeanType::Pointer meanY = MeanType::New();
    CopyType::Pointer copyX = CopyType::New();
    CopyType::Pointer copyY = CopyType::New();
    
    ComposeType::Pointer compose = ComposeType::New();
    
    dx->SetDirection(0);
    dy->SetDirection(1);
    // dx->SetFirstOrder();
    // dy->SetFirstOrder();
    dx->SetInput(log->GetOutput());
    dy->SetInput(log->GetOutput());
    
    // Compute average gradient of log intensity for all images
    Logger::verbose << "Computing derivative absorption metric" << std::endl;
    for (unsigned int i = 0; i < video.size(); i++)
    {
        // mean of gradient of log
        log->SetInput(video[i]);
        dx->Update();
        dy->Update();
        
        copyX->SetInputImage(dx->GetOutput());
        copyY->SetInputImage(dy->GetOutput());
        copyX->Update();
        copyY->Update();
        
        if (metric == "median")
        {
            medianX->PushBackInput(copyX->GetOutput());
            medianY->PushBackInput(copyY->GetOutput());
        }
        else // mean
        {
            meanX->PushBackInput(copyX->GetOutput());
            meanY->PushBackInput(copyY->GetOutput());
        }
    }
        
    // Average of gradient of logarithm
    PrintImageInfo(dx->GetOutput(), "Final gradX of log");
    PrintImageInfo(dy->GetOutput(), "Final gradY of log");
    if (metric == "median")
    {
        PrintImageInfo(medianX->GetOutput(), "Median of gradX of log");
        PrintImageInfo(medianY->GetOutput(), "Median of gradY of log");
        compose->SetInput1(medianX->GetOutput());
        compose->SetInput2(medianY->GetOutput());
    }
    else
    {
        PrintImageInfo(meanX->GetOutput(), "Mean of gradX of log");
        PrintImageInfo(meanY->GetOutput(), "Mean of gradY of log");
        compose->SetInput1(meanX->GetOutput());
        compose->SetInput2(meanY->GetOutput());
    }
    WriteImage(compose->GetOutput(), meanGradLogFile);
    
/*    // High-pass filter to remove low-frequency distortion
    SmoothType::Pointer smoothXX = SmoothType::New();
    SmoothType::Pointer smoothXY = SmoothType::New();
    SmoothType::Pointer smoothYX = SmoothType::New();
    SmoothType::Pointer smoothYY = SmoothType::New();
    DifferenceType::Pointer diffX = DifferenceType::New();
    DifferenceType::Pointer diffY = DifferenceType::New();
    smoothXX->SetZeroOrder();
    smoothXY->SetZeroOrder();
    smoothYX->SetZeroOrder();
    smoothYY->SetZeroOrder();
    smoothXX->SetDirection(0);
    smoothXY->SetDirection(1);
    smoothYX->SetDirection(0);
    smoothYY->SetDirection(1);
    smoothXX->SetSigma(hpScale);
    smoothXY->SetSigma(hpScale);
    smoothYX->SetSigma(hpScale);
    smoothYY->SetSigma(hpScale);
    smoothXX->SetInput(avgX->GetOutput());
    smoothXY->SetInput(smoothXX->GetOutput());
    smoothYX->SetInput(avgY->GetOutput());
    smoothYY->SetInput(smoothYX->GetOutput());
    diffX->SetInput1(avgX->GetOutput());
    diffY->SetInput1(avgY->GetOutput());
    diffX->SetInput2(smoothXY->GetOutput());
    diffY->SetInput2(smoothYY->GetOutput());
    PrintImageInfo(smoothXY->GetOutput(), "Smoothed avg gradX of log");
    PrintImageInfo(smoothYY->GetOutput(), "Smoothed avg gradY of log");
    PrintImageInfo(diffX->GetOutput(), "High-pass avg gradX of log");
    PrintImageInfo(diffY->GetOutput(), "High-pass avg gradY of log");
    compose->SetInput1(diffX->GetOutput());
    compose->SetInput2(diffY->GetOutput());
    WriteImage(compose->GetOutput(), highpassImageFile);*/
    
    // Compute log attenuation by integrating gradient of log attenuation
    Logger::debug << "Integrate surface from derivative estimates" << std::endl;
    SurfaceType::Pointer surface = SurfaceType::New();
    if (metric == "median")
    {
        surface->SetInputDx(medianX->GetOutput());
        surface->SetInputDy(medianY->GetOutput());
    }
    else
    {
        surface->SetInputDx(meanX->GetOutput());
        surface->SetInputDy(meanY->GetOutput());
    }
    PrintImageInfo(surface->GetOutput(), "Log of attenuation");
    
    // Take exponent of log to get absorption metric
    PowerType::Pointer exp = PowerType::New();
    exp->SetInput(surface->GetOutput());
    PrintImageInfo(exp->GetOutput(), "Raw absorption metric");
    WriteImage(exp->GetOutput(), rawAttenuateFile);
    
    // Rescale attenuation metric to be 1.0 at maximum
    Logger::debug << "Rescaling attenuation metric" << std::endl;
    StatsType::Pointer stats = StatsType::New();
    ScaleType::Pointer scale = ScaleType::New();
    stats->SetInput(exp->GetOutput());
    stats->Update();
    scale->SetScale(1.0 / stats->GetMaximum());
    scale->SetInput(exp->GetOutput());
    PrintImageInfo(scale->GetOutput(), "Rescaled attenuation metric");
    WriteImage(scale->GetOutput(), attenuateFile);

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
