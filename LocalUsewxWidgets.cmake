
#######################################
# Use wxWidgets
#######################################
SET (WXWINDOWS_USE_GL 1) 
INCLUDE (${CMAKE_ROOT}/Modules/UsewxWidgets.cmake)

#######################################
# To make things interesting, wxWidgets' setup.h file appears
# in different locations on different platforms. Also, there
# are several setup.h files, but one seems to be quite 
# important.  The module loader included with CMake seems
# to have broken with wxWidgets 2.6.3--specifically it does
# not find the platform-dependent header file.
#######################################

IF (WIN32)
  FIND_PATH (WXWINDOWS_MSVC_DIR wx/setup.h
  	${WXWINDOWS_ROOT_DIR}/include/msvc
  )
  IF (WXWINDOWS_MSVC_DIR)
    INCLUDE_DIRECTORIES(${WXWINDOWS_MSVC_DIR})
  ENDIF (WXWINDOWS_MSVC_DIR)
  
  MARK_AS_ADVANCED(WXWINDOWS_MSVC_DIR)
ENDIF (WIN32)