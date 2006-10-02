
#include <cmath>
#include <string>

#include "itkImage.h"

#include "FilePattern.h"
#include "FileSet.h"
#include "ImageSetReader.h"
#include "ImageUtils.h"
#include "Logger.h"
#include "ModulateRegionImageFilter.h"

/**
 * Modifies an input video by modulating regions of the input video's
 * frames. This simulates the effect of debris stuck to the image
 * sensor, lens, or elsewhere in the imaging optical path.
 */
int main(int argc, char** argv)
{
    if (argc < 6)
    {
        Logger::error << "Usage: " << std::endl;
        Logger::error << "    " << argv[0] << " dir formatIn start end formatOut {type}" << std::endl;
        Logger::error << "      if type is 'size', occluders of different sizes are added to the video." << std::endl;
        Logger::error << "      if type is 'mod', occluders of different densities are added to the video." << std::endl;
        exit(1);
    }
    
    // Set up file handling
    std::string dir = argv[1];
    std::string formatIn = argv[2];
    int start = atoi(argv[3]);
    int end = atoi(argv[4]);
    std::string formatOut = argv[5];
    
    std::string type("size"); // default
    if (argc > 6)
        type = argv[6];
    
    FileSet filesIn(FilePattern(dir, formatIn, start, end));
    FileSet filesOut(FilePattern(dir, formatOut, start, end));
    
    typedef itk::Image<unsigned short, 2> ImageType;
    typedef ImageType::PointType PointType;
    typedef ModulateRegionImageFilter<ImageType> ModulateType;
    
    Logger::verbose << "Setting up input video." << std::endl;
    ImageSetReader<ImageType> video(filesIn);
    ModulateType::Pointer modulate = ModulateType::New();
    
    const double PI = 3.14159265;
    PointType center;
    center[0] = 128; center[1] = 128;
    double radius = 100.0;
    PointType point;
    const unsigned int steps = 10;    
    
    if (type == "size") // vary size
    {
        Logger::verbose << "Setting up modulation regions, varying size." << std::endl;
        for (unsigned int i = 0; i < steps; i++)
        {
            double angle(i*2*PI / steps);
            point[0] = static_cast<PointType::ValueType>(center[0] + radius * std::cos(angle));
            point[1] = static_cast<PointType::ValueType>(center[1] + radius * std::sin(angle));
            modulate->AddRegion(point, 2*i + 1.0, 0.7);
        }
    }
    else                // vary modulation
    {
        Logger::verbose << "Setting up modulation regions, varying modulation." << std::endl;
        for (unsigned int i = 1; i < steps; i++)
        {
            double angle(i*2*PI / steps);
            point[0] = static_cast<PointType::ValueType>(center[0] + radius * std::cos(angle));
            point[1] = static_cast<PointType::ValueType>(center[1] + radius * std::sin(angle));
            modulate->AddRegion(point, 6.0, (double) i / steps);
        }
    }
    
    Logger::verbose << "Processing video..." << std::endl;
    for (unsigned int i = 0; i < video.size(); i++)
    {
        modulate->SetInput(video[i]);
        WriteImage(modulate->GetOutput(), filesOut[i]);
    }
    
    PrintImageInfo<ImageType>(video[0], "First original frame");
    modulate->SetInput(video[0]);
    PrintImageInfo(modulate->GetOutput(), "First modified frame");
    
    Logger::verbose << "Done." << std::endl;
    video.LogStatistics();
}
