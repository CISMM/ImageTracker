#include <string>

#include "itkImage.h"

#include "FilePattern.h"
#include "FileSet.h"
#include "MultiResolutionOpticalFlowPipeline.h"
#include "ImageSetReader.h"
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
        Logger::warning << "Usage:\n\t" << argv[0] << " dir formatIn start end formatOut [sigma] [weight] [iterations] [relaxation] [levels]" << std::endl;
        return 1;
    }

    std::string dir = argv[1];
    std::string formatIn = argv[2];
    int start = atoi(argv[3]);
    int end = atoi(argv[4]);
    std::string formatOut = argv[5];
    float sigma = argc > 6 ? atof(argv[6]) : 1.0;
    float weight = argc > 7 ? atof(argv[7]) : 10e4;
    int iterations = argc > 8 ? atoi(argv[8]) : 200;
    float relaxation = argc > 9 ? atof(argv[9]) : 1.9;
    int levels = argc > 10 ? atoi(argv[10]) : 3;
    
    Logger::debug << function << ": Setting up image I/O" << std::endl;
    typedef itk::Image< unsigned short, 2 > InputImageType;
    typedef itk::Image< float, 2 > InternalImageType;
    FileSet filesIn(FilePattern(dir, formatIn, start, end));
    FileSet filesOut(FilePattern(dir, formatOut, start, end));
    ImageSetReader<InputImageType, InternalImageType> video(filesIn);
    
    Logger::debug << function << ": Setting up optical flow pipeline" << std::endl;
    MultiResolutionOpticalFlowPipeline::Pointer pipeline = MultiResolutionOpticalFlowPipeline::New();
    pipeline->SetInput(&video);
    pipeline->SetOutputFiles(filesOut);
    pipeline->SetSpatialSigma(sigma);
    pipeline->SetRegularization(weight);
    pipeline->SetIterations(iterations);
    pipeline->SetRelaxation(relaxation);
    pipeline->SetNumberOfLevels(levels);
    
    TextPipelineObserver::Pointer observer = TextPipelineObserver::New();
    pipeline->AddObserver(observer.GetPointer());
    
    Logger::debug << function << ": Executing optical flow pipeline" << std::endl;
    pipeline->Update();
}
