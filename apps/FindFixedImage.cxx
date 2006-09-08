#include <string>

#include "itkCastImageFilter.h"
#include "itkCompose2DVectorImageFilter.h"
#include "itkDivideImageFilter.h"
#include "itkExpImageFilter.h"
#include "itkImage.h"
#include "itkImageDuplicator.h"
#include "itkRecursiveGaussianImageFilter.h"
#include "itkShiftScaleImageFilter.h"
#include "itkStatisticsImageFilter.h"

#include "DerivativesToSurfaceImageFilter.h"
#include "FilePattern.h"
#include "FileSet.h"
#include "ImageSetReader.h"
#include "ImageUtils.h"
#include "Logger.h"
#include "NaryMeanImageFilter.h"
#include "NonZeroLog10ImageFilter.h"

int main(int argc, char** argv)
{
    // Check command line arguments
    if (argc < 8)
    {
        Logger::error << "Usage:" << std::endl;
        Logger::error << "\t" << argv[0] << " dir formatIn start end meangradlog fixedImg formatOut" << std::endl;
        Logger::error << "\tNote: the output mean and fixed images should support float vector types (e.g. mha, vtk)." << std::endl;
        exit(1);
    }
    
    // Parse command line args
    std::string dir = argv[1];
    std::string formatIn = argv[2];
    int start = atoi(argv[3]);
    int end = atoi(argv[4]);
    std::string meanGradLogFile = argv[5];
    std::string fixedImageFile= argv[6];
    std::string formatOut = argv[7];
    
    // typedefs
    const unsigned int Dimension = 2;
    typedef itk::Image< unsigned short, Dimension > InputImageType;
    typedef itk::Image< float, Dimension > InternalImageType;
    
    typedef NonZeroLog10ImageFilter< InternalImageType, InternalImageType > LogType;
    typedef NaryMeanImageFilter< InternalImageType, InternalImageType > MeanType;
    typedef DerivativesToSurfaceImageFilter< InternalImageType > SurfaceType;
    
    typedef itk::RecursiveGaussianImageFilter< InternalImageType, InternalImageType > GradType;
    typedef itk::Compose2DVectorImageFilter< InternalImageType > ComposeType;
    typedef itk::ImageDuplicator< InternalImageType > CopyType;
    typedef itk::ExpImageFilter< InternalImageType, InternalImageType > ExpType;
    typedef itk::StatisticsImageFilter< InternalImageType > StatsType;
    typedef itk::ShiftScaleImageFilter< InternalImageType, InternalImageType > ScaleType;
    typedef itk::DivideImageFilter< InternalImageType, InternalImageType, InternalImageType > DivideType;
    typedef itk::CastImageFilter< InternalImageType, InputImageType > CastType;

    // Pipeline components
    Logger::verbose << "Creating pipeline components" << std::endl;
    FileSet filesIn(FilePattern(dir, formatIn, start, end));
    FileSet filesOut(FilePattern(dir, formatOut, start, end));
    ImageSetReader<InputImageType, InternalImageType> video(filesIn);
    
    LogType::Pointer log = LogType::New();
    GradType::Pointer dx = GradType::New();
    GradType::Pointer dy = GradType::New();
    MeanType::Pointer avgX = MeanType::New();
    MeanType::Pointer avgY = MeanType::New();
    
    CopyType::Pointer copyX = CopyType::New();
    CopyType::Pointer copyY = CopyType::New();
    
    ComposeType::Pointer compose = ComposeType::New();
    
    dx->SetDirection(0);
    dx->SetFirstOrder();
    dy->SetDirection(1);
    dy->SetFirstOrder();
    dx->SetInput(log->GetOutput());
    dy->SetInput(log->GetOutput());
    
    // Compute two metrics:
    // mean of gradient of logarithm
    // gradient of logarithm of mean
    Logger::verbose << "Computing derivative transmission metric" << std::endl;
    for (unsigned int i = 0; i < video.size(); i++)
    {
        // mean of gradient of log
        log->SetInput(video[i]);
        log->Update();
        dx->Update();
        dy->Update();
        
        copyX->SetInputImage(dx->GetOutput());
        copyY->SetInputImage(dy->GetOutput());
        copyX->Update();
        copyY->Update();
        
        avgX->PushBackInput(copyX->GetOutput());
        avgY->PushBackInput(copyY->GetOutput());
    }
        
    // Mean of gradient of logarithm
    PrintImageInfo(dx->GetOutput(), "Final gradX of log");
    PrintImageInfo(dy->GetOutput(), "Final gradY of log");
    PrintImageInfo(avgX->GetOutput(), "Mean of gradX of log");
    PrintImageInfo(avgY->GetOutput(), "Mean of gradY of log");
    compose->SetInput1(avgX->GetOutput());
    compose->SetInput2(avgY->GetOutput());
    WriteImage(compose->GetOutput(), meanGradLogFile);
    
    // Compute log transmission from gradient of log transmission
    Logger::debug << "Compute surface from derivative estimates" << std::endl;
    SurfaceType::Pointer surface = SurfaceType::New();
    surface->SetInputDx(avgX->GetOutput());
    surface->SetInputDy(avgY->GetOutput());
    PrintImageInfo(surface->GetOutput(), "Log of transmission");
    
    // Take exponent of gradients to get gradient of light transmission
    ExpType::Pointer exp = ExpType::New();
    exp->SetInput(surface->GetOutput());
    PrintImageInfo(exp->GetOutput(), "Transmission metric");
    
    // Rescale transmission metric to be 1.0 at maximum
    Logger::debug << "Rescaling transmission metric" << std::endl;
    StatsType::Pointer stats = StatsType::New();
    ScaleType::Pointer scale = ScaleType::New();
    stats->SetInput(exp->GetOutput());
    stats->Update();
    scale->SetInput(exp->GetOutput());
    scale->SetScale(1.0 / stats->GetMaximum());
    PrintImageInfo(scale->GetOutput(), "Transmission (rescaled)");
    WriteImage(scale->GetOutput(), fixedImageFile);

    // Repair input video
    Logger::debug << "Repairing input video" << std::endl;
    DivideType::Pointer divide = DivideType::New();
    CastType::Pointer cast = CastType::New();
    divide->SetInput2(scale->GetOutput());
    cast->SetInput(divide->GetOutput());
    for (unsigned int i = 0; i < video.size(); i++)
    {
        divide->SetInput1(video[i]);
        WriteImage(cast->GetOutput(), filesOut[i]);
    }
}
