PROJECT (gui)

###################################
# wxWindows GUI
###################################
INCLUDE (../LocalUsewxWidgets.cmake)

SET (Gui_SRCS
    CLGOpticFlowDialog.cxx                      CLGOpticFlowDialog.h
    DataSource.cxx                              DataSource.h
    DoubleSlider.cxx                            DoubleSlider.h
    wxDoubleSlider.cxx                          wxDoubleSlider.h
    FileSetDialog.cxx                           FileSetDialog.h
    GlobalRegistrationDialog.cxx                GlobalRegistrationDialog.h
    HarrisFeatureDialog.cxx                     HarrisFeatureDialog.h
    ImageTracker.cpp                            ImageTracker.h
    ImageViewer.cxx                             ImageViewer.h
    wxIntSlider.cxx                             wxIntSlider.h
    LoggerPanel.cxx                             LoggerPanel.h
    MultiResolutionRegistrationDialog.cxx	MultiResolutionRegistrationDialog.h
    RegistrationDesign_wdr.cpp                  RegistrationDesign_wdr.h
    wxCore_wdr.cpp                              wxCore_wdr.h
    wxGui.rc
)

# A library, so others can use the GUI tools
ADD_LIBRARY(ITGui ${Gui_SRCS})
TARGET_LINK_LIBRARIES(ITGui ITCommon ITAlgorithms ITView )
