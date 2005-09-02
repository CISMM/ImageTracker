###################################
# wxWindows GUI
###################################

# wxWindows Setup
INCLUDE( ${CMAKE_ROOT}/Modules/Use_wxWindows.cmake )

SET (Gui_SRCS
    CLGOpticFlowDialog.cxx          CLGOpticFlowDialog.h
    DialogLogger.cxx                DialogLogger.h
    DoubleSlider.cxx                DoubleSlider.h
    FileSetDialog.cxx               FileSetDialog.h
    GlobalRegistrationDialog.cxx    GlobalRegistrationDialog.h
    HarrisFeatureDialog.cxx         HarrisFeatureDialog.h
    ImageViewer.cxx                 ImageViewer.h
	RegistrationDesign_wdr.cpp      RegistrationDesign_wdr.h
	wxGui.rc
)

# A library, so others can use the GUI tools
ADD_LIBRARY(ITGui Gui_SRCS)
TARGET_LINK_LIBRARIES(ITGui ITCommon ITAlgorithms vtkView )

###################################
# The ImageTracker GUI executable.
#
# Note: It is *absolutely* necessary to include "WIN32" on the
# line below. (e.g. ADD_EXECUTABLE(ImageTracker WIN32 ImageTrackerApp.cxx))
# Failing to do that does not set up a wx application using the 
# IMPLEMENT_APP() macro.
###################################
ADD_EXECUTABLE(ImageTracker WIN32 ImageTrackerApp.cxx)
TARGET_LINK_LIBRARIES(ImageTracker vtkView ITGui ITCommon ITAlgorithms)