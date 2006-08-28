
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

# We use the fftw fourier transform libraries instead of ITK's vxl libraries.
# The ITK libraries seem to be buggy (e.g. two forward transforms do not yeild the same image)
LINK_LIBRARIES(fftw3 fftw3f)

# Note: the Dummy.cpp file is here to force cmake to use c++ linking for this project
SET (Filters_SRCS
    Dummy.cpp
                            AttenuationCalibrationFilter.h
                            AddConstantImageFilter.h
                            BilateralVectorImageFilter.h
                            CLGOpticFlowImageFilter.h
                            DerivativesToSurfaceImageFilter.h
                            Gaussian2DVectorFilter.h
                            GaussianFunctionImageFilter.h
							itkFFTComplexToComplexImageFilter.h
							itkFFTWComplexToComplexImageFilter.h
                            LocalContrastImageFilter.h
                            ModulateRegionImageFilter.h
                            NaryMeanImageFilter.h
                            NaryMeanVectorImageFilter.h
                            NaryMedianImageFilter.h
                            NaryMinimumImageFilter.h
                            NaryVarianceImageFilter.h
                            NonZeroLog10ImageFilter.h
                            RegistrationMotionFilter.h
                            WarpImageErrorFilter.h
)

ADD_LIBRARY(ITFilters ${Filters_SRCS})
TARGET_LINK_LIBRARIES(ITFilters ITKCommon ITKBasicFilters ITKIO ITKAlgorithms )
