#pragma once
#include "..\TestSuite\Test.h"

class TestFileSetImageReader :
    public TestSuite::Test
{
public:
    TestFileSetImageReader(void);
    ~TestFileSetImageReader(void);

    void run(void);
    void testFileSetNames();
    void testHasNext();
};
