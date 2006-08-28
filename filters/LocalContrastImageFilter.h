#pragma once

#include "itkImageToImageFilter.h"

/**
 * Computes a local contrast metric at each point in an image. The
 * contrast metric is a ratio of the difference in intensity value
 * across a pixel to the intensity value at the pixel. The intensity
 * difference calculation is centered about each pixel (as opposed to
 * a forward or backward difference). The contrast metric is
 * directional, so this filter has an associated direction to it.
 */
template < class TInput, class TOutput >
class LocalContrastImageFilter :
public itk::ImageToImageFilter< TInput, TOutput >
{
public:
    // Standard itk typedefs.
    typedef LocalContrastImageFilter Self;
    typedef itk::ImageToImageFilter< TInput, TOutput > Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    
    itkNewMacro(Self);
    itkTypeMacro(LocalContrastImageFilter, ImageToImageFilter);
    
    // Helpful typedefs
    typedef TInput InputImageType;
    typedef TOutput OutputImageType;
    typedef typename InputImageType::Pointer InputPointerType;
    typedef typename OutputImageType::Pointer OutputPointerType;
    typedef typename InputImageType::PixelType InputPixelType;
    typedef typename OutputImageType::PixelType OutputPixelType;

    itkStaticConstMacro(ImageDimension, unsigned int, InputImageType::ImageDimension);

    itkGetMacro(Direction, unsigned int);
    itkSetMacro(Direction, unsigned int);

protected:
    LocalContrastImageFilter() :
        m_Direction(0)
    {}
    
    virtual ~LocalContrastImageFilter(){}

    void GenerateInputRequestedRegion() throw (itk::InvalidRequestedRegionError);
    void GenerateData();

private:
    // purposefully not implemented
    LocalContrastImageFilter(const Self& other);
    void operator=(const Self& other);
    
    // the filter direction
    unsigned int m_Direction;
};



/** Implementation **/

#include "itkConstNeighborhoodIterator.h"
#include "itkImageRegionIterator.h"
#include "itkRegion.h"

#include "ImageUtils.h"
#include "Logger.h"

template < class TInput, class TOutput >
void LocalContrastImageFilter< TInput, TOutput >
::GenerateInputRequestedRegion() throw (itk::InvalidRequestedRegionError)
{
    Superclass::GenerateInputRequestedRegion();
    
    // Grab the default requested region
    InputPointerType input = const_cast<InputImageType *> (this->GetInput());
    if (!input)
    {
        Logger::warning << "LocalContrastImageFilter: Input image not set." << std::endl;
        return;
    }
    typename InputImageType::RegionType region = input->GetRequestedRegion();
    
    // Create a size by which to pad the image region--we want a radius
    // of one along the filter direction
    typename InputImageType::RegionType::SizeType padSize;
    padSize.Fill(0);
    padSize[this->GetDirection()] = 1;
    region.PadByRadius(padSize);
    
    // Crop the requested region at the input's largest possible region
    if (region.Crop(input->GetLargestPossibleRegion()))
    {
        input->SetRequestedRegion(region);
    }
    else
    {
        // Couldn't crop the region; throw an exception.
        input->SetRequestedRegion(region);

        itk::InvalidRequestedRegionError e(__FILE__, __LINE__);
        itk::OStringStream msg;
        msg << static_cast<const char *>(this->GetNameOfClass())
            << "::GenerateInputRequestedRegion()";
        e.SetLocation(msg.str().c_str());
        e.SetDescription("Requested region is (at least partially) outside the largest possible region.");
        e.SetDataObject(input);
        throw e;
    }
}

template < class TInput, class TOutput >
void LocalContrastImageFilter< TInput, TOutput >
::GenerateData()
{
    // Logger::verbose << "LocalContrastImageFilter::GenerateData(): starting." << std::endl;
    // Local typedefs
    typedef itk::ConstNeighborhoodIterator< InputImageType > InputIteratorType;
    typedef itk::ImageRegionIterator< OutputImageType > OutputIteratorType;

    // Allocate output
    Superclass::AllocateOutputs();
    
    // Get iteration regions
    OutputPointerType output = this->GetOutput();
    typename OutputImageType::RegionType outRegion = output->GetRequestedRegion();
    InputPointerType input = const_cast<InputImageType *>(this->GetInput());
    typename InputImageType::RegionType inRegion;
    CopyRegion<OutputImageType, InputImageType>(outRegion, inRegion);
    
    // Set up input neighborhood for the filter
    typename InputIteratorType::RadiusType radius;
    radius.Fill(0);
    radius[this->GetDirection()] = 1;
    
    // Set up offsets into the input neighborhood
    typename InputIteratorType::OffsetType pos;
    typename InputIteratorType::OffsetType neg;
    pos.Fill(0);
    neg.Fill(0);
    pos[this->GetDirection()] = 1;
    neg[this->GetDirection()] = -1;
    
    InputIteratorType inIt(radius, input, inRegion);
    OutputIteratorType outIt(output, outRegion);
    
    // Logger::verbose << "LocalContrastImageFilter::GenerateData(): computing local contrast." << std::endl;
    // Compute local contrast metric at each image location
    for (inIt.GoToBegin(), outIt.GoToBegin(); 
        !(inIt.IsAtEnd() || outIt.IsAtEnd());
        ++inIt, ++outIt)
    {
        // TODO: Decide between these two ways of calculating contrast
        // Difference is how to estimate the value of the center pixel....seems
        // the actual value would be the best estimate of that.        
        InputPixelType value(2 * inIt.GetCenterPixel());
        // InputPixelType value(inIt.GetPixel(neg) + inIt.GetPixel(pos));
        
        // Avoid division by zero.
        value = value == 0 ? 2 : value;
        outIt.Set(static_cast<OutputPixelType>(
            (inIt.GetPixel(pos) - inIt.GetPixel(neg)) / value));
    }
    
    // Graft output
    this->GraftOutput(output);
    // Logger::verbose << "LocalContrastImageFilter::GenerateData(): finished." << std::endl;
}
