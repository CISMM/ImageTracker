#include <string>

#include "itkImage.h"

#include "FilePattern.h"
#include "FileSet.h"
#include "ImageFileSetReader.h"
#include "ImageFileSetTypes.h"
#include "MultiResolutionOpticalFlowPipeline.h"
#include "Logger.h"
#include "TextPipelineObserver.h"

/**
 * \brief Compute the optical flow of an image sequence using the a multi-resolution optical flow algorithm.
 */
int main(int argc, char** argv)
{
    std::string function("MultiResolutionOpticalFlow");
    
    Logger::debug << function << ": Parsing parameters" << std::endl;
    if (argc < 6)
    {
        Logger::warning << "Usage:\n\t" << argv[0] << " dir formatIn start end formatOut [sigmaD] [sigmaI] [regularization] [iterations] [levels]" << std::endl;
        return 1;
    }

    std::string dir = argv[1];
    std::string formatIn = argv[2];
    int start = atoi(argv[3]);
    int end = atoi(argv[4]);
    std::string formatOut = argv[5];
    float sigmaI = argc > 6 ? atof(argv[6]) : 1.0;
    float sigmaD = argc > 7 ? atof(argv[7]) : 2.4;
    float regularization = argc > 8 ? atof(argv[8]) : 10e4;
    int iterations = argc > 9 ? atoi(argv[9]) : 400;
    int levels = argc > 10 ? atoi(argv[10]) : 3;
    
    Logger::debug << function << ": Setting up image I/O" << std::endl;
    FileSet filesIn(FilePattern(dir, formatIn, start, end));
    FileSet filesOut(FilePattern(dir, formatOut, start, end));
    ImageFileSetReader::Pointer images = ImageFileSetReader::New();
    images->SetFileSet(filesIn);
    
    Logger::debug << function << ": Setting up optical flow pipeline" << std::endl;
    MultiResolutionOpticalFlowPipeline::Pointer pipeline = MultiResolutionOpticalFlowPipeline::New();
    pipeline->SetInput(images);
    pipeline->SetOutputFiles(filesOut);
    pipeline->SetSpatialSigma(sigmaD);
    pipeline->SetIntegrationSigma(sigmaI);
    pipeline->SetRegularization(regularization);
    pipeline->SetIterations(iterations);
    pipeline->SetNumberOfLevels(levels);
    
    TextPipelineObserver::Pointer observer = TextPipelineObserver::New();
    pipeline->AddObserver(observer.GetPointer());
    
    Logger::debug << function << ": Executing optical flow pipeline" << std::endl;
    pipeline->Update();
}
