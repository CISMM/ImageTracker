#pragma once
#include "..\TestSuite\Test.h"

class TestFileSet :
    public TestSuite::Test
{
public:
    TestFileSet(void);
    ~TestFileSet(void);
    void run(void);
    void testCreateFileSet();
};
