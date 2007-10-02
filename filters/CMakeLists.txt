#######################################
# Filters are typically custom extensions to the ITK classes.
#######################################
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

#######################################
# Include fftw
# We use the fftw fourier transform libraries instead of ITK's vxl libraries.
# The ITK libraries seem to be buggy (e.g. two forward transforms do not yeild the same image)
#######################################
IF (WIN32)
    INCLUDE_DIRECTORIES(C:/Nano/bin/fftw)
    LINK_DIRECTORIES(C:/Nano/bin/fftw)
    SET (FFTW_LIBS  libfftw3l-3 libfftw3f-3)
ELSE (WIN32)
    LINK_LIBRARIES(fftw3 fftw3f)
ENDIF (WIN32)

# Note: the Dummy.cpp file is here to force cmake to use c++ linking for this project
SET (Filters_SRCS
    Dummy.cpp
                            AttenuationCalibrationFilter.h
                            AddConstantImageFilter.h
                            BilateralVectorImageFilter.h
                            CentralDifferenceImageFilter.h
                            CLGOpticFlowImageFilter.h
                            DerivativesToSurfaceImageFilter.h
                            itkFFTComplexToComplexImageFilter.h
                            itkFFTWComplexToComplexImageFilter.h
                            Gaussian2DVectorFilter.h
                            GaussianFunctionImageFilter.h
                            GaussianGradientImageFilter.h
                            HarrisFeatureInterestImageFilter.h
                            HornOpticalFlowImageFilter.h
                            HornOpticalFlowIterativeStepImageFilter.h
                            LocalContrastImageFilter.h
                            ModulateRegionImageFilter.h
                            MultiResolutionOpticalFlowMethod.h
                            MultiResolutionRegistration.h
                            NaryMeanImageFilter.h
                            NaryMeanVectorImageFilter.h
                            NaryMedianImageFilter.h
                            NaryMinimumImageFilter.h
                            NaryVarianceImageFilter.h
                            NonZeroLog10ImageFilter.h
                            PadFunctorImageFilter.h
                            PeakSNRImageToImageMetric.h
                            PercentileImageMetric.h
                            Power10ImageFilter.h
                            itkRealToComplexImageFilter.h
                            RegistrationMotionFilter.h
                            RungeKuttaSolver.h
                            WarpImageErrorFilter.h
)

ADD_LIBRARY(ITFilters ${Filters_SRCS})
TARGET_LINK_LIBRARIES(ITFilters ITKCommon ITKBasicFilters ITKIO ITKAlgorithms ${FFTW_LIBS} )
