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
#    DoubleSlider.cxx                            DoubleSlider.h
    wxDoubleSlider.cpp                          wxDoubleSlider.h
    EmptyVisualizationControl.cpp               EmptyVisualizationControl.h
#    FileSetDialog.cxx                           FileSetDialog.h
#    GlobalRegistrationDialog.cxx                GlobalRegistrationDialog.h
    GuiUtils.cxx                                GuiUtils.h
    HornOpticalFlowDialog.cpp                   HornOpticalFlowDialog.h
#    HarrisFeatureDialog.cxx                     HarrisFeatureDialog.h
    ImageTracker.cpp                            ImageTracker.h
    ImageTrackerController.cxx                  ImageTrackerController.h
#    ImageViewer.cxx                             ImageViewer.h
    ItkVtkPipeline.cxx                          ItkVtkPipeline.h
    wxIntSlider.cpp                             wxIntSlider.h
    IntegrateFlowDialog.cpp                     IntegrateFlowDialog.h
#    LoggerPanel.cxx                             LoggerPanel.h
    MultiResolutionRegistrationDialog.cpp	    MultiResolutionRegistrationDialog.h
    PipelineExecutor.cxx                        PipelineExecutor.h
#    RegistrationDesign_wdr.cpp                  RegistrationDesign_wdr.h
    RemoveOcclusionsDialog.cpp                  RemoveOcclusionsDialog.h
    SaveVisualizationDialog.cpp                 SaveVisualizationDialog.h
    ScalarImageControlPanel.cpp                 ScalarImageControlPanel.h
    ScalarImageItkVtkPipeline.cxx               ScalarImageItkVtkPipeline.h
    VectorGlyphPipeline.cxx                     VectorGlyphPipeline.h
    VectorGlyphControlPanel.cpp                 VectorGlyphControlPanel.h
    WxPipelineObserver.cxx                      WxPipelineObserver.h
#    wxCore_wdr.cpp                              wxCore_wdr.h
#    wxGui.rc
)

# A library, so others can use the GUI tools
ADD_LIBRARY(ITGui ${Gui_SRCS})
TARGET_LINK_LIBRARIES(ITGui ITCommon ITAlgorithms ITPipelines ITView )
