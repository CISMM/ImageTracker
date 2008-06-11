PROJECT (gui)

SET (Gui_SRCS
    AboutDialog.cpp                             AboutDialog.h
    ApplyTransformDialog.cpp                    ApplyTransformDialog.h
    CLGOpticFlowDialog.cpp                      CLGOpticFlowDialog.h
						ConnectVTKITK.h
    DataSourceDialog.cpp                        DataSourceDialog.h
    EmptyVisualizationControl.cpp               EmptyVisualizationControl.h
    FilePatternDialog.cpp                       FilePatternDialog.h
    FilePatternPanel.cpp                        FilePatternPanel.h
    GuiUtils.cxx                                GuiUtils.h
    HornOpticalFlowDialog.cpp                   HornOpticalFlowDialog.h
    ImageTracker.cpp                            ImageTracker.h
    ImageTrackerController.cxx                  ImageTrackerController.h
    ItkVtkPipeline.cxx                          ItkVtkPipeline.h
    IntegrateFlowDialog.cpp                     IntegrateFlowDialog.h
    LookupTableUtil.cxx                         LookupTableUtil.h
    MultiResolutionRegistrationDialog.cpp	MultiResolutionRegistrationDialog.h
    PipelineExecutor.cxx                        PipelineExecutor.h
    RemoveOcclusionsDialog.cpp                  RemoveOcclusionsDialog.h
    SaveVisualizationDialog.cpp                 SaveVisualizationDialog.h
    ScalarImageControlPanel.cpp                 ScalarImageControlPanel.h
    ScalarImageItkVtkPipeline.cxx               ScalarImageItkVtkPipeline.h
    VectorGlyphControlPanel.cpp                 VectorGlyphControlPanel.h
    VectorGlyphPipeline.cxx                     VectorGlyphPipeline.h
    VectorHeightMapControlPanel.cpp             VectorHeightMapControlPanel.h
    VectorHeightMapPipeline.cxx                 VectorHeightMapPipeline.h
    WxPipelineObserver.cxx                      WxPipelineObserver.h
)

# A library, so others can use the GUI tools
ADD_LIBRARY(ITGui ${Gui_SRCS})
TARGET_LINK_LIBRARIES(ITGui ITCommon ITPipelines nanoWx nanoVTKView)
