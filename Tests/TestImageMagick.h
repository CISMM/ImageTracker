#pragma once
#include <iostream>
#include <string>

#include "Magick++.h"
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
        std::string dir = "C:\\UNC_CS\\Research\\Nano\\data\\Fibrin\\lolo-2004-05-14-2narea\\";
        Magick::Image inImg(dir + "mani402.tif");
        inImg.normalize();
        inImg.write(dir + "mani402-im++.tif");
    }
};
