#pragma once
#include "..\TestSuite\Test.h"

class TestVectorConvert :
    public TestSuite::Test
{
public:
    TestVectorConvert(void);
    ~TestVectorConvert(void);
    void run(void);
    void testConversion(void);
};
