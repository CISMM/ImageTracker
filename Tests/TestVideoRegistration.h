#pragma once
#include "..\TestSuite\Test.h"

class TestVideoRegistration : 
    public TestSuite::Test
{
public:
    TestVideoRegistration(void);
    ~TestVideoRegistration(void);
    void run(void);
    void testUpdate(void);
};
