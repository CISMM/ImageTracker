#pragma once

#include "itkImage.h"
#include "itkDiscreteGaussianImageFilter.h"

#include "CommonTypes.h"

/**
 * This class represents a group of bluring filters to be applied
 * in a particular sequence.  This may be useful for image
 * processing where we wish to iteratively approach a solution
 * using coarse methods initially and narrow in on a more precise
 * solution.  A BlurFilterSchedule will blur images with Gaussians
 * of different standard deviations--a large standard deviation will
 * produce a more blurred image.  Advancing through the filter
 * schedule will produce Images with less and less blurring.
 *
 * The behavior of this class is controlled by several parameters:
 * the maximum Gaussian standard deviation, the maximum Gaussian
 * kernel size, the number of blurring levels, and the source
 * image on which the filter schedule is operating.  If at any time
 * any of these parameters is set, the schedule will restart
 * from the beginning.
 *
 * The first blurred image produced by this filter schedule is 
 * blurred with a Gaussian kernel with a standard deviation equal
 * to the maximum standard deviation.  The standard deviation of
 * subsequently blurred images is decreased linearly until the last
 * level (level 0) of the filter schedule produces an image with 
 * *no* blurring, that is the original source image.
 */
class BlurFilterSchedule
{
public:
    //todo: Can we parameterize this class over image type?
    typedef CommonTypes::ImageType ImageType;    
    typedef itk::DiscreteGaussianImageFilter<ImageType, ImageType> BlurFilterType;

    /*
     * Creates a BlurFilterSchedule with the given configuration of
     * maximum Gaussian standard deviation, number of levels, and 
     * maximum kernel size.
     */
    BlurFilterSchedule(double minDeviation = 0.0, double maxDeviation = 3.0, int levels = 3, double maxKernelSize = 10.0);

    /*
     * Deletes a FilterSchedule.
     */
    ~BlurFilterSchedule(void);

    BlurFilterSchedule* Clone(void);

    /*
     * Sets the source image on which this BlurFilterSchedule will operate.
     */
    void SetSourceImage(const ImageType::Pointer image);

    /*
     * Returns the original source image on which this BlurFilterSchedule
     * is operating.
     */
    const ImageType::Pointer GetSourceImage(void);

    void SetMaxDeviation(double maxDeviation);
    void SetMaxKernelSize(double size);
    void SetNumberOfLevels(int levels);

    /*
     * Returns the value of the current Gaussian standard deviation.
     */
    double GetCurrentDeviation(void);

    /*
     * Returns the current level.
     */
    int GetCurrentLevel(void);

    /*
     * Determines whether this BlurFilterSchedule has any additional
     * levels below the current one.  (Level 0 is the last level.)
     */
    bool HasMoreLevels(void);

    /*
     * Returns a pointer to an image with the current level of blurring applied.
     */
    ImageType::Pointer CurrentBlurredImage(void);

    /*
     * Advances the level of blurring one step and returns an image with
     * that level of blurring applied.  Note, if this is the first time
     * an image is retrieved, the level of blurring will not be advanced.
     */
    ImageType::Pointer NextBlurredImage(void);

private:
    ImageType::Pointer sourceImage;
    double maxDeviation;
    double minDeviation;
    double maxKernelSize;

    int numberOfLevels;
    double stepSize;

    int currentLevel;
    bool blurAccessed;

    void resetSchedule(void);
};
