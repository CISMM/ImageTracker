#pragma once
#include "..\TestSuite\Test.h"

class TestDemonsPipeline :
    public TestSuite::Test
{
public:
    TestDemonsPipeline(void);
    ~TestDemonsPipeline(void);
    void run(void);
    void testUpdate(void);
};
