#include <limits>
#include <string>
#include <vector>

#include "itkCastImageFilter.h"
#include "itkDivideImageFilter.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMedianImageFilter.h"
#include "itkMultiplyImageFilter.h"
#include "itkThresholdImageFilter.h"

#include "FilePattern.h"
#include "FileSet.h"
#include "ImageSetReader.h"
#include "ImageUtils.h"
#include "NaryMeanImageFilter.h"

int main(int argc, char** argv)
{
    // check usage
    if (argc < 10)
    {
        std::cout << "Usage: " << argv[0] << " dir formatIn start stop formatOut meanImg localImg ratioImg radius" << std::endl;
        exit(1);
    }

    // Typedefs
    enum { Dimension = 2 };
    typedef unsigned short PixelType;
    typedef itk::Image< PixelType, Dimension > ImageType;
    typedef itk::Image< float, Dimension > FloatImageType;

    typedef NaryMeanImageFilter< ImageType, FloatImageType > MeanType;
    typedef itk::MedianImageFilter< FloatImageType, FloatImageType > LocalType;
    typedef itk::DivideImageFilter< FloatImageType, FloatImageType, FloatImageType > DivideType;
    typedef itk::MultiplyImageFilter< ImageType, FloatImageType, FloatImageType > MultiplyType;
    typedef itk::ThresholdImageFilter< FloatImageType > ThresholdType;
    typedef itk::CastImageFilter< FloatImageType, ImageType > CastType;

    // Set up file handling
    std::string dir = argv[1];
    std::string formatIn = argv[2];
    int start = atoi(argv[3]);
    int end = atoi(argv[4]);
    std::string formatOut = argv[5];
    
    FileSet filesIn(FilePattern(dir, formatIn, start, end));
    FileSet filesOut(FilePattern(dir, formatOut, start, end));
    
    dir += "/";
    std::string meanFile = dir + argv[6];
    std::string localFile = dir + argv[7];
    std::string ratioFile = dir + argv[8];
    int rad = atoi(argv[9]);

    // Create pipeline objects
    MeanType::Pointer mean = MeanType::New();
    LocalType::Pointer local = LocalType::New();
    DivideType::Pointer divide = DivideType::New();
    ThresholdType::Pointer ratioClamp = ThresholdType::New();
    
    MultiplyType::Pointer multiply = MultiplyType::New();
    ThresholdType::Pointer threshold = ThresholdType::New();
    CastType::Pointer cast = CastType::New();

    ImageSetReader<ImageType> video(filesIn);

    // Load input files into mean calculator
    std::cout << "Loading image files..." << std::endl;
    for (unsigned int i = 0; i < video.size(); i++)
    {
        mean->AddInput(video[i]);
    }

    // Output mean image
    std::cout << "Writing mean image: " << meanFile << std::endl;
    cast->SetInput(mean->GetOutput());
    WriteImage(cast->GetOutput(), meanFile);

    PrintImageInfo< FloatImageType >(mean->GetOutput(), "Mean image");

    std::string extF = ".vtk";
    std::string meanFileF = dir + "float-mean" + extF;
    WriteImage(mean->GetOutput(), meanFileF);

    // Set up rest of pipeline
    local->SetInput(mean->GetOutput());
    LocalType::InputSizeType radius;
    radius.Fill(rad);
    local->SetRadius(radius);
    divide->SetInput1(local->GetOutput()); // what the mean intensity should be at each point
    divide->SetInput2(mean->GetOutput());  // what the mean intensity is
    // Occluders should only reduce image intensities.
    // Any attenuation ratios below 1.0 should be clamped.
    ratioClamp->ThresholdBelow(1.0);
    ratioClamp->SetOutsideValue(1.0);
    ratioClamp->SetInput(divide->GetOutput());

    // Output intermediate images
    std::cout << "Writing local metric image: " << localFile << std::endl;
    cast->SetInput(local->GetOutput());
    WriteImage(cast->GetOutput(), localFile);

    PrintImageInfo(local->GetOutput(), "Local metric image");

    std::string localFileF = dir + "float-local" + extF;
    WriteImage(local->GetOutput(), localFileF);

    std::cout << "Writing ratio image (local / mean): " << ratioFile << std::endl;
    cast->SetInput(ratioClamp->GetOutput());
    WriteImage(cast->GetOutput(), ratioFile);

    PrintImageInfo(ratioClamp->GetOutput(), "Ratio image");

    std::string ratioFileF = dir + "float-ratio" + extF;
    WriteImage(ratioClamp->GetOutput(), ratioFileF);

    // It is possible that a repaired pixel will have intensity over that
    // allowed by the pixel type.  So, we must threshold to avoid wrap-around
    // when casting to the output image type.
    threshold->SetOutsideValue(std::numeric_limits<PixelType>::max());
    threshold->ThresholdAbove(std::numeric_limits<PixelType>::max());

    // Repair video
    multiply->SetInput2(ratioClamp->GetOutput());
    threshold->SetInput(multiply->GetOutput());
    cast->SetInput(threshold->GetOutput());
    std::cout << "Repairing image sequence..." << std::endl;
    for (unsigned int i = 0; i < video.size(); i++)
    {
        multiply->SetInput1(video[i]);
        WriteImage(cast->GetOutput(), filesOut[i]);
    }

    PrintImageInfo< ImageType >(video[0], "First original frame");
    multiply->SetInput1(video[0]);
    multiply->Update();
    PrintImageInfo(multiply->GetOutput(), "First repaired frame");
    // WriteImage< FloatImageType >(multiply->GetOutput(), dir + "float-repair-01.vtk");

    video.LogStatistics();
}
