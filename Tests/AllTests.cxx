#include <iostream>

#include "../TestSuite/Suite.h"

#include "TestDemonsPipeline.h"
#include "TestFileSet.h"
#include "TestFileSetImageReader.h"
#include "TestGlobalRegistrationPipeline.h"
#include "TestImageRescale.h"
#include "TestImageStatistics.h"
#include "TestLogger.h"
#include "TestMultiRegionRegistration.h"
#include "TestRegistrationOutput.h"
#include "TestStopWatch.h"
#include "TestThresholdPipeline.h"
#include "TestTransformGroup.h"
#include "TestVectorConvert.h"
#include "TestVideoRegistration.h"

// Turn on debugging
#ifndef DEBUG
#define DEBUG true

int main()
{
    TestSuite::Suite suite("Video Registration Suite");
    // suite.addTest(new TestFileSet);
    // suite.addTest(new TestFileSetImageReader);
    // suite.addTest(new TestGlobalRegistrationPipeline);
    suite.addTest(new TestLogger);
    // suite.addTest(new TestRegistrationOutput);
    // suite.addTest(new TestThresholdPipeline);
    // suite.addTest(new TestTransformGroup);
    // suite.addTest(new TestVideoRegistration);
    // suite.addTest(new TestMultiRegionRegistration);
    // suite.addTest(new TestStopWatch);
    // suite.addTest(new TestImageStatistics);
    // suite.addTest(new TestDemonsPipeline);
    // suite.addTest(new TestVectorConvert);
    // suite.addTest(new TestImageRescale);
    suite.run();
    long errs = suite.report();
    suite.free();
    return errs;
}
#endif // Debug