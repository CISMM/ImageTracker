#######################################
# Use wxWidgets
#######################################
SET (WXWINDOWS_USE_GL 1)

# According to Use_wxWindows.cmake, this is the right way to use wxWidgets:
# New for cmake 2.4.3, we have to specify which wx libraries we think we'll
# use.  Awesome.
# SET(wxWidgets_USE_LIBS base core gl)
FIND_PACKAGE(wxWidgets REQUIRED)
IF (wxWidgets_FOUND)
    INCLUDE(${wxWidgets_USE_FILE})
    LINK_LIBRARIES( ${wxWidgets_LIBRARIES} )
ELSE (wxWidgets_FOUND)
    MESSAGE("Could not find wxWidgets.  Please install.")
ENDIF(wxWidgets_FOUND)

# From Mathieu Malaterre's wxVTKRenderWindowInteractor sample project--new for wxWidgets 2.8
IF(WIN32)
  SET(GUI_EXECUTABLE WIN32)
ELSE(WIN32)
  IF(APPLE)
    SET(GUI_EXECUTABLE MACOSX_BUNDLE)
    IF(VTK_USE_COCOA)
      SET_SOURCE_FILES_PROPERTIES(
        src/wxVTKRenderWindowInteractor.cxx
        PROPERTIES COMPILE_FLAGS "-ObjC++")
    ENDIF(VTK_USE_COCOA)
  ELSE(APPLE)
    # Ok X11 for sure, but just check:
    IF(NOT VTK_USE_X)
      MESSAGE(FATAL_ERROR "You need to have VTK_USE_X")
    ENDIF(NOT VTK_USE_X)
    #FIND_PACKAGE(GTK REQUIRED)
    #INCLUDE_DIRECTORIES(${GTK_INCLUDE_DIR} )
    # CMake 2.4.6:
    FIND_PACKAGE(PkgConfig REQUIRED)
    pkg_check_modules (GTK2 gtk+-2.0)
    # For GTK we need a couple of stuff:
    # gl: GLCanvas
    # adv: wxSashLayoutWindow and such...
    SET( wxWidgets_USE_LIBS gl adv base core)
  ENDIF(APPLE)
ENDIF(WIN32)

