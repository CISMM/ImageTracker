PROJECT (gui)

SET (Gui_SRCS
    AboutDialog.cpp                             AboutDialog.h
    ApplyTransformDialog.cpp                    ApplyTransformDialog.h
    CLGOpticalFlowDialog.cpp                    CLGOpticalFlowDialog.h
    CompositeImageFileSet.cxx                   CompositeImageFileSet.h
						                        ConnectVTKITK.h
    EmptyVisualizationControl.cpp               EmptyVisualizationControl.h
    FilePatternDialog.cpp                       FilePatternDialog.h
    FilePatternPanel.cpp                        FilePatternPanel.h
    FileSetPanel.cpp                            FileSetPanel.h
    FilterControlPanel.cxx                      FilterControlPanel.h
    FlatfieldPanel.cpp                          FlatfieldPanel.h
    GaussianFilterPanel.cpp                     GaussianFilterPanel.h
    GradientMagnitudePanel.cpp                  GradientMagnitudePanel.h
    HornOpticalFlowDialog.cpp                   HornOpticalFlowDialog.h
    ImageFileSetPanel.cpp                       ImageFileSetPanel.h
    ImageTracker.cpp                            ImageTracker.h
    ImageTrackerController.cxx                  ImageTrackerController.h
    ItkVtkPipeline.cxx                          ItkVtkPipeline.h
    IntegrateFlowDialog.cpp                     IntegrateFlowDialog.h
    LogarithmPanel.cpp                          LogarithmPanel.h
    LoggerDialog.cpp                            LoggerDialog.h
    LookupTableUtil.cxx                         LookupTableUtil.h
    MultiResolutionRegistrationDialog.cpp       MultiResolutionRegistrationDialog.h
    PipelineExecutor.cxx                        PipelineExecutor.h
    RegionPanel.cpp                             RegionPanel.h
    RemoveOcclusionsDialog.cpp                  RemoveOcclusionsDialog.h
    SaveFilterImageDialog.cpp                   SaveFilterImageDialog.h
    SaveVisualizationDialog.cpp                 SaveVisualizationDialog.h
    ScalarImageControlDialog.cpp                ScalarImageControlDialog.h
    ScalarImageControlPanel.cpp                 ScalarImageControlPanel.h
    ScalarImageVisualization.cxx                ScalarImageVisualization.h
    ThresholdPanel.cpp                          ThresholdPanel.h
    VectorGlyphControlPanel.cpp                 VectorGlyphControlPanel.h
    VectorGlyphVisualization.cxx                VectorGlyphVisualization.h
    VectorHeightMapControlPanel.cpp             VectorHeightMapControlPanel.h
    VectorHeightMapVisualization.cxx            VectorHeightMapVisualization.h
    WxPipelineObserver.cxx                      WxPipelineObserver.h
)

# A library, so others can use the GUI tools
ADD_LIBRARY(ITGui ${Gui_SRCS})
TARGET_LINK_LIBRARIES(ITGui ITCommon ITPipelines nanoWx nanoVTKView)
