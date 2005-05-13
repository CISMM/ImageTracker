###################################
# wxWindows GUI
###################################

# wxWindows Setup
INCLUDE( ${CMAKE_ROOT}/Modules/Use_wxWindows.cmake )

SET (wxGui_SRCS
    DialogLogger.cxx                DialogLogger.h
    DoubleSlider.cxx                DoubleSlider.h
    FileSetDialog.cxx               FileSetDialog.h
    GlobalRegistrationDialog.cxx    GlobalRegistrationDialog.h
	ImageCanvas.cxx                 ImageCanvas.h
	ImageViewer.cxx                 ImageViewer.h
	ImageWindow.cxx                 ImageWindow.h
	RegistrationDesign_wdr.cpp      RegistrationDesign_wdr.h
	wxGui.rc
)

# A library, so others can use the GUI tools
ADD_LIBRARY(ImageTrackerGui wxGui_SRCS)
TARGET_LINK_LIBRARIES(ImageTrackerGui ImageTrackerBase)

###################################
# The ImageTracker GUI executable.
#
# Note: It is *absolutely* necessary to include "WIN32" on the
# line below.  Failing to do that does not set up a wx application
# using the IMPLEMENT_APP() macro.
###################################
ADD_EXECUTABLE(ImageTracker WIN32 ImageTrackerApp.cxx)
TARGET_LINK_LIBRARIES(ImageTracker ImageTrackerBase ImageTrackerGui)