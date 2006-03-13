PROJECT(ImageTracker)

SET (EXECUTABLE_OUTPUT_PATH "${ImageTracker_BINARY_DIR}/bin")
SET (LIBRARY_OUTPUT_PATH "${ImageTracker_BINARY_DIR}/lib")

#######################################
# Include ITK
#######################################

FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
     "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

INCLUDE_DIRECTORIES(${ITK_INCLUDE_DIRS})
LINK_DIRECTORIES(${ITK_LIBRARY_DIRS})

#######################################
# Include VTK
#######################################

FIND_PACKAGE(VTK)
IF (VTK_FOUND)
  INCLUDE(${VTK_USE_FILE})
ELSE(VTK_FOUND)
  MESSAGE(FATAL_ERROR
     "Cannot build without VTK.  Please set VTK_DIR.")
ENDIF (VTK_FOUND)

INCLUDE_DIRECTORIES(${VTK_INCLUDE_DIRS})
LINK_DIRECTORIES(${VTK_LIBRARY_DIRS})

# wxWindows Setup
INCLUDE( ${CMAKE_ROOT}/Modules/Use_wxWindows.cmake )

#######################################
# Include ImageMagick
#######################################
INCLUDE (./vtkView/Use_ImageMagick.cmake)

#######################################
# Common architecture library
#######################################
SET (Common_SRCS
                    CommonTypes.h
    Logger.cxx      Logger.h
    StopWatch.cxx   StopWatch.h
)

#######################################
# Image processing library
#######################################
SET (Algorithms_SRCS
                                                BilateralVectorImageFilter.h
    BlurFilterSchedule.cxx                      BlurFilterSchedule.h
                                                CLGOpticFlowImageFilter.h
    CLGOpticFlowPipeline.cxx                    CLGOpticFlowPipeline.h
    DemonsPipeline.cxx                          DemonsPipeline.h
                                                Feature.h
    FeatureTrackingPipeline.cxx                 FeatureTrackingPipeline.h                                                
    FilePattern.cxx                             FilePattern.h
    FileSet.cxx                                 FileSet.h
    FileSetImageReader.cxx                      FileSetImageReader.h
                                                Gaussian2DVectorFilter.h
    GlobalRegistrationPipeline.cxx              GlobalRegistrationPipeline.h
    HarrisFeatureDetector.cxx                   HarrisFeatureDetector.h
    ImageRegistration.cxx                       ImageRegistration.h
    ImageStatistics.cxx                         ImageStatistics.h
    MultiImageRegistrationProcessor.cxx         MultiImageRegistrationProcessor.h
    MultiRegionRegistration.cxx                 MultiRegionRegistration.h
    MultiRegionRegistrationPipeline.cxx         MultiRegionRegistrationPipeline.h
                                                MultiResolutionRegistration.h
    MultiResolutionRegistrationPipeline.cxx		MultiResolutionRegistrationPipeline.h
                                                RegistrationMotionFilter.h
    RegistrationOutput.cxx                      RegistrationOutput.h
                                                Sort.h
    ThresholdPipeline.cxx                       ThresholdPipeline.h
    TransformGroup.cxx                          TransformGroup.h
    Use_ImageMagick.cmake
)
    
SET (CMAKE_VERBOSE_MAKEFILE ON)

ADD_LIBRARY(ITCommon ${Common_SRCS})
ADD_LIBRARY(ITAlgorithms ${Algorithms_SRCS})
TARGET_LINK_LIBRARIES(ITAlgorithms ITCommon ITKCommon ITKBasicFilters ITKIO ITKAlgorithms )


INCLUDE_DIRECTORIES(
    ${ImageTracker_SOURCE_DIR}
    ${ImageTracker_SOURCE_DIR}/vtkView 
    ${ImageTracker_SOURCE_DIR}/wxGui )

#######################################
# Window components
#######################################
SUBDIRS(vtkView wxGui)

#######################################
# Testing components
#######################################
OPTION (BUILD_IT_TESTS "Include ImageTracker unit tests" ON)
IF (BUILD_IT_TESTS)
    SUBDIRS(TestSuite Tests)
ENDIF (BUILD_IT_TESTS)