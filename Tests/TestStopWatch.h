#pragma once
#include "..\TestSuite\Test.h"

class TestStopWatch :
    public TestSuite::Test
{
public:
    TestStopWatch(void);
    ~TestStopWatch(void);

    void run(void);
    void testStartStop(void);
    void sleep(long secs);
};
