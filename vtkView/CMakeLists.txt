PROJECT (vtkView)

#######################################
# This project brings several open-source packages together
# and makes them usable in an application.  ImageMagick is 
# used to read images because it has superior TIFF handlers.
# ITK is used for image processing because it has a complete
# set of well-built algorithms, and is extensible.  VTK is 
# used for visualization because it is easy to use.  wxWindows
# is used as a GUI framework because it is cross-platform and
# has some good features.
#######################################

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
# Use wxWidgets
#######################################
SET (WXWINDOWS_USE_GL 1) 
INCLUDE (${CMAKE_ROOT}/Modules/UsewxWidgets.cmake)

IF (WIN32)
  FIND_PATH (WXWINDOWS_MSVC_DIR wx/setup.h
  	${WXWINDOWS_ROOT_DIR}/include/msvc
  )
  IF (WXWINDOWS_MSVC_DIR)
    INCLUDE_DIRECTORIES(${WXWINDOWS_MSVC_DIR})
  ENDIF (WXWINDOWS_MSVC_DIR)
  
  MARK_AS_ADVANCED(WXWINDOWS_MSVC_DIR)
ENDIF (WIN32)

SET (ITView_SRCS
							ConnectVTKITK.h
	ImageWindow.cxx			ImageWindow.h
	ItkMagickIO.cxx			ItkMagickIO.h
	VtkCanvas.cxx			VtkCanvas.h
	VtkViewer.cxx			VtkViewer.h
	vtkViewer_wdr.cpp		vtkViewer_wdr.h
	wxVTKRenderWindowInteractor.cxx	wxVTKRenderWindowInteractor.h
)

ADD_LIBRARY(ITView ${ITView_SRCS})
TARGET_LINK_LIBRARIES(ITView
	ITCommon
	vtkIO
	vtkCommon
	vtkGraphics
	vtkRendering
	ITKCommon 
	ITKBasicFilters 
	ITKIO 
	ITKAlgorithms
)

IF (WIN32)
    SET (GUI_EXE WIN32)
ENDIF (WIN32)

ADD_EXECUTABLE(viewer ${GUI_EXE} VtkViewerApp.cxx)
TARGET_LINK_LIBRARIES(viewer ITView)
