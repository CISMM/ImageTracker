#include "TestImageRescale.h"
#include "itkExceptionObject.h"

TestImageRescale::TestImageRescale(void)
{
}

TestImageRescale::~TestImageRescale(void)
{
}

void TestImageRescale::run(void)
{
    this->testRescaleImage();
}

void TestImageRescale::testRescaleImage(void)
{ 
    try
    {
        std::string dir = "C:\\UNC CS\\Research\\Nano\\data\\Fibrin\\Lolo-2004-08-24-man1\\";
        std::string fileNameIn = "man1_fiber2001.tif";
        std::string fileIn = dir + fileNameIn;
        std::string fileNameOut = "8bit-" + fileNameIn;
        std::string fileOut = dir + fileNameOut;

        TestImageRescale::ReaderType::Pointer reader = TestImageRescale::ReaderType::New();
        TestImageRescale::RescaleType::Pointer scaler = TestImageRescale::RescaleType::New();
        TestImageRescale::WriterType::Pointer writer = TestImageRescale::WriterType::New();

        reader->SetFileName(fileIn.c_str());
        writer->SetFileName(fileOut.c_str());
        scaler->SetInput(reader->GetOutput());
        writer->SetInput(scaler->GetOutput());
        writer->Update();
        succeed_();
    }
    catch (itk::ExceptionObject &err)
    {
        std::cout << "Exception caught!" << std::endl;
        std::cout << err << std::endl;
        fail_("Exception occurred.");
    }
}