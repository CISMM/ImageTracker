PROJECT(ImageTracker)

FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

INCLUDE (${CMAKE_ROOT}/Modules/FindVTK.cmake)
IF (USE_VTK_FILE)
  INCLUDE(${USE_VTK_FILE})
ENDIF (USE_VTK_FILE)

# Load the CableSwig settings used by ITK.
SET(CableSwig_DIR ${ITK_CableSwig_DIR})
FIND_PACKAGE(CableSwig)

INCLUDE_DIRECTORIES(${ITK_INCLUDE_DIRS}
                    ${ItkJava_SOURCE_DIR})

LINK_DIRECTORIES(${ITK_LIBRARY_DIRS})


#######################################
# Image processing library
#######################################
SET (ImageTracker_SRCS
    BlurFilterSchedule.cxx
    CommonTypes.h
    DemonsPipeline.cxx
	FilePattern.cxx
	FileSet.cxx
	FileSetImageReader.cxx
	ImageRegistration.cxx
	ImageStatistics.cxx
	MultiImageRegistrationProcessor.cxx
	MultiRegionRegistration.cxx
	MultiRegionRegistrationPipeline.cxx
	RegistrationOutput.cxx
	StopWatch.cxx
	ThresholdPipeline.cxx
	TransformGroup.cxx
)
ADD_LIBRARY(ImageTracker ImageTracker_SRCS)
TARGET_LINK_LIBRARIES(ImageTracker ITKBasicFilters)


IF(READY_FOR_PRIME_TIME)
  IF(ITK_CSWIG_JAVA)
    SET_SOURCE_FILES_PROPERTIES(ImageTracker_SRCS GENERATED)
    ADD_LIBRARY(ImageTrackerJava SHARED ImageTracker_SRCS)
    TARGET_LINK_LIBRARIES(ImageTrackerJava ITKBasicFilters
    SwigRuntimceJava ${JAVA_LIBRARY})
    ADD_CUSTOM_COMMAND(
      COMMENT "ImageTracker_wrapTcl.cxx from ImageTracker.i"
      SOURCE ${ImageTracker_SOURCE_DIR}/ImageTracker.i
      COMMAND ${CableSwig_cswig_EXE}
      ARGS -I${ImageTracker_SOURCE_DIR}
           -nocable -noruntime ${IGNORE_WARNINGS}
           -o ImageTracker_wrapTcl.cxx
           -c++ ${ImageTracker_SOURCE_DIR}/ImageTracker.i
      TARGET ImageTrackerJava
      OUTPUTS ${ImageTracker_BINARY_DIR}/ImageTracker_wrapTcl.cxx
      DEPENDS ${ImageTracker_SOURCE_DIR}/ImageTracker.i)
  ENDIF(ITK_CSWIG_JAVA)
ENDIF(READY_FOR_PRIME_TIME)

#######################################
# Test directories
#######################################
SUBDIRS(TestSuite Tests)