#pragma once
#include "..\TestSuite\Test.h"

class TestRegistrationOutput : public TestSuite::Test
{
public:
    TestRegistrationOutput(void);
    ~TestRegistrationOutput(void);
    void run(void);
    void testUpdate(void);
};
