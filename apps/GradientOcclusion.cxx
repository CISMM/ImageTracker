
#include <string>

#include "itkCompose2DVectorImageFilter.h"
#include "itkImage.h"
#include "itkRecursiveGaussianImageFilter.h"
#include "itkSqrtImageFilter.h"

#include "AddConstantImageFilter.h"
#include "FilePattern.h"
#include "FileSet.h"
#include "ImageSetReader.h"
#include "ImageUtils.h"
#include "Logger.h"
#include "NaryMeanImageFilter.h"
#include "NaryMedianImageFilter.h"
#include "NaryVarianceImageFilter.h"
#include "NonZeroLog10ImageFilter.h"

/**
 * Attempts to find occlusion regions in a video by computing the ratio of
 * neighboring intensities at each image in the video sequence. Occluding
 * objects will have a constant intensity ratio profile. The ratio is 
 * computed in the x and y direction by finding the gradient of the log
 * of intensity of each image.
 */
int main(int argc, char** argv)
{
    if (argc < 8)
    {
        Logger::error << "Usage:" << std::endl;
        Logger::error << "\t" << argv[0] << " dir formatIn start end formatOut avg std" << std::endl;
        Logger::error << "\t  The output files should support float pixel types." << std::endl;
        exit(1);
    }
    
    // Evaluate input
    Logger::verbose << "Parsing input" << std::endl;
    std::string dir = argv[1];
    std::string formatIn = argv[2];
    int start = atoi(argv[3]);
    int end = atoi(argv[4]);
    std::string formatOut = argv[5];
    std::string meanFile = argv[6];
    std::string stdFile = argv[7];
    
    FileSet filesIn(FilePattern(dir, formatIn, start, end));
    FileSet filesOut(FilePattern(dir, formatOut, start, end));
    
    // Typedefs
    typedef itk::Image< unsigned short, 2 > InputImageType;
    typedef itk::Image< float, 2> InternalImageType;
    typedef NonZeroLog10ImageFilter< InternalImageType, InternalImageType > LogType;
    typedef itk::RecursiveGaussianImageFilter< InternalImageType, InternalImageType > GaussType;
    typedef NaryMeanImageFilter< InternalImageType, InternalImageType > AverageType;
    // typedef NaryMedianImageFilter< InternalImageType, InternalImageType > AverageType;
    typedef NaryVarianceImageFilter< InternalImageType, InternalImageType > VarianceType;
    typedef itk::SqrtImageFilter< InternalImageType, InternalImageType > SqrtType;
    typedef itk::Compose2DVectorImageFilter< InternalImageType > ComposeType;
    
    Logger::verbose << "Creating pipeline" << std::endl;
    ImageSetReader<InputImageType, InternalImageType> video(filesIn);
    LogType::Pointer log = LogType::New();
    GaussType::Pointer dx = GaussType::New();
    GaussType::Pointer dy = GaussType::New();
    ComposeType::Pointer compose = ComposeType::New();
    AverageType::Pointer avgX = AverageType::New();
    AverageType::Pointer avgY = AverageType::New();
    VarianceType::Pointer varX = VarianceType::New();
    VarianceType::Pointer varY = VarianceType::New();
    SqrtType::Pointer stdX = SqrtType::New();
    SqrtType::Pointer stdY = SqrtType::New();
    
    dx->SetInput(log->GetOutput());
    dx->SetDirection(0);
    dx->SetFirstOrder();
    dy->SetInput(log->GetOutput());
    dy->SetDirection(1);
    dy->SetFirstOrder();
    compose->SetInput1(dx->GetOutput());
    compose->SetInput2(dy->GetOutput());
    
    Logger::verbose << "Creating gradient images" << std::endl;
    for (unsigned int i = 0; i < video.size(); i++)
    {
        log->SetInput(video[i]);
        avgX->PushBackInput(dx->GetOutput());
        avgY->PushBackInput(dy->GetOutput());
        varX->PushBackInput(dx->GetOutput());
        varY->PushBackInput(dy->GetOutput());
        WriteImage(compose->GetOutput(), filesOut[i]);
    }
    
    Logger::verbose << "Creating average and standard deviation images" << std::endl;
    
    // mean
    compose->SetInput1(avgX->GetOutput());
    compose->SetInput2(avgY->GetOutput());
    WriteImage(compose->GetOutput(), meanFile);
    PrintImageInfo(avgX->GetOutput(), "Average dX");
    PrintImageInfo(avgY->GetOutput(), "Average dY");
    
    // standard deviation
    stdX->SetInput(varX->GetOutput());
    stdY->SetInput(varY->GetOutput());
    compose->SetInput1(stdX->GetOutput());
    compose->SetInput2(stdY->GetOutput());
    WriteImage(compose->GetOutput(), stdFile);
    PrintImageInfo(stdX->GetOutput(), "Stdev dX");
    PrintImageInfo(stdY->GetOutput(), "Stdev dY");
}
