#include <string>

#include "itkImage.h"

#include "FilePattern.h"
#include "FileSet.h"
#include "HarrisFeatureInterestImageFilter.h"
#include "ImageSetReader.h"
#include "ImageUtils.h"
#include "Logger.h"

int main(int argc, char** argv)
{
    std::string function("TestHarrisFeatureDetector");
    Logger::info << ": Checking input paramters" << std::endl;
    if (argc < 9)
    {
        Logger::warning << "Usage:\n\t" << argv[0] << " dir formatIn start end formatOut derivSigma windowSigma traceWeight" << std::endl;
        exit(1);
    }
    
    std::string dir = argv[1];
    std::string formatIn = argv[2];
    int start = atoi(argv[3]);
    int end = atoi(argv[4]);
    std::string formatOut = argv[5];
    float derivSigma = atof(argv[6]);
    float windowSigma = atof(argv[7]);
    float traceWeight = atof(argv[8]);
    
    Logger::info << function << ": Setting file I/O" << std::endl;
    FileSet filesIn(FilePattern(dir, formatIn, start, end));
    FileSet filesOut(FilePattern(dir, formatOut, start, end));
    
    typedef itk::Image< unsigned int, 2 > InputImageType;
    typedef itk::Image< float, 2 > InternalImageType;
    typedef HarrisFeatureInterestImageFilter< InternalImageType, InternalImageType > HarrisType;
    
    ImageSetReader< InputImageType, InternalImageType > video(filesIn);
    
    Logger::info << function << ": Configuring Harris feature interest filter" << std::endl;
    HarrisType::Pointer harris = HarrisType::New();
    
    harris->SetDerivativeSigma(derivSigma);
    harris->SetWindowSigma(windowSigma);
    harris->SetTraceWeight(traceWeight);
    
    Logger::info << function << ": Computing Harris feature images" << std::endl;
    for (int i = 0; i < video.size(); i++)
    {
        harris->SetInput(video[i]);
        WriteImage(harris->GetOutput(), filesOut[i]);
        PrintImageInfo(harris->GetOutput());
    }
    
    Logger::info << function << ": done." << std::endl;
}
