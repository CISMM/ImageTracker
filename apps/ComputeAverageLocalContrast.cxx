#include <string>

#include "itkCompose2DVectorImageFilter.h"
#include "itkImage.h"
#include "itkImageDuplicator.h"

#include "FilePattern.h"
#include "FileSet.h"
#include "ImageSetReader.h"
#include "ImageUtils.h"
#include "LocalContrastImageFilter.h"
#include "Logger.h"
#include "NaryMeanImageFilter.h"
#include "NaryMedianImageFilter.h"

int main(int argc, char** argv)
{
    if (argc < 7)
    {
        Logger::error << "Usage:\n\t" << argv[0] << " dir formatIn start end meancontrast mediancontrast" << std::endl;
        Logger::error << "\tNote: the output images should support float vector types (e.g. mha, vtk)." << std::endl;
        exit(1);
    }
    
    // typedefs
    typedef itk::Image< unsigned int, 2 > InputImageType;
    typedef itk::Image< float, 2 > InternalImageType;
    typedef itk::ImageDuplicator< InternalImageType > CopyType;
    typedef NaryMeanImageFilter< InternalImageType, InternalImageType > MeanType;
    typedef NaryMedianImageFilter< InternalImageType, InternalImageType > MedianType;
    typedef LocalContrastImageFilter< InternalImageType, InternalImageType > ContrastType;
    typedef itk::Compose2DVectorImageFilter< InternalImageType > ComposeType;
    
    // Parse command line arguments
    std::string dir = argv[1];
    std::string formatIn = argv[2];
    int start = atoi(argv[3]);
    int end = atoi(argv[4]);
    std::string meanContrastFile = argv[5];
    std::string medianContrastFile = argv[6];
    
    // Setup IO
    FileSet filesIn(FilePattern(dir, formatIn, start, end));
    ImageSetReader< InputImageType, InternalImageType > video(filesIn);
    
    // Setup pipeline
    ContrastType::Pointer contrastX = ContrastType::New();
    ContrastType::Pointer contrastY = ContrastType::New();
    MeanType::Pointer meanContX = MeanType::New();
    MeanType::Pointer meanContY = MeanType::New();
    MedianType::Pointer medianContX = MedianType::New();
    MedianType::Pointer medianContY = MedianType::New();
    CopyType::Pointer copyX = CopyType::New();
    CopyType::Pointer copyY = CopyType::New();
    ComposeType::Pointer compose = ComposeType::New();
    
    contrastX->SetDirection(0);
    contrastY->SetDirection(1);
    
    // Compute mean of contrast and mean intensity image
    Logger::verbose << "Computing mean and median of local contrast..." << std::endl;
    for (unsigned int i = 0; i < video.size(); i++)
    {
        contrastX->SetInput(video[i]);
        contrastY->SetInput(video[i]);
        contrastX->Update();
        contrastY->Update();
        copyX->SetInputImage(contrastX->GetOutput());
        copyY->SetInputImage(contrastY->GetOutput());
        copyX->Update();
        copyY->Update();
        meanContX->PushBackInput(copyX->GetOutput());
        meanContY->PushBackInput(copyY->GetOutput());
        medianContX->PushBackInput(copyX->GetOutput());
        medianContY->PushBackInput(copyY->GetOutput());
    }
    
    PrintImageInfo(meanContX->GetOutput(), "Mean of contrast X");
    PrintImageInfo(meanContY->GetOutput(), "Mean of contrast Y");
    compose->SetInput1(meanContX->GetOutput());
    compose->SetInput2(meanContY->GetOutput());
    WriteImage(compose->GetOutput(), meanContrastFile);
    
    PrintImageInfo(medianContX->GetOutput(), "Median of contrast X");
    PrintImageInfo(medianContY->GetOutput(), "Median of contrast Y");
    compose->SetInput1(medianContX->GetOutput());
    compose->SetInput2(medianContY->GetOutput());
    WriteImage(compose->GetOutput(), medianContrastFile);
}
