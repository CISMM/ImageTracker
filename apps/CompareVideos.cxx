
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

/**
 * Compares two input videos, creating a third video that represents the
 * comparison metric at each frame. Additionally, a summary image is also
 * created that shows the mean difference metric value at each pixel.
 */
int main(int argc, char** argv)
{
    if (argc < 9)
    {
        Logger::error << "Usage:" << std::endl;
        Logger::error << "    " << argv[0] << " dir formatIn1 formatIn2 start end formatOut mean meanDisp" << std::endl;
        exit(1);
    }
    
    // Grab parameters
    std::string dir = argv[1];
    std::string formatIn1 = argv[2];
    std::string formatIn2 = argv[3];
    int start = atoi(argv[4]);
    int end = atoi(argv[5]);
    std::string formatOut = argv[6];
    std::string meanFile = argv[7];
    std::string meanDispFile = argv[8];
    
    // Set up IO
    FileSet filesIn1(FilePattern(dir, formatIn1, start, end));
    FileSet filesIn2(FilePattern(dir, formatIn2, start, end));
    FileSet filesOut(FilePattern(dir, formatOut, start, end));
    
    // Typedefs
    typedef itk::Image< unsigned short, 2 > InputImageType;
    typedef itk::Image< float, 2 > FloatImageType;
    typedef itk::Image< unsigned char, 2 > DisplayImageType;
    // typedef itk::AbsoluteValueDifferenceImageFilter< ImageType, ImageType, ImageType > DifferenceType;
    // typedef itk::SquaredDifferenceImageFilter< ImageType, ImageType, FloatImageType > DifferenceType;
    typedef itk::SubtractImageFilter< FloatImageType, FloatImageType, FloatImageType > DifferenceType;
    typedef itk::DivideImageFilter< FloatImageType, FloatImageType, FloatImageType > DivideType;
    typedef itk::AbsImageFilter< FloatImageType, FloatImageType > AbsType;
    typedef itk::ImageDuplicator< FloatImageType > CopyType;
    typedef NaryMeanImageFilter< FloatImageType, FloatImageType > MeanType;
    typedef itk::RescaleIntensityImageFilter< FloatImageType, DisplayImageType > RescaleType;
    
    // Create objects
    ImageSetReader<InputImageType, FloatImageType> video1(filesIn1);
    ImageSetReader<InputImageType, FloatImageType> video2(filesIn2);
    DifferenceType::Pointer difference = DifferenceType::New();
    DivideType::Pointer divide = DivideType::New();
    // AbsType::Pointer abs = AbsType::New();
    CopyType::Pointer copy = CopyType::New();
    MeanType::Pointer mean = MeanType::New();
    char label[80];
    
    for (unsigned int i = 0; i < video1.size(); i++)
    {
        // We want to compute (recon - orig)/orig because the sign
        // of this percent difference metric then matches the 
        // direction of the intensity change.
        difference->SetInput1(video2[i]);
        difference->SetInput2(video1[i]);
        divide->SetInput1(difference->GetOutput());
        divide->SetInput2(video1[i]);
        
        WriteImage(divide->GetOutput(), filesOut[i]);
        
        // abs->SetInput(divide->GetOutput());
        // abs->Update();
        divide->Update();
        copy->SetInputImage(divide->GetOutput());
        copy->Update();
        mean->PushBackInput(copy->GetOutput());
        
        // sprintf(label, "Frame %03d % difference", i);
        // PrintImageInfo(divide->GetOutput(), std::string(label));
    }
    
    mean->Update();
    WriteImage(mean->GetOutput(), meanFile);
    PrintImageInfo<FloatImageType>(video1[0], "First frame");
    PrintImageInfo(mean->GetOutput(), "Mean abs % difference");
    
    RescaleType::Pointer rescale = RescaleType::New();
    rescale->SetOutputMinimum(itk::NumericTraits< DisplayImageType::PixelType >::min());
    rescale->SetOutputMaximum(itk::NumericTraits< DisplayImageType::PixelType >::max());
    rescale->SetInput(mean->GetOutput());
    WriteImage(rescale->GetOutput(), meanDispFile);
    
    video1.LogStatistics();
    video2.LogStatistics();
}
