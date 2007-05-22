PROJECT (ImageTracker)

SET (EXECUTABLE_OUTPUT_PATH "${ImageTracker_BINARY_DIR}/bin")
SET (LIBRARY_OUTPUT_PATH "${ImageTracker_BINARY_DIR}/lib")
SET (CMAKE_VERBOSE_MAKEFILE ON)
SET (CMAKE_COLOR_MAKEFILE ON)

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

#######################################
# Personal filter library and support classes.
#######################################
INCLUDE_DIRECTORIES (
	${ImageTracker_SOURCE_DIR}/common
	${ImageTracker_SOURCE_DIR}/image
	${ImageTracker_SOURCE_DIR}/filters
        ${ImageTracker_SOURCE_DIR}/pipelines)
SUBDIRS (common image filters pipelines)

#######################################
# Image processing library
#######################################
SET (Algorithms_SRCS
    BlurFilterSchedule.cxx                      BlurFilterSchedule.h
    DemonsPipeline.cxx                          DemonsPipeline.h
                                                Feature.h
    FeatureTrackingPipeline.cxx                 FeatureTrackingPipeline.h
    GlobalRegistrationPipeline.cxx              GlobalRegistrationPipeline.h
    HarrisFeatureDetector.cxx                   HarrisFeatureDetector.h
    ImageRegistration.cxx                       ImageRegistration.h
    ImageStatistics.cxx                         ImageStatistics.h
    MultiRegionRegistration.cxx                 MultiRegionRegistration.h
    MultiRegionRegistrationPipeline.cxx         MultiRegionRegistrationPipeline.h
                                                MultiResolutionRegistration.h
    RegistrationOutput.cxx                      RegistrationOutput.h
)

ADD_LIBRARY(ITAlgorithms ${Algorithms_SRCS})
TARGET_LINK_LIBRARIES(ITAlgorithms ITCommon ITImage ITFilters ITKCommon ITKBasicFilters ITKIO ITKAlgorithms )

INCLUDE_DIRECTORIES(
    ${ImageTracker_SOURCE_DIR}
    ${ImageTracker_SOURCE_DIR}/apps
    ${ImageTracker_SOURCE_DIR}/vtkView 
    ${ImageTracker_SOURCE_DIR}/wxGui )

#######################################
# Windowing components and executables
#######################################
SUBDIRS(vtkView wxGui apps)

#######################################
# Testing components
#######################################
OPTION (BUILD_IT_TESTS "Include ImageTracker unit tests" ON)
IF (BUILD_IT_TESTS)
    SUBDIRS(TestSuite Tests)
ENDIF (BUILD_IT_TESTS)
