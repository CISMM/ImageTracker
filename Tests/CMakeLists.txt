PROJECT(tests)

INCLUDE_DIRECTORIES (../TestSuite)

# wxWindows Setup
INCLUDE(../LocalUsewxWidgets.cmake)

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
                                            TestImagePyramid.h
    TestImageRescale.cxx
    TestImageStatistics.cxx
    TestImageWindow.cxx
                                            TestItkMagickIO.h
    TestLogger.cxx
    TestMultiRegionRegistration.cxx
                                            TestMultiResolutionRegistration.h
                                            TestMultiResolutionRegistrationPipeline.h
                                            TestNaryMeanImageFilter.h
                                            TestRandomVectorImage.h
    TestRegistrationOutput.cxx
                                            TestRegistrationMotionFilter.h
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

SET( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:msvcrt" )
ADD_EXECUTABLE(TestCases ${TestCases_SRCS})
# TODO: the logger window from the GUI-based test client causes crashing.  
# Probably due to a change in the wx library: "can't crerate wxWindow without parent."
# ADD_EXECUTABLE(TestCases WIN32 ${TestCases_SRCS})
TARGET_LINK_LIBRARIES(TestCases TestSuite ITCommon ITAlgorithms ITImage ITView ITGui )


