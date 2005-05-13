#pragma once
#include "..\TestSuite\Test.h"

class TestHarrisFeatureDetector :
    public TestSuite::Test
{
public:
    TestHarrisFeatureDetector(void);
    ~TestHarrisFeatureDetector(void);

    void run();
    void testDetector();
};
