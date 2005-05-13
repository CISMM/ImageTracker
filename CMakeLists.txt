PROJECT(ImageTracker)

FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

#INCLUDE (${CMAKE_ROOT}/Modules/FindVTK.cmake)
#IF (USE_VTK_FILE)
#  INCLUDE(${USE_VTK_FILE})
#ENDIF (USE_VTK_FILE)

# Load the CableSwig settings used by ITK.
#SET(CableSwig_DIR ${ITK_CableSwig_DIR})
#FIND_PACKAGE(CableSwig)

INCLUDE_DIRECTORIES(${ITK_INCLUDE_DIRS})
#                    ${ItkJava_SOURCE_DIR})

LINK_DIRECTORIES(${ITK_LIBRARY_DIRS})

# wxWindows Setup
INCLUDE( ${CMAKE_ROOT}/Modules/Use_wxWindows.cmake )

#######################################
# Image processing library
#######################################
SET (ImageTrackerBase_SRCS
    BlurFilterSchedule.cxx                      BlurFilterSchedule.h
                                                CommonTypes.h
    DemonsPipeline.cxx                          DemonsPipeline.h
                                                Feature.h
	FilePattern.cxx                             FilePattern.h
	FileSet.cxx                                 FileSet.h
	FileSetImageReader.cxx                      FileSetImageReader.h
	GlobalRegistrationPipeline.cxx              GlobalRegistrationPipeline.h
	HarrisFeatureDetector.cxx                   HarrisFeatureDetector.h
	ImageRegistration.cxx                       ImageRegistration.h
	ImageStatistics.cxx                         ImageStatistics.h
	Logger.cxx                                  Logger.h
	MultiImageRegistrationProcessor.cxx         MultiImageRegistrationProcessor.h
	MultiRegionRegistration.cxx                 MultiRegionRegistration.h
	MultiRegionRegistrationPipeline.cxx         MultiRegionRegistrationPipeline.h
	RegistrationOutput.cxx                      RegistrationOutput.h
	                                            Sort.h
	StopWatch.cxx                               StopWatch.h
	ThresholdPipeline.cxx                       ThresholdPipeline.h
	TransformGroup.cxx                          TransformGroup.h
)
ADD_LIBRARY(ImageTrackerBase ImageTrackerBase_SRCS)
TARGET_LINK_LIBRARIES(ImageTrackerBase ImageTrackerGui ITKCommon ITKBasicFilters ITKIO ITKAlgorithms)


#######################################
# wxWindows and Test suites
#######################################
SUBDIRS(wxGui TestSuite Tests)