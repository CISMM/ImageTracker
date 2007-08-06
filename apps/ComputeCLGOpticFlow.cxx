
#include "itkImage.h"
#include "itkVector.h"

#include "CLGOpticFlowImageFilter.h"
#include "FilePattern.h"
#include "FileSet.h"
#include "ImageSetReader.h"
#include "ImageUtils.h"
#include "Logger.h"

int main(int argc, char** argv)
{
    // Check inputs
    if (argc < 10)
    {
        Logger::error << "Usage: " << std::endl;
        Logger::error << "\t" << argv[0] << " dir formatIn start stop formatOut sigma regularization iterations relaxation" << std::endl;
        exit(1);
    }
    
    // Parse inputs
    Logger::debug << "Parsing inputs." << std::endl;
    std::string dir         = argv[1];
    std::string formatIn    = argv[2];
    int start               = atoi(argv[3]);
    int stop                = atoi(argv[4]);
    std::string formatOut   = argv[5];
    double sigma            = atof(argv[6]);
    double reg              = atof(argv[7]);
    int iter                = atoi(argv[8]);
    double rel              = atof(argv[9]);
    
    FileSet filesIn(FilePattern(dir, formatIn, start, stop));
    FileSet filesOut(FilePattern(dir, formatOut, start, stop-1));
    
    // Define types
    const unsigned int dimension = 2;
    typedef itk::Image< unsigned short, dimension > InputImageType;
    typedef itk::Image< float, dimension > InternalImageType;
    typedef itk::Vector< float, dimension > VectorType;
    typedef itk::Image< VectorType, dimension > OutputImageType;
    typedef ImageSetReader< InputImageType, InternalImageType > ReaderType;
    typedef CLGOpticFlowImageFilter< InternalImageType, InternalImageType, float > OpticFlowType;
    
    // Setup pipeline objects
    Logger::debug << "Setting up pipeline." << std::endl;
    ReaderType video(filesIn);
    OpticFlowType::Pointer flow = OpticFlowType::New();
    flow->SetSpatialSigma(sigma);
    flow->SetRegularization(reg);
    flow->SetRelaxation(rel);
    flow->SetIterations(iter);
    
    // Compute optic flow for each image pair
    Logger::debug << "Computing optic flow." << std::endl;
    for (int i = 0; i < video.size() - 1; i++)
    {
        Logger::debug << "CLG:\t" << i << " / " << (video.size() - 1) << std::endl;
        flow->SetInput1(video[i]);
        flow->SetInput2(video[i+1]);
        flow->Update();
        WriteImage< OutputImageType >(flow->GetOutput(), filesOut[i]);
    }

    video.LogStatistics();
}
