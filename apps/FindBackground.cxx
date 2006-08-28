
#include <string>

#include "itkAbsoluteValueDifferenceImageFilter.h"
#include "itkImage.h"
#include "itkNaryMaximumImageFilter.h"
#include "itkSquaredDifferenceImageFilter.h"

#include "FilePattern.h"
#include "FileSet.h"
#include "ImageSetReader.h"
#include "ImageUtils.h"
#include "Logger.h"
#include "NaryMinimumImageFilter.h"

/**
 * Given a transmission bright field microscopy video, computes a video that maps
 * likely background regions. In transmission microscopy, a bright uniform light
 * illuminates the back of the sample. Light is absorbed by the sample; regions 
 * that absorb more light darker than regions that absorb less light. If the sample
 * is moving, the maximum value of any given image location (the most light 
 * irradiated at that image location) represents a candidate for a stable, unmoving
 * background. We construct a maximum image that represents the candidate background
 * image. Then, we compare this maximum image to each frame in the original video
 * to determine where in the video this background image can be seen. The background
 * would be visible in regions where the video matches the maximum image over a
 * coherent spatiotemporal patch. That is, the correlation should be a) strong, 
 * b) cover a large enough image patch, and c) persist for a long enough temporal
 * period. It is probable the visible background would move over time as the sample
 * moves.
 */
int main(int argc, char** argv)
{
    // check on provided parameters
    if (argc < 7)
    {
        Logger::error << "Usage:" << std::endl;
        Logger::error << "\t" << argv[0] << " dir formatIn start end formatOut backImage min/max" << std::endl;
        Logger::error << "\t" << "  Use 'max' for bright field, 'min' for fluorescence." << std::endl;
        exit(1);
    }
    
    Logger::verbose << "Parsing inputs" << std::endl;
    std::string dir = argv[1];
    std::string formatIn = argv[2];
    int start = atoi(argv[3]);
    int end = atoi(argv[4]);
    std::string formatOut = argv[5];
    std::string backImage = argv[6];
    std::string minmax = argv[7];
    
    // typedefs
    typedef itk::Image< unsigned short, 2 > ImageType;
    typedef itk::Image< float, 2 > FloatImageType;
    typedef itk::NaryMaximumImageFilter< ImageType, ImageType > MaximumType;
    typedef NaryMinimumImageFilter< ImageType, ImageType > MinimumType;
    typedef itk::AbsoluteValueDifferenceImageFilter< ImageType, ImageType, ImageType > DifferenceType;
    
    // image IO
    FileSet filesIn(FilePattern(dir, formatIn, start, end));
    FileSet filesOut(FilePattern(dir, formatOut, start, end));
    ImageSetReader< ImageType > video(filesIn);
    
    MaximumType::Pointer maximum = MaximumType::New();
    MinimumType::Pointer minimum = MinimumType::New();
    DifferenceType::Pointer difference = DifferenceType::New();
    
    // Compute the maximum image
    Logger::verbose << "Computing maximum & minimum image" << std::endl;
    for (unsigned int i = 0; i < video.size(); i++)
    {
        maximum->PushFrontInput(video[i]);
        minimum->PushFrontInput(video[i]);
    }
    
    if (minmax == "min")
    {
        Logger::verbose << "Using minimum image." << std::endl;
        WriteImage(minimum->GetOutput(), backImage);
        difference->SetInput2(minimum->GetOutput());
    }
    else
    {
        Logger::verbose << "Using maximum image." << std::endl;
        WriteImage(maximum->GetOutput(), backImage);
        difference->SetInput2(maximum->GetOutput());
    }
    
    // Compute the difference at each frame in the video
    Logger::verbose << "Comparing background image to video" << std::endl;
    
    for (unsigned int i = 0; i < video.size(); i++)
    {
        difference->SetInput1(video[i]);
        WriteImage(difference->GetOutput(), filesOut[i]);
    }
}
