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
	RegistrationDesign_wdr.cpp      RegistrationDesign_wdr.h
	wxGui.rc
)

ADD_EXECUTABLE(ImageTracker WIN32 wxGui_SRCS)
TARGET_LINK_LIBRARIES(ImageTracker ImageTrackerBase ITKCommon ITKBasicFilters ITKIO ITKNumerics)