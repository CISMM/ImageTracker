#pragma once
#include <string>

#include "..\ItkMagickIO.h"
#include "..\Logger.h"
#include "..\TestSuite\Test.h"
#include "..\wxGui\ImageWindow.h"

class TestItkMagickIO : public TestSuite::Test
{
public:
    TestItkMagickIO() {}
    ~TestItkMagickIO() {}

    void run()
    {
        this->testRead();
    }

    void testRead()
    {
        std::string dir = "C:\\UNC_CS\\Research\\Nano\\data\\Fibrin\\lolo-2004-05-14-2narea\\";
        std::string name = "mani402.tif";

        std::string msg = "";
        char num[40];
        itoa(sizeof(Magick::Quantum), num, 10);
        Logger::logInfo(msg.append("Magick::Quantum:\t").append(num));

        msg = "";
        itoa(sizeof(ItkMagickIO::MagickPixelType), num, 10);
        Logger::logInfo(msg.append("MagickPixelType (unsigned char):\t").append(num));

        msg = "";
        itoa(sizeof(CommonTypes::InputPixelType), num, 10);
        Logger::logInfo(msg.append("InputPixelType (unsigned short):\t").append(num));

        msg = "";
        itoa(sizeof(CommonTypes::InternalPixelType), num, 10);
        Logger::logInfo(msg.append("InternalPixelType (float):\t").append(num));

        Logger::logDebug("Reading image through ImageMagick...");
        ItkMagickIO::InternalImageType::Pointer img = ItkMagickIO::Read(dir + name);
        Logger::logDebug("Displaying image...");
        ImageWindow::ImShow(img, name);
    }
};