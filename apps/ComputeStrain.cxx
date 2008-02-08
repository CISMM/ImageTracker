
#include <string>

#include "itkImage.h"
#include "itkVector.h"

#include "FilePattern.h"
#include "FileSet.h"
#include "ImageSetReader.h"
#include "StrainTensorPipeline.h"
#include "TextPipelineObserver.h"

/**
 * Compute the strain of a given displacement vector field input.  Strain is the spatial derivative of
 * displacement; for each input field, two strain fields are computed---one each for the displacement
 * in x and y.  Each strain field has two components, again, one each for the derivative in x and y.
 *
 */
int main(int argc, char** argv)
{
    std::string function("ComputeStrain");
    
    // Check args
    if (argc < 6)
    {
        Logger::warning << "Usage:\n\t" << argv[0] << " dir formatIn start end formatOut [sigma]" << std::endl;
        exit(1);
    }
    
    // Typedefs
    typedef float ComponentType;
    typedef itk::Vector< ComponentType, 2 > PixelType;
    typedef itk::Image< PixelType, 2 > ImageType;
    
    // Parse parameters
    Logger::info << function << ": Parsing parameters" << std::endl;
    std::string dir(argv[1]);
    std::string formatIn(argv[2]);
    int start = atoi(argv[3]);
    int end = atoi(argv[4]);
    std::string formatOut(argv[5]);
    float sigma = argc > 6 ? atof(argv[6]) : 1.0;
    
    // Create IO objects
    Logger::info << function << ": Initializing IO" << std::endl;
    FileSet filesIn(FilePattern(dir, formatIn, start, end));
    FileSet filesOut(FilePattern(dir, formatOut, start, end));
    ImageSetReader< ImageType > video(filesIn);
    
    // Create processing pipeline
    Logger::info << function << ": Creating processing pipeline" << std::endl;
    StrainTensorPipeline::Pointer pipeline = StrainTensorPipeline::New();
    pipeline->SetInput(&video);
    pipeline->SetOutputFiles(filesOut);
    pipeline->SetSigma(sigma);
    
    // Observe
    TextPipelineObserver::Pointer observer = TextPipelineObserver::New();
    pipeline->AddObserver(observer.GetPointer());
    
    // Execute
    Logger::info << function << ": Executing pipeline" << std::endl;
    pipeline->Update();
    Logger::info << function << ": Done" << std::endl;
}
