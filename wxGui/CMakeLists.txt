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

#ADD_EXECUTABLE(wxGui WIN32 ${wxGui_SRCS})
#TARGET_LINK_LIBRARIES(wxGui ImageTracker ITKCommon ITKBasicFilters ITKIO ITKNumerics)

ADD_EXECUTABLE(ImageViewer WIN32 wxGui_SRCS)
TARGET_LINK_LIBRARIES(ImageViewer ImageTracker ITKCommon ITKBasicFilters ITKIO ITKNumerics)