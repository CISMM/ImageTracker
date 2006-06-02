
#include <string>

#include "itkDivideImageFilter.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkRescaleIntensityImageFilter.h"

#include "AttenuationCalibrationFilter.h"
#include "ImageUtils.h"

int main(int argc, char** argv)
{
    if (argc < 4)
    {
        std::cout << "Usage: " << argv[0] << " calibIn calibOut repairOut" << std::endl;
        exit(1);
    }

    // typedefs
    enum { Dimension = 2 };
    typedef itk::Image< unsigned short, Dimension > ImageType;
    typedef itk::Image< float, Dimension > FloatImageType;
    typedef itk::ImageFileReader< ImageType > ReaderType;
    typedef itk::ImageFileWriter< ImageType > WriterType;
    typedef AttenuationCalibrationFilter< ImageType, FloatImageType > CalibrateType;
    typedef itk::RescaleIntensityImageFilter< FloatImageType, ImageType > RescaleType;
    typedef itk::DivideImageFilter< ImageType, FloatImageType, ImageType > DivideType;

    // Create objects
    ReaderType::Pointer reader = ReaderType::New();
    CalibrateType::Pointer calibrate = CalibrateType::New();
    RescaleType::Pointer rescale = RescaleType::New();
    DivideType::Pointer divide = DivideType::New();
    WriterType::Pointer writer = WriterType::New();

    // Set up pipeline
    reader->SetFileName(argv[1]);
    calibrate->SetInput(reader->GetOutput());
    calibrate->SetStyle(Mean);
    rescale->SetInput(calibrate->GetOutput());
    rescale->SetOutputMinimum(0);
    rescale->SetOutputMaximum(255);
    writer->SetInput(rescale->GetOutput());
    writer->SetFileName(argv[2]);

    // Run pipeline
    // calibrate->Update();
    writer->Update();

    // Repair the original image
    divide->SetInput1(reader->GetOutput());
    divide->SetInput2(calibrate->GetOutput());
    writer->SetInput(divide->GetOutput());
    writer->SetFileName(argv[3]);
    writer->Update();

    PrintImageInfo< ImageType >(reader->GetOutput(), "Input image");
    PrintImageInfo< FloatImageType >(calibrate->GetOutput(), "Attenuation image");
    PrintImageInfo< ImageType >(divide->GetOutput(), "Repaired image");
}

