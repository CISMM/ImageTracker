#######################################
# Use wxWidgets
#######################################
SET (WXWINDOWS_USE_GL 1)

# According to Use_wxWindows.cmake, this is the right way to use wxWidgets:
# New for cmake 2.4.3, we have to specify which wx libraries we think we'll
# use.  Awesome.
SET(wxWidgets_USE_LIBS base core gl)
FIND_PACKAGE(wxWidgets)
IF (wxWidgets_FOUND)
    INCLUDE(${wxWidgets_USE_FILE})
    LINK_LIBRARIES( ${wxWidgets_LIBRARIES} )
ELSE (wxWidgets_FOUND)
    MESSAGE("Could not find wxWidgets.  Please install.")
ENDIF(wxWidgets_FOUND)

IF(VTK_USE_X)
    # Find GTK, from Mathieu Malaterre's wxVTKRenderWindowInteractor sample project
    FIND_PROGRAM(CMAKE_PKG_CONFIG pkg-config ../gtk/bin ../../gtk/bin)
    IF(CMAKE_PKG_CONFIG)
        SET(CMAKE_GTK_CXX_FLAGS "`${CMAKE_PKG_CONFIG} --cflags gtk+-2.0`")
        SET(GTK_LIBRARIES "`${CMAKE_PKG_CONFIG} --libs gtk+-2.0`")
        SET(CMAKE_CXX_FLAGS "${CMAKE_GTK_CXX_FLAGS} ${CMAKE_CXX_FLAGS}")
    ELSE(CMAKE_PKG_CONFIG)
        FIND_PROGRAM(CMAKE_GTK_CONFIG gtk-config ../gtk/bin ../../gtk/bin)
        IF(CMAKE_GTK_CONFIG)
            SET(CMAKE_GTK_CXX_FLAGS "`${CMAKE_GTK_CONFIG} --cxxflags`")
            SET(GTK_LIBRARIES "`${CMAKE_GTK_CONFIG} --libs`")
            SET(CMAKE_CXX_FLAGS "${CMAKE_GTK_CXX_FLAGS} ${CMAKE_CXX_FLAGS}")
        ELSE(CMAKE_GTK_CONFIG)
            # DOH ! We need to default to CMake's modules
            MESSAGE("Falling back on CMake's GTK module...")
            FIND_PACKAGE(GTK)
            INCLUDE_DIRECTORIES(${GTK_INCLUDE_DIR})
        ENDIF(CMAKE_GTK_CONFIG)
    ENDIF(CMAKE_PKG_CONFIG)
ENDIF(VTK_USE_X)


# INCLUDE (${CMAKE_ROOT}/Modules/UsewxWidgets.cmake)

# -------------------------------------------------------------
# The following setup of wxWindows is from Mathieu Malaterre's
# wxVTKRenderWindowInteractor sample project.
# FIND_PACKAGE(wxWindows)
# IF ( CMAKE_WX_CAN_COMPILE )
# 
# IF(WXWINDOWS_INCLUDE_DIR)
#   INCLUDE_DIRECTORIES(${WXWINDOWS_INCLUDE_DIR})
# ENDIF(WXWINDOWS_INCLUDE_DIR)
# 
# IF(CMAKE_WX_CXX_FLAGS)
#   SET(CMAKE_CXX_FLAGS "${CMAKE_WX_CXX_FLAGS} ${CMAKE_CXX_FLAGS}")
# ENDIF(CMAKE_WX_CXX_FLAGS)
# 
# IF(VTK_USE_X)
#   # Is this always true, do we really need gtk all the time ?
# 
#   FIND_PROGRAM(CMAKE_PKG_CONFIG pkg-config ../gtk/bin ../../gtk/bin)
#   IF(CMAKE_PKG_CONFIG)
#     SET(CMAKE_GTK_CXX_FLAGS "`${CMAKE_PKG_CONFIG} --cflags gtk+-2.0`")
#     SET(GTK_LIBRARIES "`${CMAKE_PKG_CONFIG} --libs gtk+-2.0`")
#     SET(CMAKE_CXX_FLAGS "${CMAKE_GTK_CXX_FLAGS} ${CMAKE_CXX_FLAGS}")
#   ELSE(CMAKE_PKG_CONFIG)
#     FIND_PROGRAM(CMAKE_GTK_CONFIG gtk-config ../gtk/bin ../../gtk/bin)
#     IF(CMAKE_GTK_CONFIG)
#       SET(CMAKE_GTK_CXX_FLAGS "`${CMAKE_GTK_CONFIG} --cxxflags`")
#       SET(GTK_LIBRARIES "`${CMAKE_GTK_CONFIG} --libs`")
#       SET(CMAKE_CXX_FLAGS "${CMAKE_GTK_CXX_FLAGS} ${CMAKE_CXX_FLAGS}")
#     ELSE(CMAKE_GTK_CONFIG)
#       # DOH ! We need to default to CMake's modules
#       FIND_PACKAGE(GTK)
#       INCLUDE_DIRECTORIES(${GTK_INCLUDE_DIR})
#     ENDIF(CMAKE_GTK_CONFIG)
#   ENDIF(CMAKE_PKG_CONFIG)
# 
# ENDIF(VTK_USE_X)
# 
# LINK_LIBRARIES( ${WXWINDOWS_LIBRARY} )
# 
# ELSE ( CMAKE_WX_CAN_COMPILE )
#   MESSAGE("Cannot find wxWindows libraries and/or header files")
# ENDIF ( CMAKE_WX_CAN_COMPILE )
# # -------------------------------------------------------------
# 
# #######################################
# # To make things interesting, wxWidgets' setup.h file appears
# # in different locations on different platforms. Also, there
# # are several setup.h files, but one seems to be quite 
# # important.  The module loader included with CMake seems
# # to have broken with wxWidgets 2.6.3--specifically it does
# # not find the platform-dependent header file.
# #######################################
# 
# IF (WIN32)
#   FIND_PATH (WXWINDOWS_MSVC_DIR wx/setup.h
#   	${WXWINDOWS_ROOT_DIR}/include/msvc
#   )
#   IF (WXWINDOWS_MSVC_DIR)
#     INCLUDE_DIRECTORIES(${WXWINDOWS_MSVC_DIR})
#   ENDIF (WXWINDOWS_MSVC_DIR)
#   
#   MARK_AS_ADVANCED(WXWINDOWS_MSVC_DIR)
# ENDIF (WIN32)