#pragma once
#include "..\TestSuite\Test.h"

class TestLogger : public TestSuite::Test
{
public:
    TestLogger(void);
    ~TestLogger(void);
    void run();
    void testStreamLogger();
    void testDelete();
};
