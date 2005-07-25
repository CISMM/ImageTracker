#pragma once

#include "itkCovariantVector.h"
#include "itkImage.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkRigid2DTransform.h"
#include "itkTranslationTransform.h"
#include "itkImageRegistrationMethod.h"
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkNormalizedCorrelationImageToImageMetric.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkPointSet.h"
#include "itkRegularStepGradientDescentOptimizer.h"
#include "itkVector.h"
#include "itkRegionOfInterestImageFilter.h"

#include "CommonTypes.h"
#include "Feature.h"
#include "ImageRegistration.h"

/**
 * Performs image registration of multiple regions of interest.
 * The result of this registration is a Vector field of the same
 * size as the input images.  For each point on the Moving Image,
 * a region of interest (ROI) is computed from a user-specified 
 * pixel radius of interest.  This ROI is then registered with
 * a larger region on the fixed image to determine how that region
 * likely moved from one image to the next.  The resultant
 * registration parameters are used as the vector data for that
 * point in the output vector field.
 */
class MultiRegionRegistration
{
public:
    MultiRegionRegistration(int radius = 5, double roiRatio = 2.0)
    {
        this->SetRadiusOfInterest(radius);
        this->SetROIRatio(roiRatio);
    }
    ~MultiRegionRegistration(void) {}

    // Input / Ouput types
    // MUST be float, and MUST be CovariantVector--other seemingly similar
    // types don't work!
    const static unsigned int VectorDimension = 2;
    typedef float ComponentType;
    typedef itk::Vector<ComponentType, VectorDimension> VectorType;

    typedef CommonTypes::ImageType InputImageType;
    typedef itk::Image<VectorType, CommonTypes::Dimension> OutputImageType;

    // Processing types
    typedef itk::RegionOfInterestImageFilter<InputImageType, InputImageType> ROIFilterType;
    typedef itk::ImageRegionIteratorWithIndex<InputImageType> IteratorType;

    // Registration types
    typedef itk::TranslationTransform<double, 2> TransformType;
    // typedef itk::MeanSquaresImageToImageMetric<InputImageType, InputImageType> MetricType;
    typedef itk::NormalizedCorrelationImageToImageMetric<InputImageType, InputImageType> MetricType;
    typedef itk::RegularStepGradientDescentOptimizer OptimizerType;
    typedef itk::LinearInterpolateImageFunction<InputImageType, double> InterpolatorType;
    typedef itk::ImageRegistrationMethod<InputImageType, InputImageType> RegistrationType;
    typedef itk::PointSet<Feature, 2> PointSetType;

    void SetFixedImage(InputImageType::Pointer fixed) { this->fixedImage = fixed; }
    void SetMovingImage(InputImageType::Pointer moving) { this->movingImage = moving; }
    int GetRadiusOfInterest() { return this->radiusOfInterest; }
    double GetROIRatio() { return this->roiRatio; }
    void SetRadiusOfInterest(int pixels) { this->radiusOfInterest = pixels; }
    void SetROIRatio(double ratio) { this->roiRatio = ratio; }
    OutputImageType::Pointer Update(void);
    OutputImageType::Pointer Update(PointSetType::Pointer features);

private:
    TransformType::Pointer DoRegister(InputImageType::Pointer fixed, InputImageType::Pointer moving);

    /**
     * Creats an image containing a sub-region of interest from the given input
     * image.  The subregion is centered at the given pixel center and has the
     * given pixel radius.  If a resulting region intersects the boundary of
     * the input image, the ROI is truncated along that edge.
     */
    InputImageType::Pointer CreateROI(InputImageType::Pointer image, InputImageType::IndexType pixelCenter, int pixelRadius);
    
    /**
     * Creates an empty vector image of the same size as the given input
     * image.
     */
    OutputImageType::Pointer CreateVectorImage(const InputImageType::Pointer image);

    void InitProgress(InputImageType::Pointer image);
    void UpdateProgress(InputImageType::IndexType index, OutputImageType::PixelType pixel);

    InputImageType::Pointer movingImage;
    InputImageType::Pointer fixedImage;
    int radiusOfInterest;
    double roiRatio;
    InputImageType::SizeType size;
};
