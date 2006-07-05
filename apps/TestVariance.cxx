#include <string>
#include <vector>

#include "itkCastImageFilter.h"
#include "itkImage.h"
#include "itkSqrtImageFilter.h"

#include "FilePattern.h"
#include "FileSet.h"
#include "ImageSetReader.h"
#include "ImageUtils.h"
#include "Logger.h"
#include "NaryMeanImageFilter.h"
#include "NaryVarianceImageFilter.h"

/**
 * Computes the temporal mean, variance, and standard deviation of an image sequence.
 * Note: the variance image will have very large intensity values (order of the image
 * sequence's maximum pixel value squared). As such, this image can't typically be
 * represented as 16-bit. It is suggested you save the variance image as .mha or .vtk.
 */
int main(int argc, char** argv)
{
    // check usage
    if (argc < 8)
    {
        Logger::error << "Usage:\n    " << argv[0] << " dir format start end meanOut varOut stdOut" << std::endl;
        Logger::error << "    Note: the variance image should probably be a .mha or .vtk file." << std::endl;
        exit(1);
    }

    // Process inputs
    std::string dir = argv[1];
    std::string format = argv[2];
    int start = atoi(argv[3]);
    int end = atoi(argv[4]);
    std::string meanFile = argv[5];
    std::string varFile = argv[6];
    std::string stdFile = argv[7];
    
    // Typedefs
    enum { Dimension = 2 };
    typedef itk::Image< unsigned short, Dimension > ImageType;
    typedef itk::Image< float, Dimension > FloatImageType;
    typedef itk::SqrtImageFilter< FloatImageType, FloatImageType > SqrtType;
    typedef NaryMeanImageFilter< ImageType, FloatImageType > MeanType;
    typedef NaryVarianceImageFilter< ImageType, FloatImageType > VarianceType;
    typedef itk::CastImageFilter< FloatImageType, ImageType > CastType;

    // Define pipeline objects
    FileSet filesIn(FilePattern(dir, format, start, end));
    ImageSetReader<ImageType> video(filesIn);

    MeanType::Pointer mean = MeanType::New();
    VarianceType::Pointer variance = VarianceType::New();
    SqrtType::Pointer sqrt = SqrtType::New();
    CastType::Pointer cast = CastType::New();

    // Load input files into variance calculator
    Logger::verbose << "Loading image files..." << std::endl;
    for (unsigned int i = 0; i < video.size(); i++)
    {
        mean->AddInput(video[i]);
        variance->AddInput(video[i]);
    }

    // Write mean image
    PrintImageInfo(mean->GetOutput(), "Mean image");
    cast->SetInput(mean->GetOutput());
    WriteImage(cast->GetOutput(), meanFile);

    // Write variance image
    PrintImageInfo(variance->GetOutput(), "Variance image");
    WriteImage(variance->GetOutput(), varFile);

    // Write standard deviation image
    sqrt->SetInput(variance->GetOutput());
    PrintImageInfo(sqrt->GetOutput(), "Stdev image");
    cast->SetInput(sqrt->GetOutput());
    WriteImage(cast->GetOutput(), stdFile);
}
