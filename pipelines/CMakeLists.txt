#######################################
# Pipelines are typically collections of ITK classes composed to 
# perform a specific task.
#######################################
PROJECT (Pipelines)

SET (Pipelines_SRCS
    ApplyTransformsPipeline.cxx             ApplyTransformsPipeline.h
    CLGOpticFlowPipeline.cxx                CLGOpticFlowPipeline.h
    HornOpticalFlowPipeline.cxx             HornOpticalFlowPipeline.h
    ItkPipeline.cxx                         ItkPipeline.h
                                            ItkPipelineObserver.h
    MultiResolutionRegistrationPipeline.cxx MultiResolutionRegistrationPipeline.h
    RemovePartialOcclusionsPipeline.cxx     RemovePartialOcclusionsPipeline.h
    TextPipelineObserver.cxx                TextPipelineObserver.h
)

ADD_LIBRARY (ITPipelines ${Pipelines_SRCS})
TARGET_LINK_LIBRARIES(ITPipelines ITCommon ITImage ITFilters)
