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
#include "Logger.h"
#include "NaryMeanImageFilter.h"

int main(int argc, char** argv)
{
    // check usage
    if (argc < 10)
    {
        Logger::error << "Usage:\n\t" << argv[0] << " dir formatIn start stop formatOut meanImg localImg transmitImg radius" << std::endl;
        Logger::error << "\tthe transmission image should support float pixels (.mha, .vtk)." << std::endl;
        exit(1);
    }

    // Typedefs
    enum { Dimension = 2 };
    typedef unsigned short PixelType;
    typedef itk::Image< PixelType, Dimension > ImageType;
    typedef itk::Image< float, Dimension > FloatImageType;

    typedef NaryMeanImageFilter< FloatImageType, FloatImageType > MeanType;
    typedef itk::MedianImageFilter< FloatImageType, FloatImageType > LocalType;
    typedef itk::DivideImageFilter< FloatImageType, FloatImageType, FloatImageType > DivideType;
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
    
    DivideType::Pointer repair = DivideType::New();
    ThresholdType::Pointer threshold = ThresholdType::New();
    CastType::Pointer cast = CastType::New();

    ImageSetReader<ImageType, FloatImageType> video(filesIn);

    // Load input files into mean calculator
    std::cout << "Loading image files..." << std::endl;
    for (unsigned int i = 0; i < video.size(); i++)
    {
        mean->PushBackInput(video[i]);
    }

    // Output mean image
    std::cout << "Writing mean image: " << meanFile << std::endl;
    cast->SetInput(mean->GetOutput());
    WriteImage(cast->GetOutput(), meanFile);
    PrintImageInfo< FloatImageType >(mean->GetOutput(), "Mean image");    

//     std::string extF = ".vtk";
//     std::string meanFileF = dir + "float-mean" + extF;
//     WriteImage(mean->GetOutput(), meanFileF);

    // Set up rest of pipeline
    local->SetInput(mean->GetOutput());
    LocalType::InputSizeType radius;
    radius.Fill(rad);
    local->SetRadius(radius);
    
    divide->SetInput2(local->GetOutput()); // what the mean intensity should be at each point
    divide->SetInput1(mean->GetOutput());  // what the mean intensity is
    // Occluders should only reduce image intensities.
    // Any attenuation ratios below 1.0 should be clamped.
    ratioClamp->ThresholdAbove(1.0);
    ratioClamp->SetOutsideValue(1.0);
    ratioClamp->SetInput(divide->GetOutput());

    // Output intermediate images
    std::cout << "Writing local metric image: " << localFile << std::endl;
    cast->SetInput(local->GetOutput());
    WriteImage(cast->GetOutput(), localFile);
    PrintImageInfo(local->GetOutput(), "Local metric image");

    std::cout << "Writing ratio image (mean / local): " << ratioFile << std::endl;
    // cast->SetInput(ratioClamp->GetOutput());
    WriteImage(ratioClamp->GetOutput(), ratioFile);
    PrintImageInfo(ratioClamp->GetOutput(), "Ratio image");

    // It is possible that a repaired pixel will have intensity over that
    // allowed by the pixel type.  So, we must threshold to avoid wrap-around
    // when casting to the output image type.
    threshold->SetOutsideValue(std::numeric_limits<PixelType>::max());
    threshold->ThresholdAbove(std::numeric_limits<PixelType>::max());

    // Repair video
    repair->SetInput2(ratioClamp->GetOutput());
    threshold->SetInput(repair->GetOutput());
    cast->SetInput(threshold->GetOutput());
    std::cout << "Repairing image sequence..." << std::endl;
    for (unsigned int i = 0; i < video.size(); i++)
    {
        repair->SetInput1(video[i]);
        WriteImage(cast->GetOutput(), filesOut[i]);
    }

    PrintImageInfo< FloatImageType >(video[0], "First original frame");
    repair->SetInput1(video[0]);
    repair->Update();
    PrintImageInfo(repair->GetOutput(), "First repaired frame");

    video.LogStatistics();
}
