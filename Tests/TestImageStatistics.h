#pragma once
#include "..\TestSuite\Test.h"

class TestImageStatistics :
    public TestSuite::Test
{
public:
    TestImageStatistics(void);
    ~TestImageStatistics(void);

    void run(void);
    void testStatistics(void);
};
