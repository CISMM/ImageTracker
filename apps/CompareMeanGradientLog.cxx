#include <string>

#include "itkAbsoluteValueDifferenceImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkCompose2DVectorImageFilter.h"
#include "itkExpImageFilter.h"
#include "itkImage.h"
#include "itkImageDuplicator.h"
#include "itkRecursiveGaussianImageFilter.h"

#include "DerivativesToSurfaceImageFilter.h"
#include "FilePattern.h"
#include "FileSet.h"
#include "ImageSetReader.h"
#include "ImageUtils.h"
#include "Logger.h"
#include "NaryMeanImageFilter.h"
#include "NaryMedianImageFilter.h"
#include "NonZeroLog10ImageFilter.h"

int main(int argc, char** argv)
{
    // Check command line arguments
    if (argc < 10)
    {
        Logger::error << "Usage:" << std::endl;
        Logger::error << "\t" << argv[0] << " dir formatIn start end meangradlog gradmeanlog mediangradlog compmean surfmean" << std::endl;
        Logger::error << "\tNote: the output images should support float vector types (e.g. mha, vtk)." << std::endl;
        exit(1);
    }
    
    // Parse command line args
    std::string dir = argv[1];
    std::string formatIn = argv[2];
    int start = atoi(argv[3]);
    int end = atoi(argv[4]);
    std::string meanGradLogFile = argv[5];
    std::string gradMeanLogFile = argv[6];
    std::string medianGradLogFile = argv[7];
    std::string compareFile = argv[8];
    std::string surfMeanFile = argv[9];
    
    // typedefs
    const unsigned int Dimension = 2;
    typedef itk::Image< unsigned short, Dimension > InputImageType;
    typedef itk::Image< float, Dimension > InternalImageType;
    
    typedef NonZeroLog10ImageFilter< InternalImageType, InternalImageType > LogType;
    typedef NaryMeanImageFilter< InternalImageType, InternalImageType > MeanType;
    typedef NaryMedianImageFilter< InternalImageType, InternalImageType > MedianType;
    typedef DerivativesToSurfaceImageFilter< InternalImageType > SurfaceType;
    typedef itk::RecursiveGaussianImageFilter< InternalImageType, InternalImageType > GradType;
    typedef itk::AbsoluteValueDifferenceImageFilter< InternalImageType, InternalImageType, InternalImageType > DiffType;
    typedef itk::Compose2DVectorImageFilter< InternalImageType > ComposeType;
    typedef itk::ImageDuplicator< InternalImageType > CopyType;
    typedef itk::ExpImageFilter< InternalImageType, InternalImageType > ExpType;
    
    // Pipeline components
    Logger::verbose << "Creating pipeline components" << std::endl;
    FileSet filesIn(FilePattern(dir, formatIn, start, end));
    ImageSetReader<InputImageType, InternalImageType> video(filesIn);
    
    LogType::Pointer log = LogType::New();
    GradType::Pointer dx = GradType::New();
    GradType::Pointer dy = GradType::New();
    MeanType::Pointer avgX = MeanType::New();
    MeanType::Pointer avgY = MeanType::New();
    MeanType::Pointer avgLog = MeanType::New();
    MedianType::Pointer medX = MedianType::New();
    MedianType::Pointer medY = MedianType::New();
    
    CopyType::Pointer copyX = CopyType::New();
    CopyType::Pointer copyY = CopyType::New();
    CopyType::Pointer copyLog = CopyType::New();
    
    DiffType::Pointer diffX = DiffType::New();
    DiffType::Pointer diffY = DiffType::New();
    ComposeType::Pointer compose = ComposeType::New();
    
    SurfaceType::Pointer surface = SurfaceType::New();
    ExpType::Pointer exp = ExpType::New();

    dx->SetDirection(0);
    dx->SetFirstOrder();
    dy->SetDirection(1);
    dy->SetFirstOrder();
    dx->SetInput(log->GetOutput());
    dy->SetInput(log->GetOutput());
    
    // Compute two metrics:
    // mean of gradient of logarithm
    // gradient of logarithm of mean
    Logger::verbose << "Computing metric two ways" << std::endl;
    for (unsigned int i = 0; i < video.size(); i++)
    {
        // mean of gradient of log
        log->SetInput(video[i]);
        log->Update();
        dx->Update();
        dy->Update();
        
        copyX->SetInputImage(dx->GetOutput());
        copyY->SetInputImage(dy->GetOutput());
        copyLog->SetInputImage(log->GetOutput());
        copyX->Update();
        copyY->Update();
        copyLog->Update();
        
        avgX->PushBackInput(copyX->GetOutput());
        avgY->PushBackInput(copyY->GetOutput());
        medX->PushBackInput(copyX->GetOutput());
        medY->PushBackInput(copyY->GetOutput());
        avgLog->PushBackInput(log->GetOutput());
    }
        
    // Mean of gradient of logarithm
    PrintImageInfo(dx->GetOutput(), "Final gradX of log");
    PrintImageInfo(dy->GetOutput(), "Final gradY of log");
    PrintImageInfo(avgX->GetOutput(), "Mean of gradX of log");
    PrintImageInfo(avgY->GetOutput(), "Mean of gradY of log");
    compose->SetInput1(avgX->GetOutput());
    compose->SetInput2(avgY->GetOutput());
    WriteImage(compose->GetOutput(), meanGradLogFile);
    
    // Gradient of mean of log
    dx->SetInput(avgLog->GetOutput());
    dy->SetInput(avgLog->GetOutput());
    PrintImageInfo(dx->GetOutput(), "GradX of mean of log");
    PrintImageInfo(dy->GetOutput(), "GradY of mean of log");
    compose->SetInput1(dx->GetOutput());
    compose->SetInput2(dy->GetOutput());
    WriteImage(compose->GetOutput(), gradMeanLogFile);

    // compare methods
    diffX->SetInput1(avgX->GetOutput());
    diffX->SetInput2(dx->GetOutput());
    diffY->SetInput1(avgY->GetOutput());
    diffY->SetInput2(dy->GetOutput());
    
    // print stats
    PrintImageInfo(diffX->GetOutput(), "Difference X");
    PrintImageInfo(diffY->GetOutput(), "Difference Y");
    compose->SetInput1(diffX->GetOutput());
    compose->SetInput2(diffY->GetOutput());
    WriteImage(compose->GetOutput(), compareFile);
    
    // Median of gradient of log
    PrintImageInfo(medX->GetOutput(), "Median of gradX of log");
    PrintImageInfo(medY->GetOutput(), "Median of gradY of log");
    compose->SetInput1(medX->GetOutput());
    compose->SetInput2(medY->GetOutput());
    WriteImage(compose->GetOutput(), medianGradLogFile);
    
    // Compute surface from mean of grad of log
    Logger::debug << "Compute surface from derivative estimates (mean of grad of log)" << std::endl;
    surface->SetInputDx(avgX->GetOutput());
    surface->SetInputDy(avgY->GetOutput());
    exp->SetInput(surface->GetOutput());
    PrintImageInfo(exp->GetOutput(), "Surface");
    WriteImage(exp->GetOutput(), surfMeanFile);
}
