#pragma once

#include <string>
#include "../TestSuite/Test.h"

class TestMultiRegionRegistration : public TestSuite::Test
{
public:
    TestMultiRegionRegistration();
    void run();
    void testUpdate();
};



