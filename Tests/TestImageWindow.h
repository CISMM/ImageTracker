#pragma once
#include "..\TestSuite\Test.h"

class TestImageWindow :
    public TestSuite::Test
{
public:
    TestImageWindow(void);
    ~TestImageWindow(void);
    void run(void);
    void testShowImageWindow();
};
