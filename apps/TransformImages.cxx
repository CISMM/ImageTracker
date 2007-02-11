
#include <string>

#include "itkImage.h"
        
#include "ApplyTransformsPipeline.h"
#include "FilePattern.h"
#include "FileSet.h"
#include "ImageSetReader.h"
#include "Logger.h"
#include "RegistrationOutput.h"
#include "TransformGroup.h"

int main(int argc, char** argv)
{
    // Check input arguments
    if (argc < 7)
    {
        Logger::verbose << "Usage:\n\t" << argv[0] << " dir formatIn start end transformFile formatOut" << std::endl;
        exit(1);
    }
    
    typedef itk::Image< unsigned short, 2 > InputImageType;
    typedef itk::Image< float, 2 > InternalImageType;
    
    std::string dir = argv[1];
    std::string formatIn = argv[2];
    int start = atoi(argv[3]);
    int end = atoi(argv[4]);
    std::string transformFile = argv[5];
    std::string formatOut = argv[6];
    
    Logger::verbose << "Setting up file IO" << std::endl;
    FileSet filesIn(FilePattern(dir, formatIn, start, end));
    FileSet filesOut(FilePattern(dir, formatOut, start, end));
    ImageSetReader< InputImageType, InternalImageType > video(filesIn);
    
    Logger::verbose << "Setting up pipeline" << std::endl;
    ApplyTransformsPipeline::Pointer pipeline = ApplyTransformsPipeline::New();
    pipeline->SetInput(&video);
    pipeline->SetTransformFile(transformFile);
    pipeline->SetOutputFiles(filesOut);
    
    Logger::verbose << "Executing pipeline" << std::endl;
    pipeline->Update();
    
    Logger::verbose << "Done." << std::endl;
    
//     TransformGroup::TransformVector* transforms = TransformGroup::LoadTransforms(transformFile);
//     
//     Logger::verbose << "Setting up registration output." << std::endl;
//     RegistrationOutput regout(filesIn, *transforms, filesOut, RegistrationOutput::COMPOSE_PRE);
//     
//     Logger::verbose << "Executing transforms." << std::endl;
//     regout.Update();
}
