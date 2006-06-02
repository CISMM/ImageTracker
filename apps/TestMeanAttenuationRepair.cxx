#include <string>
#include <vector>

#include "itkDivideImageFilter.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkRescaleIntensityImageFilter.h"

#include "AttenuationCalibrationFilter.h"
#include "ImageUtils.h"
#include "NaryMeanImageFilter.h"

int main(int argc, char** argv)
{
    // check usage
    if (argc < 7)
    {
        std::cout << "Usage: " << argv[0] << " dir formatIn start stop formatOut meanImg" << std::endl;
        exit(1);
    }

    // Typedefs
    enum { Dimension = 2 };
    typedef std::vector< std::string > StringVector;
    typedef itk::Image< unsigned short, Dimension > ImageType;
    typedef itk::Image< float, Dimension > FloatImageType;
    typedef itk::ImageFileReader< ImageType > ReaderType;
    typedef itk::ImageFileWriter< ImageType > WriterType;
    typedef NaryMeanImageFilter< ImageType, ImageType > MeanType;
    typedef AttenuationCalibrationFilter< ImageType, FloatImageType> AttenuateType;
    typedef itk::DivideImageFilter< ImageType, FloatImageType, FloatImageType> DivideType;
    typedef itk::RescaleIntensityImageFilter< FloatImageType, ImageType > RescaleType;
    typedef std::vector< ReaderType::Pointer > ReaderVector;

    // Set up file handling
    StringVector filesIn;
    StringVector filesOut;
    std::string dir = argv[1];
    dir += "/";
    std::string formatIn = dir + argv[2];
    int start = atoi(argv[3]);
    int end = atoi(argv[4]);

    std::string formatOut = dir + argv[5];
    std::string meanFile = dir + argv[6];

    char cName[256];

    // Create input file names
    for (int i = start; i <= end; i++)
    {
        sprintf(cName, formatIn.c_str(), i);
        filesIn.push_back(cName);
        sprintf(cName, formatOut.c_str(), i);
        filesOut.push_back(cName);
    }

    MeanType::Pointer mean = MeanType::New();
    AttenuateType::Pointer attenuate = AttenuateType::New();
    DivideType::Pointer divide = DivideType::New();
    RescaleType::Pointer rescale = RescaleType::New();
    WriterType::Pointer writer = WriterType::New();
    ReaderVector video;

    // Load input files into mean calculator
    std::cout << "Loading image files..." << std::endl;
    StringVector::iterator it;
    for (it = filesIn.begin(); it != filesIn.end(); ++it)
    {
        std::cout << "\t" << *it << std::endl;
        ReaderType::Pointer reader = ReaderType::New();
        reader->SetFileName(it->c_str());
        reader->Update();
        mean->AddInput(reader->GetOutput());
        video.push_back(reader);
    }

    // Save the mean image
    std::cout << "Saving mean image: " << meanFile << std::endl;
    writer->SetInput(mean->GetOutput());
    writer->SetFileName(meanFile.c_str());
    writer->Update();

    // Set up unchanging parts of the pipeline
    attenuate->SetInput(mean->GetOutput());
    // the attenuation calculation is based off the mean of the mean image.
    attenuate->SetStyle(Mean); 
    divide->SetInput2(attenuate->GetOutput());
    rescale->SetInput(divide->GetOutput());
    rescale->SetOutputMinimum(0);
    rescale->SetOutputMaximum(255);
    writer->SetInput(rescale->GetOutput());

    // Repair each image in the sequence
    std::cout << "Repairing image sequence..." << std::endl;
    StringVector::iterator sit;
    ReaderVector::iterator rit;
    for (sit = filesOut.begin(), rit = video.begin();
         sit != filesOut.end(), rit != video.end();
         ++sit, ++rit)
    {
        std::cout << "\t" << *sit << std::endl;
        divide->SetInput1((*rit)->GetOutput());
        writer->SetFileName(sit->c_str());
        writer->Update();
    }
}
