#include ".\TestImageWindow.h"

#include "../Logger.h"
#include "../CommonTypes.h"
#include "../vtkView/ImageWindow.h"
#include <string>
#include "itkImageFileReader.h"

TestImageWindow::TestImageWindow(void)
{
}

TestImageWindow::~TestImageWindow(void)
{
}

void TestImageWindow::run()
{
    this->testShowImageWindow();
}

void TestImageWindow::testShowImageWindow()
{
    typedef itk::ImageFileReader<CommonTypes::InternalImageType> ReaderType;
    std::string file = "C:\\UNC_CS\\Research\\Nano\\data\\Lisa\\WH200311\\GoodfromWHrh01.png";
    
    Logger::logDebug("Loading image: " + file);
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(file.c_str());
    reader->Update();

    Logger::logDebug("Viewing image....");
    ImageWindow::ImShow(reader->GetOutput());
}