#include <string>

#include "itkImage.h"

#include "FilePattern.h"
#include "FileSet.h"
#include "HornOpticalFlowPipeline.h"
#include "ImageSetReader.h"
#include "Logger.h"

/**
 * \brief Compute the optical flow of an image sequence using the Horn & Schunck optical flow algorithm.
 *
 * The user selects the scale parameter to use when computing spatial derivatives, the smoothness criteria
 * weighting, and the number of iterative steps to take.  In general, the scale parameter should be chosen
 * to smooth noise from the input images.  The weight parameter should be chosen based on the expected
 * level of turbulance and discontinuity of the optical flow.  The number of iterations should be chosen
 * as the largest of:
 *  - the number of pixels separating smooth image regions from textured image regions in the input images
 *  - the order magnitued of the smoothness weighting parameter
 * A larger smoothness constraint leads to a slowly-evolving optical flow update, so more iterations are
 * needed to reach a stable solution.
 */
int main(int argc, char** argv)
{
    std::string function("ComputeHornOpticalFlow");
    
    Logger::debug << function << ": Parsing parameters" << std::endl;
    if (argc < 9)
    {
        Logger::warning << "Usage:\n\t" << argv[0] << " dir formatIn start end formatOut sigma weight iterations" << std::endl;
        return 1;
    }

    std::string dir = argv[1];
    std::string formatIn = argv[2];
    int start = atoi(argv[3]);
    int end = atoi(argv[4]);
    std::string formatOut = argv[5];
    float sigma = atof(argv[6]);
    float weight = atof(argv[7]);
    int iterations = atoi(argv[8]);
    
    Logger::debug << function << ": Setting up image I/O" << std::endl;
    typedef itk::Image< unsigned short, 2 > InputImageType;
    typedef itk::Image< float, 2 > InternalImageType;
    FileSet filesIn(FilePattern(dir, formatIn, start, end));
    FileSet filesOut(FilePattern(dir, formatOut, start, end));
    ImageSetReader<InputImageType, InternalImageType> video(filesIn);
    
    Logger::debug << function << ": Setting up optical flow pipeline" << std::endl;
    HornOpticalFlowPipeline::Pointer pipeline = HornOpticalFlowPipeline::New();
    pipeline->SetInput(&video);
    pipeline->SetOutputFiles(filesOut);
    pipeline->SetSpatialSigma(sigma);
    pipeline->SetSmoothWeighting(weight);
    pipeline->SetIterations(iterations);
    
    Logger::debug << function << ": Executing optical flow pipeline" << std::endl;
    pipeline->Update();
}
