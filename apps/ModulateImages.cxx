
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
    typedef ModulateRegionImageFilter<ImageType> ModulateType;
    
    Logger::verbose << "Setting up input video." << std::endl;
    ImageSetReader<ImageType> video(filesIn);
    ModulateType::Pointer modulate = ModulateType::New();
    
    ImageType::PointType point;
    
    if (type == "size") // vary size
    {
        Logger::verbose << "Setting up modulation regions, varying size." << std::endl;
        point.Fill(100);
        modulate->AddRegion(point, 1.0, 0.8);
        point.Fill(130);
        modulate->AddRegion(point, 2.0, 0.8);
        point.Fill(170);
        modulate->AddRegion(point, 4.0, 0.8);
        point.Fill(220);
        modulate->AddRegion(point, 6.0, 0.8);
        point.Fill(300);
        modulate->AddRegion(point, 8.0, 0.8);
        point.Fill(400);
        modulate->AddRegion(point, 10.0, 0.8);
        point[0] = 90; point[1] = 300;
        modulate->AddRegion(point, 12.0, 0.8);
        point[0] = 200; point[1] = 400;
        modulate->AddRegion(point, 14.0, 0.8);
        point[0] = 280; point[1] = 80;
        modulate->AddRegion(point, 16.0, 0.8);
        point[0] = 420; point[1] = 200;
        modulate->AddRegion(point, 20.0, 0.8);
    }
    else                // vary modulation
    {
        Logger::verbose << "Setting up modulation regions, varying modulation." << std::endl;
        point.Fill(100);
        modulate->AddRegion(point, 3.0, 1.0);
        point.Fill(130);
        modulate->AddRegion(point, 3.0, 0.9);
        point.Fill(160);
        modulate->AddRegion(point, 3.0, 0.8);
        point.Fill(190);
        modulate->AddRegion(point, 3.0, 0.7);
        point.Fill(220);
        modulate->AddRegion(point, 3.0, 0.6);
        point.Fill(250);
        modulate->AddRegion(point, 3.0, 0.5);
        point.Fill(280);
        modulate->AddRegion(point, 3.0, 0.4);
        point.Fill(310);
        modulate->AddRegion(point, 3.0, 0.3);
        point.Fill(340);
        modulate->AddRegion(point, 3.0, 0.2);
        point.Fill(370);
        modulate->AddRegion(point, 3.0, 0.1);
        // point.Fill(400);
        // modulate->AddRegion(point, 3.0, 0.0);
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
