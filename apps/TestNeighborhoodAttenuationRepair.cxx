#include <string>
#include <vector>

#include "itkDivideImageFilter.h"
#include "itkExpNegativeImageFilter.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMeanImageFilter.h"
#include "itkMedianImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkSubtractImageFilter.h"

#include "GaussianFunctionImageFilter.h"
#include "ImageUtils.h"
#include "NaryMeanImageFilter.h"

int main(int argc, char** argv)
{
    // check usage
    if (argc < 10)
    {
        std::cout << "Usage: " << argv[0] << " dir formatIn start stop formatOut meanImg localImg diffImg metricImg" << std::endl;
        exit(1);
    }

    // Typedefs
    enum { Dimension = 2 };
    typedef std::vector< std::string > StringVector;
    typedef itk::Image< unsigned short, Dimension > ImageType;
    typedef itk::Image< float, Dimension > FloatImageType;

    typedef itk::ImageFileReader< ImageType > ReaderType;
    typedef itk::ImageFileWriter< ImageType > WriterType;
    typedef itk::ImageFileWriter< FloatImageType > FloatWriterType;

    typedef NaryMeanImageFilter< ImageType, FloatImageType > MeanType;

    typedef itk::MedianImageFilter< FloatImageType, FloatImageType > LocalType;
    // typedef itk::MeanImageFilter< FloatImageType, FloatImageType > LocalType;
    typedef itk::SubtractImageFilter< FloatImageType, FloatImageType, FloatImageType > DifferenceType;

    typedef GaussianFunctionImageFilter< FloatImageType, FloatImageType > MetricType;
    // typedef itk::DivideImageFilter< FloatImageType, FloatImageType, FloatImageType > MetricType;
    // typedef itk::ExpNegativeImageFilter< FloatImageType, FloatImageType > MetricType;

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
    std::string localFile = dir + argv[7];
    std::string diffFile = dir + argv[8];
    std::string metricFile = dir + argv[9];

    char cName[256];

    // Create input file names
    for (int i = start; i <= end; i++)
    {
        sprintf(cName, formatIn.c_str(), i);
        filesIn.push_back(cName);
        sprintf(cName, formatOut.c_str(), i);
        filesOut.push_back(cName);
    }

    // Create pipeline objects
    MeanType::Pointer mean = MeanType::New();
    LocalType::Pointer local = LocalType::New();
    DifferenceType::Pointer difference = DifferenceType::New();
    MetricType::Pointer metric = MetricType::New();

    DivideType::Pointer divide = DivideType::New();
    RescaleType::Pointer rescale = RescaleType::New();
    WriterType::Pointer writer = WriterType::New();
    FloatWriterType::Pointer writerF = FloatWriterType::New();
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

    rescale->SetOutputMinimum(0);
    rescale->SetOutputMaximum(255);
    writer->SetInput(rescale->GetOutput());

    // Output mean image
    std::cout << "Writing mean image: " << meanFile << std::endl;
    rescale->SetInput(mean->GetOutput());
    writer->SetFileName(meanFile.c_str());
    writer->Update();

    PrintImageInfo< FloatImageType >(mean->GetOutput(), "Mean image");

    // Set up rest of pipeline
    local->SetInput(mean->GetOutput());
    LocalType::InputSizeType radius;
    radius.Fill(5);
    local->SetRadius(radius);
    difference->SetInput1(mean->GetOutput());
    difference->SetInput2(local->GetOutput());

    metric->SetInput(difference->GetOutput());
    metric->SetSigma(15000);
    // metric->SetInput1(mean->GetOutput());
    // metric->SetInput2(local->GetOutput());

    // Output intermediate images
    std::cout << "Writing local metric image: " << localFile << std::endl;
    rescale->SetInput(local->GetOutput());
    writer->SetFileName(localFile.c_str());
    writer->Update();

    PrintImageInfo< FloatImageType >(local->GetOutput(), "Local metric image");

    std::string extF = ".vtk";
    std::string localFileF = dir + "atten-local" + extF;
    writerF->SetInput(local->GetOutput());
    writerF->SetFileName(localFileF.c_str());
    writerF->Update();

    std::cout << "Writing difference image (mean - local): " << diffFile << std::endl;
    rescale->SetInput(difference->GetOutput());
    writer->SetFileName(diffFile.c_str());
    writer->Update();

    PrintImageInfo< FloatImageType >(difference->GetOutput(), "Difference image");

    std::string diffFileF = dir + "atten-diff" + extF;
    writerF->SetInput(difference->GetOutput());
    writerF->SetFileName(diffFileF.c_str());
    writerF->Update();

    std::cout << "Writing attenuation metric image: " << metricFile << std::endl;
    rescale->SetInput(metric->GetOutput());
    writer->SetFileName(metricFile.c_str());
    writer->Update();

    PrintImageInfo< FloatImageType >(metric->GetOutput(), "Attenuation metric image");

    std::string metricFileF = dir + "atten-metric" + extF;
    writerF->SetInput(metric->GetOutput());
    writerF->SetFileName(metricFileF.c_str());
    writerF->Update();

    // Repair video
    divide->SetInput2(metric->GetOutput());
    rescale->SetInput(divide->GetOutput());
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

    PrintImageInfo< ImageType >(video[0]->GetOutput(), "First original frame");
    divide->SetInput1(video[0]->GetOutput());
    divide->Update();
    PrintImageInfo< FloatImageType >(divide->GetOutput(), "First repared frame");
}
