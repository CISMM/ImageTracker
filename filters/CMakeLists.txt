
PROJECT (Filters)

#######################################
# Include ITK
#######################################
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
     "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

INCLUDE_DIRECTORIES(${ITK_INCLUDE_DIRS})
LINK_DIRECTORIES(${ITK_LIBRARY_DIRS})

# Note: the Dummy.cpp file is here to force cmake to use c++ linking for this project
SET (Filters_SRCS
    Dummy.cpp
                            AttenuationCalibrationFilter.h
                            BilateralVectorImageFilter.h
                            CLGOpticFlowImageFilter.h
                            Gaussian2DVectorFilter.h
                            GaussianFunctionImageFilter.h
                            NaryMeanImageFilter.h
                            NaryMeanVectorImageFilter.h
                            NaryVarianceImageFilter.h
                            RegistrationMotionFilter.h
                            WarpImageErrorFilter.h
)

ADD_LIBRARY(ITFilters ${Filters_SRCS})
TARGET_LINK_LIBRARIES(ITFilters ITKCommon ITKBasicFilters ITKIO ITKAlgorithms )
