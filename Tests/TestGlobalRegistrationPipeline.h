#pragma once
#include "..\TestSuite\Test.h"

class TestGlobalRegistrationPipeline : public TestSuite::Test
{
public:
    TestGlobalRegistrationPipeline(void);
    ~TestGlobalRegistrationPipeline(void);
    void run(void);
    void testUpdateAll(void);
};
