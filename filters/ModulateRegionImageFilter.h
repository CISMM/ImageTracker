#pragma once

#include <vector>

#include "itkImageToImageFilter.h"

/**
 * /class ModulateRegionImageFilter
 *
 * /description Modulates circular regions of an image by a scalar. This
 * serves to simulate the effect of a partial light occlusion on part of
 * the image. The measurements (e.g. center and radius) of the modulated 
 * regions are done in real coordinates. For an image with origin at 
 * (0, 0, ...) and pixel spacing (1, 1, ...), this corresponds to pixel
 * coodinates.
 *
 * Author: Brian Eastwood, (C) 2006
 *
 * Copyright: See COPYING file that comes with this distribution
 */
template< class TImage >
class ModulateRegionImageFilter :
public itk::ImageToImageFilter< TImage, TImage >
{
public:
    // Common itk typedefs
    typedef ModulateRegionImageFilter Self;
    typedef itk::ImageToImageFilter< TImage, TImage > Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    
    itkNewMacro(Self);
    itkTypeMacro(ModulateRegionImageFilter, ImageToImageFilter);
    
    typedef TImage ImageType;
    typedef typename ImageType::PointType PointType;
    typedef typename ImageType::IndexType IndexType;
    typedef typename ImageType::RegionType RegionType;
    typedef typename ImageType::PointType::RealType RealType;
    
    /**
     * Adds a modulated region to this image filter. The region is specified
     * by the physical center of the region, the radius, and the factor by
     * which to modulate image intensities that fall inside this region.
     */
    void AddRegion(const PointType& center, RealType radius, double ratio);
    
protected:
    ModulateRegionImageFilter(){}
    virtual ~ModulateRegionImageFilter(){}
    
    /**
     * Computes the output image for a given image region. As each output image 
     * pixel is independent of every other, this image filter can use multiple
     * threads. At each pixel, we check if the current pixel lies within any of
     * the filter modulation regions. If so, we modulate the output pixel by
     * the appropriate ratio. (Note: a particular pixel may be within multiple
     * modulation regions.)
     */
    virtual void ThreadedGenerateData(const RegionType& region, int threadId);
    
private:
    // Intentionally unimplemented for copy protection
    ModulateRegionImageFilter(const Self&);
    void operator=(const Self&);

    std::vector< PointType > centers;
    std::vector< RealType > sqRadii;
    std::vector< double > ratios;
};

// Implementation //

#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIteratorWithIndex.h"

template < class TImage >
void ModulateRegionImageFilter<TImage>::
AddRegion(const PointType& center, RealType radius, double ratio)
{
    this->centers.push_back(center);
    this->sqRadii.push_back((radius*radius));
    this->ratios.push_back(ratio);
}

template < class TImage >
void ModulateRegionImageFilter<TImage>::
ThreadedGenerateData(const RegionType& region, int threadId)
{
    typedef itk::ImageRegionConstIterator< ImageType > InputIteratorType;
    typedef itk::ImageRegionIteratorWithIndex< ImageType > OutputIteratorType;
    
    // Set up image iterators
    InputIteratorType inputIt(this->GetInput(), region);
    OutputIteratorType outputIt(this->GetOutput(), region);
    
    // Iterate through image regions. At each point, we check all modulation
    // regions to see if we need to modulate the output intensity.
    for (inputIt.GoToBegin(), outputIt.GoToBegin();
        !(inputIt.IsAtEnd() || outputIt.IsAtEnd());
        ++inputIt, ++outputIt)
    {
        // get the input image value
        typename ImageType::PixelType value = inputIt.Get();
        
        // find the current physical image position
        PointType point;	
        this->GetInput()->TransformIndexToPhysicalPoint(inputIt.GetIndex(), point);
        
        // check whether the current position is within any of the 
        // modulating regions. (A given pixel may be within more than
        // one modulation region.)
        for (unsigned int i = 0; i < this->centers.size(); i++)
        {
            RealType distance = point.SquaredEuclideanDistanceTo(this->centers[i]);
            if (distance <= this->sqRadii[i])    // the point falls within the ith region
                value = static_cast<typename ImageType::PixelType>(this->ratios[i] * value); // modulate
        }
        
        outputIt.Set(value);
    }
}
