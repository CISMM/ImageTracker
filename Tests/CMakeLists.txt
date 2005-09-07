# wxWindows Setup
INCLUDE( ${CMAKE_ROOT}/Modules/Use_wxWindows.cmake )

# ImageMagick Setup
# INCLUDE ( ../Use_ImageMagick.cmake )

###################################
# Test Cases
###################################
SET (TestCases_SRCS
                                            TestBilateralVectorFilter.h
                                            TestCLGOpticFlowImageFilter.h
	TestDemonsPipeline.cxx
	TestFileSet.cxx
	TestFileSetImageReader.cxx
	TestGlobalRegistrationPipeline.cxx
	TestHarrisFeatureDetector.cxx
	                                        TestImageMagick.h
	TestImageRescale.cxx
	TestImageStatistics.cxx
	TestImageWindow.cxx
	                                        TestItkMagickIO.h
	TestLogger.cxx
	                                        TestLogStream.h
	TestMultiRegionRegistration.cxx
                                            TestRandomVectorImage.h
	TestRegistrationOutput.cxx
	                                        TestSort.h
	TestStopWatch.cxx
	TestThresholdPipeline.cxx
	TestTransformGroup.cxx
	TestVectorConvert.cxx
	                                        TestVectorWrite.h
	TestVideoRegistration.cxx
	wxTestApp.cxx                           wxTestApp.h
	wxTestDesigns_wdr.cpp                   wxTestDesigns_wdr.h
)
# ADD_EXECUTABLE(TestCases ${TestCases_SRCS})
ADD_EXECUTABLE(TestCases WIN32 ${TestCases_SRCS})
TARGET_LINK_LIBRARIES(TestCases TestSuite ITCommon vtkView ITAlgorithms ITGui )


