#include <string>

#include "FilePattern.h"
#include "FileSet.h"
#include "IntegrateFlowFieldPipeline.h"
#include "ImageSetReader.h"
#include "Logger.h"
#include "TextPipelineObserver.h"

/**
 * \brief 
 */
int main(int argc, char** argv)
{
    std::string function("IntegrateOpticalFlow");
    
    Logger::debug << function << ": Parsing parameters" << std::endl;
    if (argc < 7)
    {
        Logger::warning << "Usage:\n\t" << argv[0] << " dir formatIn start end formatOut stepSize" << std::endl;
        return 1;
    }

    std::string dir = argv[1];
    std::string formatIn = argv[2];
    int start = atoi(argv[3]);
    int end = atoi(argv[4]);
    std::string formatOut = argv[5];
    float stepSize = argc > 6 ? atof(argv[6]) : 1.0;
    
    Logger::debug << function << ": Setting up image I/O" << std::endl;
    
    typedef IntegrateFlowFieldPipeline::FlowImageType FlowImageType;
    FileSet filesIn(FilePattern(dir, formatIn, start, end));
    FileSet filesOut(FilePattern(dir, formatOut, start, end));
    ImageSetReader<FlowImageType, FlowImageType> video(filesIn);
    
    Logger::debug << function << ": Setting up flow integration pipeline" << std::endl;
    IntegrateFlowFieldPipeline::Pointer pipeline = IntegrateFlowFieldPipeline::New();
    pipeline->SetInput(&video);
    pipeline->SetOutputFiles(filesOut);
    pipeline->SetStepSize(stepSize);
    
    TextPipelineObserver::Pointer observer = TextPipelineObserver::New();
    pipeline->AddObserver(observer.GetPointer());
    
    Logger::debug << function << ": Executing flow integration pipeline" << std::endl;
    pipeline->Update();
}
