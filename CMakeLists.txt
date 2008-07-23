PROJECT (ImageTracker)

cmake_minimum_required(VERSION 2.4)

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 OLD)
endif(COMMAND cmake_policy)


SET (EXECUTABLE_OUTPUT_PATH "${ImageTracker_BINARY_DIR}/bin")
SET (LIBRARY_OUTPUT_PATH "${ImageTracker_BINARY_DIR}/lib")

OPTION (CMAKE_VERBOSE_MAKEFILE  "Enable/Disable verbose compiler output" ON)
OPTION (CMAKE_COLOR_MAKEFILE "Enable/Disable color cues when building" ON)
MARK_AS_ADVANCED (CLEAR CMAKE_VERBOSE_MAKEFILE CMAKE_COLOR_MAKEFILE)

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
# Handle threading libraries
#######################################
FIND_PACKAGE(Threads)
IF (CMAKE_USE_PTHREADS_INIT)
    ADD_DEFINITIONS(-DIT_USE_PTHREADS)
ELSEIF (CMAKE_USE_WIN32_THREADS_INIT)
    ADD_DEFINITIONS(-DIT_USE_WIN32_THREADS)
ENDIF(CMAKE_USE_PTHREADS_INIT)

#######################################
# Build nanoCommon
#######################################
set (NANO_COMMON_DIR ../nanoCommon/cpp)
include (LocalUsewxWidgets.cmake)
subdirs (${NANO_COMMON_DIR}/wx)
subdirs (${NANO_COMMON_DIR}/wxvtk)

#######################################
# Personal filter library and support classes.
#
# ImageTracker is split up into several logical groups.
# Common    - contains application architecture components
# Image     - contains image handling components
# Filters   - contains image processing filters--typically ITK-based templated classes
# Pipelines - contains classes that perform specific image processing tasks on sets of
# images--this usually combines multiple itk and custom filters
#######################################
INCLUDE_DIRECTORIES (
    ${ImageTracker_SOURCE_DIR}/common
    ${ImageTracker_SOURCE_DIR}/image
    ${ImageTracker_SOURCE_DIR}/filters
    ${ImageTracker_SOURCE_DIR}/pipelines)
SUBDIRS (common image filters pipelines)

#######################################
# Windowing components and executables
#######################################
INCLUDE_DIRECTORIES(
    ${ImageTracker_SOURCE_DIR}
    ${NANO_COMMON_DIR}/wx
    ${NANO_COMMON_DIR}/wxvtk
    ${ImageTracker_SOURCE_DIR}/apps 
    ${ImageTracker_SOURCE_DIR}/wxGui )

SUBDIRS(wxGui apps)

#######################################
# Testing components
#######################################
OPTION (BUILD_IT_TESTS "Include ImageTracker unit tests" ON)
IF (BUILD_IT_TESTS)
    SUBDIRS(TestSuite Tests)
ENDIF (BUILD_IT_TESTS)
