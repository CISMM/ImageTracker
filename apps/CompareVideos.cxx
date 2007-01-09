
#include <string>

#include "itkAbsImageFilter.h"
#include "itkAbsoluteValueDifferenceImageFilter.h"
#include "itkDivideImageFilter.h"
#include "itkImage.h"
#include "itkImageDuplicator.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkSquaredDifferenceImageFilter.h"
#include "itkSubtractImageFilter.h"

#include "FilePattern.h"
#include "FileSet.h"
#include "ImageSetReader.h"
#include "ImageUtils.h"
#include "Logger.h"
#include "NaryMeanImageFilter.h"
#include "PeakSNRImageToImageMetric.h"

/**
 * Compares two input videos, creating a third video that represents the
 * comparison metric at each frame. Additionally, a summary image is also
 * created that shows the mean difference metric value at each pixel.
 */
int main(int argc, char** argv)
{
    if (argc < 8)
    {
        Logger::error << "Usage:\n\t" << argv[0] << " dir formatIn1 formatIn2 start end meanComp meanShort" << std::endl;
        Logger::error << "\tmean image should be in float-compatible format (mha, vtk)." << std::endl;
        exit(1);
    }
    
    // Grab parameters
    std::string dir = argv[1];
    std::string formatIn1 = argv[2];
    std::string formatIn2 = argv[3];
    int start = atoi(argv[4]);
    int end = atoi(argv[5]);
    std::string meanFile = argv[6];
    std::string meanDispFile = argv[7];
    
    // Set up IO
    FileSet filesIn1(FilePattern(dir, formatIn1, start, end));
    FileSet filesIn2(FilePattern(dir, formatIn2, start, end));
    
    // Typedefs
    typedef itk::Image< unsigned short, 2 > InputImageType;
    typedef itk::Image< float, 2 > FloatImageType;
    // typedef itk::AbsoluteValueDifferenceImageFilter< ImageType, ImageType, ImageType > DifferenceType;
    // typedef itk::SquaredDifferenceImageFilter< ImageType, ImageType, FloatImageType > DifferenceType;
    typedef itk::SubtractImageFilter< FloatImageType, FloatImageType, FloatImageType > DifferenceType;
    typedef itk::DivideImageFilter< FloatImageType, FloatImageType, FloatImageType > DivideType;
    typedef itk::ImageDuplicator< FloatImageType > CopyType;
    typedef NaryMeanImageFilter< FloatImageType, FloatImageType > MeanType;
    typedef itk::RescaleIntensityImageFilter< FloatImageType, InputImageType > RescaleType;
    typedef PeakSNRImageToImageMetric< FloatImageType, FloatImageType > PSNRType;
    
    // Create objects
    ImageSetReader<InputImageType, FloatImageType> video1(filesIn1);
    ImageSetReader<InputImageType, FloatImageType> video2(filesIn2);
    DifferenceType::Pointer difference = DifferenceType::New();
    DivideType::Pointer divide = DivideType::New();
    CopyType::Pointer copy = CopyType::New();
    MeanType::Pointer mean = MeanType::New();
    PSNRType::Pointer psnr = PSNRType::New();
    char label[80];
    double minRMSE = itk::NumericTraits<double>::max(), maxRMSE = itk::NumericTraits<double>::min();
    double minPSNR = itk::NumericTraits<double>::max(), maxPSNR = itk::NumericTraits<double>::min();
    double sumRMSE = 0;
    double sumPSNR = 0;
    
    for (unsigned int i = 0; i < video1.size(); i++)
    {
        // We want to compute (recon - orig)/orig because the sign
        // of this percent difference metric then matches the 
        // direction of the intensity change.
        difference->SetInput1(video2[i]);
        difference->SetInput2(video1[i]);
        divide->SetInput1(difference->GetOutput());
        divide->SetInput2(video1[i]);
        divide->Update();
        
        // Add image to mean computation
        copy->SetInputImage(divide->GetOutput());
        copy->Update();
        mean->PushBackInput(copy->GetOutput());
        
        // Compute PSNR
        psnr->SetSourceImage(video1[i]);
        psnr->SetReconstructImage(video2[i]);
        minRMSE = psnr->GetRMSE() < minRMSE ? psnr->GetRMSE() : minRMSE;
        maxRMSE = psnr->GetRMSE() > maxRMSE ? psnr->GetRMSE() : maxRMSE;
        sumRMSE += psnr->GetRMSE();
        minPSNR = psnr->GetPSNR() < minPSNR ? psnr->GetPSNR() : minPSNR;
        maxPSNR = psnr->GetPSNR() > maxPSNR ? psnr->GetPSNR() : maxPSNR;
        sumPSNR += psnr->GetPSNR();
    }
    
    Logger::debug << "\tmin\tmax\tmean" << std::endl;
    Logger::debug << "RMSE\t" << minRMSE << "\t" << maxRMSE << "\t" << (sumRMSE / video1.size()) << std::endl;
    Logger::debug << "PSNR\t" << minPSNR << "\t" << maxPSNR << "\t" << (sumPSNR / video1.size()) << std::endl;
    
    mean->Update();
    WriteImage(mean->GetOutput(), meanFile);
    PrintImageInfo<FloatImageType>(video1[0], "First frame");
    PrintImageInfo(mean->GetOutput(), "Mean difference ratio");
    
    RescaleType::Pointer rescale = RescaleType::New();
    rescale->SetOutputMinimum(itk::NumericTraits< InputImageType::PixelType >::min());
    rescale->SetOutputMaximum(itk::NumericTraits< InputImageType::PixelType >::max());
    rescale->SetInput(mean->GetOutput());
    WriteImage(rescale->GetOutput(), meanDispFile);
    
    video1.LogStatistics();
    video2.LogStatistics();
}
