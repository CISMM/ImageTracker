###################################
# Test Cases
###################################
SET (TestCases_SRCS
	AllTests.cxx
	TestDemonsPipeline.cxx
	TestFileSet.cxx
	TestFileSetImageReader.cxx
	TestGlobalRegistrationPipeline.cxx
	TestImageRescale.cxx
	TestImageStatistics.cxx
	TestLogger.cxx
	TestMultiRegionRegistration.cxx
	TestRegistrationOutput.cxx
	TestStopWatch.cxx
	TestThresholdPipeline.cxx
	TestTransformGroup.cxx
	TestVectorConvert.cxx
	TestVideoRegistration.cxx
)
ADD_EXECUTABLE(TestCases ${TestCases_SRCS})
TARGET_LINK_LIBRARIES(TestCases TestSuite ImageTracker ITKCommon ITKBasicFilters ITKIO ITKNumerics)
