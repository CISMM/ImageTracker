PROJECT (view)

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
INCLUDE (../LocalUsewxWidgets.cmake)

SET (ITView_SRCS
                            ConnectVTKITK.h
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

ADD_EXECUTABLE(vtk TestVtk.cxx)
TARGET_LINK_LIBRARIES(vtk ITCommon vtkIO vtkCommon vtkGraphics vtkRendering)
