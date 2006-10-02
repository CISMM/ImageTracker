#pragma once

#include "itkImageToImageFilter.h"

/**
 * /class CentralDifferenceImageFilter
 * /brief Computes the central difference derivative of an image.
 * 
 * The central difference image filter filters an input image with a 
 * kernel: [ -1/2 0 +1/2 ]. This measures the difference between the 
 * two neigbors of each pixel, and is a quick estimate of the derivative 
 * of an image. The filter is directional, so the kernel is oriented along
 * this filter's direction.
 */
template < class TInputImage, class TOutputImage >
class CentralDifferenceImageFilter :
    public itk::ImageToImageFilter< TInputImage, TOutputImage >
{
public:
    // Helpful types
    typedef TInputImage InputImageType;
    typedef TOutputImage OutputImageType;
    typedef typename InputImageType::Pointer InputPointerType;
    typedef typename OutputImageType::Pointer OutputPointerType;
    // Common ITK typedefs
    typedef CentralDifferenceImageFilter Self;
    typedef itk::ImageToImageFilter< InputImageType, OutputImageType > Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    
    itkNewMacro(Self);
    itkTypeMacro(CentralDifferenceImageFilter, itk::ImageToImageFilter);
    itkStaticConstMacro(ImageDimension, unsigned int, InputImageType::ImageDimension);

    itkGetMacro(Direction, unsigned int);
    itkSetMacro(Direction, unsigned int);
    
protected:
    CentralDifferenceImageFilter() :
        m_Direction(0)
    {}
    
    virtual ~CentralDifferenceImageFilter() {}

    /**
     * The input requested region for this filter should be one pixel wider in each
     * direction along the axis of this filter's Direction.
     */
    void GenerateInputRequestedRegion() throw (itk::InvalidRequestedRegionError);
    
    /**
     * Compute the central difference derivative of the input image.
     */
    void GenerateData();
    
private:
    // purposefully not implemented
    CentralDifferenceImageFilter(const Self& other);
    void operator=(const Self& other);
    
    unsigned int m_Direction;
};

/** Implementation **/

#include "itkConstNeighborhoodIterator.h"
#include "itkImageRegionIterator.h"
#include "itkRegion.h"

#include "ImageUtils.h"
#include "Logger.h"

template < class TInputImage, class TOutputImage >
void CentralDifferenceImageFilter< TInputImage, TOutputImage >
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

template < class TInputImage, class TOutputImage >
void CentralDifferenceImageFilter< TInputImage, TOutputImage >
::GenerateData()
{
    // Logger::verbose << "CentralDifferenceImageFilter::GenerateData(): starting" << std::endl;
    // Local typedefs
    typedef itk::ConstNeighborhoodIterator< InputImageType > InputIteratorType;
    typedef itk::ImageRegionIterator< OutputImageType > OutputIteratorType;
    typedef typename OutputImageType::PixelType OutputPixelType;

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
    
    // Logger::verbose << "CentralDifferenceImageFilter::GenerateData(): computing central difference" << std::endl;
    // Compute local contrast metric at each image location
    for (inIt.GoToBegin(), outIt.GoToBegin(); 
         !(inIt.IsAtEnd() || outIt.IsAtEnd());
         ++inIt, ++outIt)
    {
        // central difference kernel: 1/2 * [ -1 0 1 ]
        outIt.Set(static_cast<OutputPixelType>( 0.5 * (inIt.GetPixel(pos) - inIt.GetPixel(neg)) ));
    }
    
    // Graft output
    this->GraftOutput(output);
    // Logger::verbose << "CentralDifferenceImageFilter::GenerateData(): finished." << std::endl;
}
