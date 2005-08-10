PROJECT(ImageTracker)

FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

#INCLUDE (${CMAKE_ROOT}/Modules/FindVTK.cmake)
#IF (VTK_USE_FILE)
#  INCLUDE(${VTK_USE_FILE})
#ENDIF (USE_VTK_FILE)

# Load the CableSwig settings used by ITK.
#SET(CableSwig_DIR ${ITK_CableSwig_DIR})
#FIND_PACKAGE(CableSwig)

INCLUDE_DIRECTORIES(${ITK_INCLUDE_DIRS})
#                    ${ItkJava_SOURCE_DIR})

LINK_DIRECTORIES(${ITK_LIBRARY_DIRS})

# wxWindows Setup
INCLUDE( ${CMAKE_ROOT}/Modules/Use_wxWindows.cmake )

# ImageMagick setup
INCLUDE ( ./Use_ImageMagick.cmake )

#######################################
# Image processing library
#######################################
SET (ImageTrackerBase_SRCS
                                                BilateralVectorImageFilter.h
    BlurFilterSchedule.cxx                      BlurFilterSchedule.h
                                                CLGOpticFlowImageFilter.h
    CLGOpticFlowPipeline.cxx                    CLGOpticFlowPipeline.h
                                                CommonTypes.h
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
	ItkMagickIO.cxx                             ItkMagickIO.h
	Logger.cxx                                  Logger.h
	LogStream.cxx                               LogStream.h
	MultiImageRegistrationProcessor.cxx         MultiImageRegistrationProcessor.h
	MultiRegionRegistration.cxx                 MultiRegionRegistration.h
	MultiRegionRegistrationPipeline.cxx         MultiRegionRegistrationPipeline.h
	RegistrationOutput.cxx                      RegistrationOutput.h
	                                            Sort.h
	StopWatch.cxx                               StopWatch.h
	ThresholdPipeline.cxx                       ThresholdPipeline.h
	TransformGroup.cxx                          TransformGroup.h
	Use_ImageMagick.cmake
)

SET (MAGICK_LIB 
#	CORE_RL_bzlib_
#	CORE_RL_coders_
#	CORE_RL_filters_
#	CORE_RL_jbig_
#	CORE_RL_jp2_
# 	CORE_RL_jpeg_
# 	CORE_RL_lcms_
# 	CORE_RL_libxml_
	CORE_RL_magick_ 
# 	CORE_RL_png_
# 	CORE_RL_tiff_
# 	CORE_RL_ttf_
# 	CORE_RL_wmf_
# 	CORE_RL_xlib_
# 	CORE_RL_zlib_
#   CORE_RL_wand_
# 	X11
# 	Xext
	)
	
SET (CMAKE_VERBOSE_MAKEFILE ON)
ADD_LIBRARY(ImageTrackerBase ImageTrackerBase_SRCS)
TARGET_LINK_LIBRARIES(ImageTrackerBase ImageTrackerGui ITKCommon ITKBasicFilters ITKIO ITKAlgorithms ${MAGICK_LIB} )


#######################################
# wxWindows and Test suites
#######################################
SUBDIRS(wxGui TestSuite Tests)
