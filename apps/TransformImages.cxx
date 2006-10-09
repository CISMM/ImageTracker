
#include <string>

#include "FilePattern.h"
#include "FileSet.h"
#include "FileSetImageReader.h"
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
    
    std::string dir = argv[1];
    std::string formatIn = argv[2];
    int start = atoi(argv[3]);
    int end = atoi(argv[4]);
    std::string transformFile = argv[5];
    std::string formatOut = argv[6];
    
    Logger::verbose << "Setting up file IO" << std::endl;
    FileSet filesIn(FilePattern(dir, formatIn, start, end));
    FileSet filesOut(FilePattern(dir, formatOut, start, end));
    TransformGroup::TransformVector* transforms = TransformGroup::LoadTransforms(transformFile);
    
    Logger::verbose << "Setting up registration output." << std::endl;
    RegistrationOutput regout(filesIn, *transforms, filesOut, RegistrationOutput::COMPOSE_PRE);
    
    Logger::verbose << "Executing transforms." << std::endl;
    regout.Update();
}