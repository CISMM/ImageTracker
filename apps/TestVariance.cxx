#include <string>
#include <vector>

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkSqrtImageFilter.h"

#include "ImageUtils.h"
#include "NaryMeanImageFilter.h"
#include "NaryVarianceImageFilter.h"

int main(int argc, char** argv)
{
    // check usage
    if (argc < 8)
    {
        std::cout << "Usage: " << argv[0] << " dir format start stop meanOut varOut stdOut" << std::endl;
        exit(1);
    }

    // Typedefs
    enum { Dimension = 2 };
    typedef std::vector< std::string > StringVector;
    typedef itk::Image< unsigned short, Dimension > ImageType;
    typedef itk::Image< float, Dimension > FloatImageType;
    typedef itk::ImageFileReader< ImageType > ReaderType;
    typedef itk::ImageFileWriter< ImageType > WriterType;
    typedef itk::SqrtImageFilter< FloatImageType, FloatImageType > SqrtType;
    typedef itk::RescaleIntensityImageFilter< FloatImageType, ImageType > RescaleType;
    typedef NaryMeanImageFilter< ImageType, ImageType > MeanType;
    typedef NaryVarianceImageFilter< ImageType, FloatImageType > VarianceType;

    // Define pipeline objects
    MeanType::Pointer mean = MeanType::New();
    VarianceType::Pointer variance = VarianceType::New();
    RescaleType::Pointer rescale = RescaleType::New();
    SqrtType::Pointer sqrt = SqrtType::New();
    WriterType::Pointer writer = WriterType::New();

    StringVector files;
    std::string dir = argv[1];
    dir += "/";
    std::string format = dir + argv[2];
    int start = atoi(argv[3]);
    int end = atoi(argv[4]);

    std::string meanFile = dir + argv[5];
    std::string varFile = dir + argv[6];
    std::string stdFile = dir + argv[7];

    char cName[256];

    // Create input file names
    for (int i = start; i <= end; i++)
    {
        sprintf(cName, format.c_str(), i);
        files.push_back(cName);
    }

    // Load input files into variance calculator
    std::cout << "Loading image files" << std::endl;
    StringVector::iterator it;
    for (it = files.begin(); it != files.end(); ++it)
    {
        std::cout << "\t" << *it << std::endl;
        ReaderType::Pointer reader = ReaderType::New();
        reader->SetFileName(it->c_str());
        reader->Update();
        mean->AddInput(reader->GetOutput());
        variance->AddInput(reader->GetOutput());
    }

    // Write mean image
    writer->SetInput(mean->GetOutput());
    writer->SetFileName(meanFile.c_str());
    writer->Update();
    PrintImageInfo< ImageType >(mean->GetOutput(), "Mean image");

    // Build and update pipeline for variance
    rescale->SetInput(variance->GetOutput());
    rescale->SetOutputMinimum(0);
    rescale->SetOutputMaximum(255);

    // Write variance image
    writer->SetInput(rescale->GetOutput());
    writer->SetFileName(varFile.c_str());
    writer->Update();
    PrintImageInfo< ImageType >(rescale->GetOutput(), "Variance image");

    // Write standard deviation image
    sqrt->SetInput(variance->GetOutput());
    rescale->SetInput(sqrt->GetOutput());
    writer->SetInput(rescale->GetOutput());
    writer->SetFileName(stdFile.c_str());
    writer->Update();
    PrintImageInfo< ImageType >(rescale->GetOutput(), "Stdev image");
}
