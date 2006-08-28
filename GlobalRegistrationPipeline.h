#pragma once
#include "CommonTypes.h"
#include "FileSet.h"
#include "ImageSetReader.h"
#include "ImageRegistration.h"

#include "itkDiscreteGaussianImageFilter.h"
#include "itkExceptionObject.h"
#include "itkImageRegistrationMethod.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkProcessObject.h"
#include "itkRecursiveGaussianImageFilter.h"
#include "itkRegularStepGradientDescentOptimizer.h"
#include "itkStatisticsImageFilter.h"
#include "itkThresholdImageFilter.h"

class RegistrationHelper;

/*
 * Handles Registration with thresholding and smoothing.  Each given input image is
 * first thresholded and then smoothed before registration.  Thresholding allows one
 * to eliminate high or low valued pixels from the registration.  Smoothing allows for
 * ignoring the fine-featured details of image objects.
 */
class GlobalRegistrationPipeline
{
public:
    typedef CommonTypes::InputImageType InputImageType;
    typedef CommonTypes::InternalImageType ImageType;
    typedef ImageSetReader<InputImageType, ImageType> ReaderType;
    typedef CommonTypes::TransformType TransformType;
    typedef std::vector<TransformType::Pointer> TransformVector;
    typedef itk::RecursiveGaussianImageFilter<ImageType, ImageType> SmoothType;
    typedef itk::ThresholdImageFilter<ImageType> ThresholdType;
    typedef itk::ImageRegistrationMethod<ImageType, ImageType> RegistrationType;
    typedef itk::StatisticsImageFilter<ImageType> StatsType;
    
    /*
     * Default constructor.
     */
    GlobalRegistrationPipeline(void);

    /*
     * Destructor.
     */
    ~GlobalRegistrationPipeline(void);

    /*
     * Tie the appropriate filter and registration components together in the proper order.
     */
    void InitializePipeline();

    /*
     * Update the variables of the filter components.
     */
    void UpdatePipeline();

    /*
     * Register all images in the input FileSet.  Each image is registered with the previous
     * image in the sequence.  The set of transforms for all image registrations is returned.
     */
    TransformVector * UpdateAll();

    /*
     * Register one pair of images from the input FileSet.  The input FileSet is advanced one
     * image pair.  The result of the registration is returned.
     */
    TransformType::Pointer UpdateOne();

    /*
     * Indicates whether all files in the input FileSet have been registered.
     */
    bool IsRegistrationFinished();

    /*
     * Saves all transforms to this pipeline's transform file.
     */
    void SaveTransforms();

    /*
     * Saves all transformed images to the output FileSet (using RegistrationOutput).
     */
    void SaveImages();

    /*
     * Sets the upper limit for input image pixels.
     */
    void SetThresholdAbove(ThresholdType::PixelType * above);

    /*
     * Sets the lower limit for input image pixels.
     */
    void SetThresholdBelow(ThresholdType::PixelType * below);

    /*
     * Sets the degree of smoothing to apply to input images.
     */
    void SetSmoothSigma(double sigma);

    /*
     * Sets the list of image files to use for registration.
     */
    void SetInput(const FileSet& input);

    /*
     * Sets the output image list.
     */
    void SetOutput(const FileSet& output);

    /*
     * Sets the name of the file in which to save transform information.
     */
    void SetTransformFile(std::string transformFile);

    const FileSet& GetInput();
    const FileSet& GetOutput();
    std::string GetTransformFile();

    /*
     * Gets the "current" image of the input FileSet.  This will be
     * the next image to be considered fixed in the group of registrations.
     */
    ImageType::Pointer GetCurrentFixedImage();

    /*
     * Gets the "next" image of the input FileSet.  This will be
     * the next image to be considered moving in the group of registrations.
     */
    ImageType::Pointer GetCurrentMovingImage();

    /*
     * Gets the list of all transforms performed to date.
     */
    const TransformVector& GetTransforms();

    void GetMinMax(ImageType::PixelType &min, ImageType::PixelType &max)
    {
        StatsType::Pointer stats = StatsType::New();
        stats->SetInput(this->reader[this->index]);
        min = stats->GetMinimum();
        max = stats->GetMaximum();
    }

private:
    bool init;
    unsigned int index;
    FileSet inputFiles;
    ReaderType reader;

    ThresholdType::Pointer thresholderF;
    ThresholdType::Pointer thresholderM;
    ThresholdType::PixelType * above;
    ThresholdType::PixelType * below;

    SmoothType::Pointer smootherFx;
    SmoothType::Pointer smootherFy;
    SmoothType::Pointer smootherMx;
    SmoothType::Pointer smootherMy;
    double sigma;

    RegistrationHelper * registrar;
    
    TransformVector transforms;
    FileSet outputFiles;
    std::string transformFile;
};

/*
 * Helps with image registration.  Actually does all the work.
 */
class RegistrationHelper
{
public:
    typedef CommonTypes::InternalImageType ImageType;
    typedef CommonTypes::TransformType TransformType;
    typedef itk::MeanSquaresImageToImageMetric<ImageType, ImageType> MetricType;
    typedef itk::RegularStepGradientDescentOptimizer OptimizerType;
    typedef itk::LinearInterpolateImageFunction<ImageType, TransformType::ScalarType> InterpolatorType;
    typedef itk::ImageRegistrationMethod<ImageType, ImageType> RegistrationType;

    /*
     * Default constructor.
     */
    RegistrationHelper(void);

    /*
     * Destructor.
     */
    ~RegistrationHelper(void);

    /*
     * Initialize all registration components.
     */
    void Initialize();
    
    /*
     * Register two images.  The initial registration parameters will be computed from
     * the image dimensions.
     */
    TransformType::Pointer Register(ImageType::Pointer fixed, ImageType::Pointer moving);
    
    /*
     * Register the two given images given an initial transform.
     */
    TransformType::Pointer Register(ImageType::Pointer fixed, ImageType::Pointer moving, TransformType::Pointer initialTransform);
    
    RegistrationType::Pointer registration;
    MetricType::Pointer metric;
    OptimizerType::Pointer optimizer;
    InterpolatorType::Pointer interpolator;

private:
    /*
     * Setup the registration optimizer.
     */
    void SetupOptimizer();
    
    /*
     * Find the center of the given image.
     */
    TransformType::InputPointType FindImageCenter(const ImageType::Pointer image);
    
    /*
     * Compute the default initial transform for two images.
     */
    TransformType::Pointer DefaultInitialTransform(ImageType::Pointer fixed, ImageType::Pointer moving);

};
