PROJECT (gui)

###################################
# wxWindows GUI
###################################
INCLUDE (../LocalUsewxWidgets.cmake)

SET (Gui_SRCS
    AboutDialog.cpp                             AboutDialog.h
    ApplyTransformDialog.cpp                    ApplyTransformDialog.h
    CLGOpticFlowDialog.cpp                      CLGOpticFlowDialog.h
    DataSourceDialog.cpp                        DataSourceDialog.h
    DoubleSlider.cxx                            DoubleSlider.h
    wxDoubleSlider.cxx                          wxDoubleSlider.h
    FileSetDialog.cxx                           FileSetDialog.h
    GlobalRegistrationDialog.cxx                GlobalRegistrationDialog.h
    GuiUtils.cxx                                GuiUtils.h
    HornOpticalFlowDialog.cpp                   HornOpticalFlowDialog.h
#    HarrisFeatureDialog.cxx                     HarrisFeatureDialog.h
    ImageTracker.cpp                            ImageTracker.h
    ImageTrackerController.cxx                  ImageTrackerController.h
#    ImageViewer.cxx                             ImageViewer.h
    ItkVtkPipeline.cxx                          ItkVtkPipeline.h
    wxIntSlider.cxx                             wxIntSlider.h
    LoggerPanel.cxx                             LoggerPanel.h
    MultiResolutionRegistrationDialog.cpp	    MultiResolutionRegistrationDialog.h
    PipelineExecutor.cxx                        PipelineExecutor.h
    RegistrationDesign_wdr.cpp                  RegistrationDesign_wdr.h
    RemoveOcclusionsDialog.cpp                  RemoveOcclusionsDialog.h
    ScalarImageItkVtkPipeline.cxx               ScalarImageItkVtkPipeline.h
    Vector2DGlyphItkVtkPipeline.cxx             Vector2DGlyphItkVtkPipeline.h
    WxPipelineObserver.cxx                      WxPipelineObserver.h
    wxCore_wdr.cpp                              wxCore_wdr.h
    wxGui.rc
)

# A library, so others can use the GUI tools
ADD_LIBRARY(ITGui ${Gui_SRCS})
TARGET_LINK_LIBRARIES(ITGui ITCommon ITAlgorithms ITPipelines ITView )
