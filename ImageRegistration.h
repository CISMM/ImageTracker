#pragma once

#include "itkImage.h"
#include "itkImageRegistrationMethod.h"
#include "itkCenteredRigid2DTransform.h"
#include "itkMattesMutualInformationImageToImageMetric.h"
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkNormalizedCorrelationImageToImageMetric.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkRegularStepGradientDescentOptimizer.h"

#include "BlurFilterSchedule.h"
#include "CommonTypes.h"

/*
 * Provides a simple interface to image registration.  Since this class is
 * static, however, it can only perform a specific type of registration.  (It
 * may be possible to parameterize this class, but the first pass is statically
 * typed.)
 * 
 * Very simple image registration methods are available--two images can be
 * registered, with an optional initial transformation.
 *
 * The actual method by which the registration is carried out is hidden from the
 * user.  However, looking at the public registration typedefs, one can determine
 * what type of image registration methods are used and what types of transforms
 * can be expected (centered, rigid, 2D transforms).
 */
class ImageRegistration
{
public:
    /*
     * Creates an ImageRegistration instance.
     */
    ImageRegistration(void);
    /*
     * Deletes this ImageRegistration instance.
     */
    ~ImageRegistration(void);

    //Image types
    //todo: Can we parameterize class on this?
    typedef CommonTypes::ImageType ImageType;

    //Registration classes
    //todo: Can we parameterize class with these
    //todo: Or use configurable factories?
    typedef CommonTypes::TransformType TransformType;
    typedef TransformType::Pointer TransformPointer;
    //Try some different metrics...
    //typedef itk::MattesMutualInformationImageToImageMetric<ImageType, ImageType> MetricType;
    typedef itk::MeanSquaresImageToImageMetric<ImageType, ImageType> MetricType;
    //typedef itk::NormalizedCorrelationImageToImageMetric<ImageType, ImageType> MetricType;
    typedef itk::RegularStepGradientDescentOptimizer OptimizerType;
    typedef itk::LinearInterpolateImageFunction<ImageType, double> InterpolatorType;
    typedef itk::ImageRegistrationMethod<ImageType, ImageType> RegistrationType;

    //Image registration functions

    /*
     * Registers the two images given, returning the resulting transform.
     */
    TransformType::Pointer Register(ImageType::Pointer fixedImage, ImageType::Pointer movingImage);
    
    /*
     * Registers the two images given starting with the supplied initial transform, 
     * returning the resulting transform.
     */
    TransformType::Pointer Register(ImageType::Pointer fixedImage, ImageType::Pointer movingImage, TransformType::Pointer initialTransform);

    //todo: What sorts of parameters should we allow to be set through this interface?
    void SetBlurSchedule(BlurFilterSchedule* pBlurSchedule);

private:
    BlurFilterSchedule* pBlurSchedule;

    /*
     * Finds the center of an image.
     */
    TransformType::InputPointType FindImageCenter(const ImageType::Pointer image);
    
    /*
     * Sets up the optimizer for registration
     */
    void SetupOptimizer(OptimizerType::Pointer optimizer, TransformType::Pointer transform);

    /*
     * Computes a default initial transform for two images.  This transform will specify
     * no rotation, no translation, and will find the centers of the images.
     */
    TransformType::Pointer DefaultInitialTransform(
        ImageType::Pointer fixedImage, 
        ImageType::Pointer movingImage);

    /*
     * Internal registration method--performs the actual work associated with registering
     * two images.  Pre-processing may be applied to the images before registration.
     */
    TransformType::Pointer DoRegister(
        ImageType::Pointer fixedImage, 
        ImageType::Pointer movingImage, 
        TransformType::Pointer initialTransform, 
        bool verbose = false);

    /*
     * Reports registration results to standard out.
     */
    void PrintRegistrationResults(RegistrationType::Pointer registration, OptimizerType::Pointer optimizer);
};
