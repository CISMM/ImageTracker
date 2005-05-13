# wxWindows Setup
INCLUDE( ${CMAKE_ROOT}/Modules/Use_wxWindows.cmake )

###################################
# Test Cases
###################################
SET (TestCases_SRCS
	TestDemonsPipeline.cxx
	TestFileSet.cxx
	TestFileSetImageReader.cxx
	TestGlobalRegistrationPipeline.cxx
	TestHarrisFeatureDetector.cxx
	TestImageRescale.cxx
	TestImageStatistics.cxx
	TestImageWindow.cxx
	TestLogger.cxx
	TestMultiRegionRegistration.cxx
	TestRegistrationOutput.cxx
	                                TestSort.h
	TestStopWatch.cxx
	TestThresholdPipeline.cxx
	TestTransformGroup.cxx
	TestVectorConvert.cxx
	                                TestVectorWrite.h
	TestVideoRegistration.cxx
	wxTestApp.cxx                   wxTestApp.h
	wxTestDesigns_wdr.cpp           wxTestDesigns_wdr.h
)
# ADD_EXECUTABLE(TestCases ${TestCases_SRCS})
ADD_EXECUTABLE(TestCases WIN32 ${TestCases_SRCS})
TARGET_LINK_LIBRARIES(TestCases TestSuite ImageTrackerBase ImageTrackerGui)


