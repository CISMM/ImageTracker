#pragma once
#include "..\TestSuite\Test.h"

class TestThresholdPipeline : public TestSuite::Test
{
public:
    TestThresholdPipeline(void);
    ~TestThresholdPipeline(void);
    void run(void);
    void testUpdate(void);
};
