#include "TestImageStatistics.h"

#include <iostream>

#include "itkExceptionObject.h"
#include "itkImageFileReader.h"

#include "..\ImageStatistics.h"

TestImageStatistics::TestImageStatistics(void)
{
}

TestImageStatistics::~TestImageStatistics(void)
{
}

void TestImageStatistics::run()
{
    this->testStatistics();
}

void TestImageStatistics::testStatistics()
{
    try
    {
        typedef itk::ImageFileReader<ImageStatistics::ImageType> ReaderType;
        ReaderType::Pointer reader = ReaderType::New();
        std::string file = "C:\\UNC CS\\Research\\Nano\\data\\test\\Demons-RotateNoise-01.vtk";
        reader->SetFileName(file.c_str());
        std::cout << "Reading => " << file << std::endl;
        reader->Update();

        ImageStatistics *pStat = new ImageStatistics();
        std::cout << "Sending image to statistics calculator." << std::endl;
        pStat->SetImage(reader->GetOutput());

        std::cout << "Count     => " << pStat->GetCount() << std::endl;
        std::cout << "Mean      => " << pStat->GetMean() << std::endl;
        std::cout << "Deviation => " << pStat->GetDeviation() << std::endl;
        std::cout << "Minimum   => " << pStat->GetMinimum() << std::endl;
        std::cout << "Maximum   => " << pStat->GetMaximum() << std::endl;

        succeed_();
    }
    catch (itk::ExceptionObject &err)
    {
        std::cerr << "Exception caught!\n" << err << std::endl;
        fail_("Exception caught");
    }
}