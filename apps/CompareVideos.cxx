
#include <string>

#include "itkAbsoluteValueDifferenceImageFilter.h"
#include "itkImage.h"
#include "itkSquaredDifferenceImageFilter.h"

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
    if (argc < 8)
    {
        Logger::error << "Usage:" << std::endl;
        Logger::error << "    " << argv[0] << " dir formatIn1 formatIn2 start end formatOut mean" << std::endl;
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
    
    // Set up IO
    FileSet filesIn1(FilePattern(dir, formatIn1, start, end));
    FileSet filesIn2(FilePattern(dir, formatIn2, start, end));
    FileSet filesOut(FilePattern(dir, formatOut, start, end));
    
    // Typedefs
    typedef itk::Image< unsigned short, 2 > ImageType;
    typedef itk::Image< float, 2 > FloatImageType;
    typedef itk::AbsoluteValueDifferenceImageFilter< ImageType, ImageType, ImageType > DifferenceType;
    // typedef itk::SquaredDifferenceImageFilter< ImageType, ImageType, FloatImageType > DifferenceType;
    typedef NaryMeanImageFilter< ImageType, ImageType > MeanType;
    
    // Create objects
    ImageSetReader<ImageType> video1(filesIn1);
    ImageSetReader<ImageType> video2(filesIn2);
    MeanType::Pointer mean = MeanType::New();
    char label[80];
    
    for (unsigned int i = 0; i < video1.size(); i++)
    {
        DifferenceType::Pointer difference = DifferenceType::New();
        difference->SetInput1(video1[i]);
        difference->SetInput2(video2[i]);
        difference->Update();
        mean->AddInput(difference->GetOutput());
        WriteImage(difference->GetOutput(), filesOut[i]);
        sprintf(label, "Frame %03d", i);
        PrintImageInfo(difference->GetOutput(), std::string(label));
    }
    
    mean->Update();
    WriteImage(mean->GetOutput(), meanFile);
    PrintImageInfo<ImageType>(video1[0], "First frame");
    PrintImageInfo(mean->GetOutput(), "Mean difference");
    
    video1.LogStatistics();
    video2.LogStatistics();
}
