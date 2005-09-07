#pragma once
#include <iostream>
#include <string>

#include "Magick++.h"
#include "..\Logger.h"
#include "..\TestSuite\Test.h"

class TestImageMagick :
    public TestSuite::Test
{
public:
    TestImageMagick(void) {}
    ~TestImageMagick(void) {}

    void run()
    {
        this->testReadWrite();
    }

    void testReadWrite()
    {
        std::string dir("C:\\UNC_CS\\Research\\Nano\\data\\Fibrin\\lolo-2004-05-14-2narea\\");
        std::string file("mani402.png");
        Logger::logInfo("Magick++ reading: " + dir + file);
        Magick::Image inImg(dir + file);
        inImg.normalize();
        inImg.write(dir + "mani402-im++.png");
    }
};
