#include "HarrisFeatureDetector.h"

#include <vector>
#include <stdio.h>

#include "itkAddImageFilter.h"
#include "itkConstNeighborhoodIterator.h"
#include "itkImageDuplicator.h"
#include "itkRecursiveGaussianImageFilter.h"
#include "itkMultiplyImageFilter.h"
#include "itkSubtractImageFilter.h"

#include "Logger.h"
#include "Sort.h"

// TODO: Remove after debugging
#include "./wxGui/ImageWindow.h"

HarrisFeatureDetector::HarrisFeatureDetector(void)
{
}

HarrisFeatureDetector::~HarrisFeatureDetector(void)
{
}

/**
 * Finds Harris corner features within a given image.  The number of
 * features found is limited by maxCount.
 */
HarrisFeatureDetector::PointSetType::Pointer
    HarrisFeatureDetector::findFeatures(ImageType::Pointer image, int maxCount, double sigma)
{
     ImageType::Pointer weightImg = this->harrisWeight(image, sigma);
     PointSetType::Pointer features = this->findMaxima(weightImg, maxCount);

     return features;
}

/**
 * This image returns the most significant feature points from a
 * feature weight image.  The supplied image should be an intesity
 * map of "featureness" of image locations.  The features are extracted,
 * sorted, and the best maxCount number of them are returned in sorted
 * order.
 */
HarrisFeatureDetector::PointSetType::Pointer
    HarrisFeatureDetector::findMaxima(ImageType::Pointer image, int maxCount)
{
    Logger::logDebug("HarrisFeatureDetector: Finding local maxima.");
    // Create a neighborhood filter.  This allows us to traverse the image one
    // region at a time.  We'll use a 3x3 window--radius = 1.
    typedef itk::ConstNeighborhoodIterator<ImageType> NeighborhoodIteratorType;
    NeighborhoodIteratorType::RadiusType radius;
    radius.Fill(1);
    NeighborhoodIteratorType nit(radius, image, image->GetLargestPossibleRegion());

    // We need four offset positions for finding local maxima:
    NeighborhoodIteratorType::OffsetType offset1 = {{-1, 0}};
    NeighborhoodIteratorType::OffsetType offset2 = {{1, 0}};
    NeighborhoodIteratorType::OffsetType offset3 = {{0, -1}};
    NeighborhoodIteratorType::OffsetType offset4 = {{0, 1}};

    // Create a Feature list for storing our info
    std::vector<Feature>* features = new std::vector<Feature>;

    // TODO: Optimize this using itk::NeighborhoodAlgorithm::ImageBoundaryFacesCalculator
    for (nit.GoToBegin(); !nit.IsAtEnd(); ++nit)
    {
        ImageType::PixelType val = nit.GetCenterPixel();
        if (val > nit.GetPixel(offset1) &&
            val > nit.GetPixel(offset2) &&
            val > nit.GetPixel(offset3) &&
            val > nit.GetPixel(offset4))
        {
            // add point to Feature list--this is a candidate feature
            Feature feature(nit.GetIndex(), val);
            features->push_back(feature);
        }
    }

    // Sort features
    Sort<Feature>::mergeSort(*features, false);

    // Copy features into PointSet
    PointSetType::Pointer featurePoints = PointSetType::New();
    int pId = 0;

    for (std::vector<Feature>::iterator it = features->begin();
        it != features->end(), pId < maxCount;
        it++, pId++)
    {
        Feature feat = *it;

        ImageType::IndexType idx = feat.getLocation();
        PointSetType::PointType point;
        point[0] = (float) idx[0];
        point[1] = (float) idx[1];

        featurePoints->SetPoint(pId, point);
        featurePoints->SetPointData(pId, feat);
    }

    // Log results
    char* pointCount = itoa(featurePoints->GetNumberOfPoints(), new char[5], 10);
    char* featCount = itoa(features->size(), new char[5], 10);

    std::string msg = "Selected maxima: ";
    msg.append(pointCount).append("/").append(featCount);
    Logger::logDebug(msg);

    delete features;

    return featurePoints;
}

/**
 * Computes Harris corner feature weighting for a given image.  A Harris
 * feature has non-zero intensity gradients in two independent directions.
 * Another way to say this is that the local Hessian is non singular and
 * all singular values are large.
 */
HarrisFeatureDetector::ImageType::Pointer
    HarrisFeatureDetector::harrisWeight(ImageType::Pointer image, double sigma)
{
    Logger::logDebug("HarrisFeatureDetector: calculating Harris feature weights.");
    typedef itk::RecursiveGaussianImageFilter<ImageType, ImageType>
        DerivativeFilterType;
    typedef itk::ImageDuplicator<ImageType> DuplicatorType;
    typedef HarrisWeightImageFilter<ImageType, ImageType, ImageType, ImageType> HarrisWeightType;

    ImageType::Pointer Ixx, Iyy, Ixy;
    DerivativeFilterType::Pointer dx = DerivativeFilterType::New();
    DerivativeFilterType::Pointer dy = DerivativeFilterType::New();
    dx->SetDirection(0);
    dy->SetDirection(1);
    dx->SetSigma(sigma);
    dy->SetSigma(sigma);

    DuplicatorType::Pointer copier = DuplicatorType::New();

    // Setup the derivative-taking pipeline
    dx->SetInput(image);
    dy->SetInput(dx->GetOutput());
    copier->SetInputImage(dy->GetOutput());

    // Compute second order derivative Ixx
    dx->SetSecondOrder();
    dy->SetZeroOrder();
    dy->Update();
    copier->Update();
    Ixx = copier->GetOutput();

    // Compute second order derivative Iyy
    dx->SetZeroOrder();
    dy->SetSecondOrder();
    dy->Update();
    copier->Update();
    Iyy = copier->GetOutput();

    // Compute covariant derivative Ixy
    dx->SetFirstOrder();
    dy->SetFirstOrder();
    dy->Update();
    copier->Update();
    Ixy = copier->GetOutput();

    // DEBUG:
    //ImageWindow::ImShow(Ixx, "Ixx");
    //ImageWindow::ImShow(Iyy, "Iyy");
    //ImageWindow::ImShow(Ixy, "Ixy");

    // Compute Harris weights based on derivatives
    HarrisWeightType::Pointer weighter = HarrisWeightType::New();
    weighter->SetInput1(Ixx);
    weighter->SetInput2(Iyy);
    weighter->SetInput3(Ixy);
    // weighter->SetNumberOfThreads(1);
    ImageType::Pointer weightImg = weighter->GetOutput();
    weighter->Update();

    // DEBUG
    //char message[40];
    //sprintf(message, "HarrisWeights, sigma=%3.1f", sigma);
    //ImageWindow::ImShow(weightImg, message);

    return weightImg;
}