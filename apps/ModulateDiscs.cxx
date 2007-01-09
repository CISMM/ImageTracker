#include <string.h>

#include "itkImage.h"

#include "FilePattern.h"
#include "FileSet.h"
#include "ImageSetReader.h"
#include "ImageUtils.h"
#include "Logger.h"
#include "ModulateRegionImageFilter.h"

int main(int argc, char** argv)
{
    Logger::verbose << "Checking parameters" << std::endl;
    if (argc < 8)
    {
        Logger::error << "Usage:\n\t" << " dir formatIn start end formatOut initRadius levels" << std::endl;
        exit(1);
    }
    
    Logger::verbose << "Parsing parameters" << std::endl;
    std::string dir         = argv[1];
    std::string formatIn    = argv[2];
    int start               = atoi(argv[3]);
    int end                 = atoi(argv[4]);
    std::string formatOut   = argv[5];
    int initRadius          = atoi(argv[6]);
    int levels              = atoi(argv[7]);
    
    double factor = 1.0 / levels;
    
    FileSet filesIn(FilePattern(dir, formatIn, start, end));
    FileSet filesOut(FilePattern(dir, formatOut, start, end));
    
    typedef itk::Image<unsigned short, 2> ImageType;
    typedef ModulateRegionImageFilter<ImageType> ModulateType;
    
    Logger::verbose << "Setting up input video" << std::endl;
    ImageSetReader<ImageType> video(filesIn);
    ModulateType::Pointer modulate = ModulateType::New();
    
    ImageType::SizeType size = video[0]->GetLargestPossibleRegion().GetSize();
    ImageType::PointType center;
    center[0] = size[0] / 2;
    center[1] = size[1] / 2;
    
    Logger::verbose << "Creating attenuation regions" << std::endl;
    for (int i = 1; i <= levels; i++)
    {
        modulate->AddRegion(center, i * initRadius, factor);
    }
    
    Logger::verbose << "Processing video" << std::endl;
    for (unsigned int i = 0; i < video.size(); i++)
    {
        modulate->SetInput(video[i]);
        WriteImage(modulate->GetOutput(), filesOut[i]);
    }
    
    Logger::verbose << "Done" << std::endl;
}
