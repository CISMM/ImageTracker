#include "TestVectorConvert.h"

#include "itkExceptionObject.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkVector.h"

TestVectorConvert::TestVectorConvert(void)
{
}

TestVectorConvert::~TestVectorConvert(void)
{
}

void TestVectorConvert::run()
{
    this->testConversion();
}

void TestVectorConvert::testConversion()
{
    try
    {
        typedef itk::Vector<float, 2> VectorType;
        typedef itk::Image<VectorType, 2> ImageType;
        typedef itk::ImageFileReader<ImageType> ReaderType;
        typedef itk::ImageFileWriter<ImageType> WriterType;

        ReaderType::Pointer reader = ReaderType::New();
        WriterType::Pointer writer = WriterType::New();

        std::string fileNameBase = "C:\\UNC CS\\Research\\Nano\\data\\test\\Demons-RotateNoise-01";
        std::string inFile = fileNameBase + ".vtk";
        std::string outFile = fileNameBase + ".mha";

        std::cout << "Setting up vector image conversion pipeline." << std::endl;
        reader->SetFileName(inFile.c_str());
        writer->SetFileName(outFile.c_str());
        writer->SetInput(reader->GetOutput());
        std::cout << "Updating pipeline (running)." << std::endl;
        writer->Update();

        succeed_();
    }
    catch (itk::ExceptionObject &err)
    {
        std::cerr << "Exception caught!\n" << err << std::endl;
        fail_("Exception caught");
    }
}
