#include <string>

#include "itkImage.h"
#include "itkShiftScaleImageFilter.h"
#include "itkStatisticsImageFilter.h"
#include "itkSubtractImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"

#include "FilePattern.h"
#include "FileSet.h"
#include "ImageSetReader.h"
#include "ImageUtils.h"
#include "Logger.h"
#include "NaryMeanImageFilter.h"

/**
 * \brief Perform background subtraction on a sequence of images.
 *
 * Compute a "background" image by finding the mean intensity image from a sequence of images. Subtract
 * this mean image from all images in the sequence, and shift the resulting images to have the same mean
 * as the input image sequence.
 */
int main(int argc, char** argv)
{
    typedef itk::Image< unsigned char, 2 > InputImageType;
    typedef itk::Image< float, 2 > InternalImageType;
    typedef ImageSetReader< InputImageType, InternalImageType > VideoType;
    typedef NaryMeanImageFilter< InternalImageType, InternalImageType > MeanType;
    typedef itk::SubtractImageFilter< InternalImageType, InternalImageType, InternalImageType  > SubtractType;
    typedef itk::ShiftScaleImageFilter< InternalImageType, InternalImageType > ShiftScaleType;
    typedef itk::RescaleIntensityImageFilter< InternalImageType > RescaleType;
    typedef itk::StatisticsImageFilter< InternalImageType > StatsType;
    
    std::string function("SubtractBackground");
    
    Logger::debug << function << ": Checking input" << std::endl;
    if (argc < 7)
    {
        Logger::warning << "Usage:\n\t" << argv[0] << " dir formatIn start end formatOut meanImg" << std::endl;
        exit(1);
    }
    
    std::string dir = argv[1];
    std::string formatIn = argv[2];
    int start = atoi(argv[3]);
    int end = atoi(argv[4]);
    std::string formatOut = argv[5];
    std::string meanImgFile = argv[6];
    
    Logger::debug << function << ": Setting up image I/O" << std::endl;
    FileSet filesIn(FilePattern(dir, formatIn, start, end));
    FileSet filesOut(FilePattern(dir, formatOut, start, end));
    VideoType video(filesIn);
    
    Logger::debug << function << ": Computing video mean" << std::endl;
    MeanType::Pointer mean = MeanType::New();
    for (int i = 0; i < video.size(); i++)
    {
        mean->PushBackInput(video[i]);
    }
    mean->Update();
    PrintImageInfo(mean->GetOutput(), "Mean");
    WriteImage<InternalImageType, InputImageType>(mean->GetOutput(), meanImgFile);
    
    Logger::debug << function << ": Setting up mean subtraction pipeline" << std::endl;
    SubtractType::Pointer subtract = SubtractType::New();
    ShiftScaleType::Pointer shift = ShiftScaleType::New();
    RescaleType::Pointer threshold = RescaleType::New();
    
    subtract->SetInput1(video[0]);
    subtract->SetInput2(mean->GetOutput());
    shift->SetInput(subtract->GetOutput());
    threshold->SetInput(shift->GetOutput());
    
    // Compute the shift to apply to the images from the first image
    Logger::debug << function << ": Finding shift amount" << std::endl;
    StatsType::Pointer stats = StatsType::New();
    stats->SetInput(video[0]);
    stats->Update();
    float meanImg0 = stats->GetMean();
    stats->SetInput(subtract->GetOutput());
    stats->Update();
    float meanSub0 = stats->GetMean();
    
    shift->SetShift(meanImg0 - meanSub0);
    
    // Threshold to the output image range 
    threshold->SetOutputMinimum(std::numeric_limits<InputImageType::PixelType>::min());
    threshold->SetOutputMaximum(std::numeric_limits<InputImageType::PixelType>::max());
    
    Logger::debug << function << ": Computing adjusted images" << std::endl;
    for (int i = 0; i < video.size(); i++)
    {
        subtract->SetInput1(video[i]);
        WriteImage<InternalImageType, InputImageType>(threshold->GetOutput(), filesOut[i]);
    }
}
