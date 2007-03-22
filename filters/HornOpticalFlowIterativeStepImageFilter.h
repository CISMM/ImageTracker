#pragma once

#include "itkImage.h"
#include "itkImageToImageFilter.h"
#include "itkVector.h"

/**
 * \class HornOpticalFlowIterativeStepImageFilter
 * \brief Implements one step in the Horn and Schunck optical flow energy minimization algorithm.
 *
 * Computes the next value of the flow field, (u,v) for each image location, given the previous
 * flow estimate and the image derivatives (dI/dx, dI/dy, dI/dt).
 *
 * This filter uses threaded execution; the calling filter should not.  Otherwise, the iteration
 * in one portion of the image could get ahead of the iterations in another part of the image,
 * corrupting the result.  The value at on location in an iteration depend on the neighboring 
 * values from the previous iteration.
 */
template < class TInputImage, class TDerivativeImage, class TOutputImage = TInputImage >
class HornOpticalFlowIterativeStepImageFilter :
    public itk::ImageToImageFilter< TInputImage, TOutputImage >
{
public:
    // Convenient typedefs
    typedef TInputImage InputImageType;
    typedef typename InputImageType::PixelType InputPixelType;
    typedef typename InputImageType::RegionType InputRegionType;
    typedef TOutputImage OutputImageType;
    typedef typename OutputImageType::PixelType OutputPixelType;
    typedef typename OutputImageType::RegionType OutputRegionType;
    typedef TDerivativeImage DerivativeImageType;
    
    itkStaticConstMacro(ImageDimension, unsigned int, InputImageType::ImageDimension);
    
    // Common ITK typedefs
    typedef HornOpticalFlowIterativeStepImageFilter Self;
    typedef itk::ImageToImageFilter< TInputImage, TOutputImage > Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    
    itkNewMacro(Self);
    itkTypeMacro(HornOpticalFlowIterativeStepImageFilter, itk::ImageToImageFilter);
    
    void SetDerivativeX(const DerivativeImageType* image);
    void SetDerivativeY(const DerivativeImageType* image);
    void SetDerivativeT(const DerivativeImageType* image);
    
    DerivativeImageType* GetDerivativeX();
    DerivativeImageType* GetDerivativeY();
    DerivativeImageType* GetDerivativeT();
    
    itkGetMacro(SmoothWeighting, double);
    itkSetMacro(SmoothWeighting, double);
    
    /**
     * Pads the requested region by one pixel to enable computing new pixel values at every point.
     */
    virtual void GenerateInputRequestedRegion() throw (itk::InvalidRequestedRegionError);

protected:
    HornOpticalFlowIterativeStepImageFilter()
        : m_SmoothWeighting(100)
    {}
    virtual ~HornOpticalFlowIterativeStepImageFilter() {}
    
    /**
     * Generates new flow estimates within the requested output region.
     */
    void ThreadedGenerateData(const OutputRegionType& outputRegion, int threadId);
    
private:
    // Not implemented
    HornOpticalFlowIterativeStepImageFilter(const Self& other);
    void operator=(const Self& other);
    
    /** 
     * The weighting, alpha, to apply to the smoothing term of the energy function.
     * Larger values increase the importance of a smooth flow field and generate more
     * uniform flow.  This term gets squared.
     */
    double m_SmoothWeighting;
    DerivativeImageType* m_dx;
    DerivativeImageType* m_dy;
    DerivativeImageType* m_dt;
};

/**----- Implementation -----**/

#include "itkConstNeighborhoodIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "Logger.h"

template < class TInputImage, class TDerivativeImage, class TOutputImage >
    void HornOpticalFlowIterativeStepImageFilter< TInputImage, TDerivativeImage, TOutputImage >
    ::GenerateInputRequestedRegion() throw (itk::InvalidRequestedRegionError)
{
    std::string function("HornOpticalFlowIterativeStepImageFilter::GenerateInputRequestedRegion");
//     Logger::debug << function << std::endl;
    
    // Call Superclass implementation; this will copy the output
    // requested region to the input requested region.
    Superclass::GenerateInputRequestedRegion();

    // Get pointer to the input image:
    typename InputImageType::Pointer input = const_cast<InputImageType*>(this->GetInput());

    // Make sure we can procede
    if (!input)
    {
        Logger::warning << function << ": input image was not set!" << std::endl;
        return;
    }

    // Get a copy of the input requested region
    InputRegionType region = input->GetRequestedRegion();
    
    // Pad by one
    region.PadByRadius(1);

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

template < class TInputImage, class TDerivativeImage, class TOutputImage >
    void HornOpticalFlowIterativeStepImageFilter< TInputImage, TDerivativeImage, TOutputImage >
    ::ThreadedGenerateData(const OutputRegionType& outputRegion, int threadId)
{
    std::string function("HornOpticalFlowIterativeStepImageFilter::ThreadedGenerateData");
//     if (threadId == 0)
//     {
//         Logger::debug << function << "(" << threadId << "): region: " << std::endl;
//         PrintRegionInfo<OutputImageType>(outputRegion);
//     }
    // Get output
    typename OutputImageType::Pointer output = this->GetOutput();
    typename InputImageType::ConstPointer input  = this->GetInput();
    typename DerivativeImageType::Pointer dx = this->GetDerivativeX();
    typename DerivativeImageType::Pointer dy = this->GetDerivativeY();
    typename DerivativeImageType::Pointer dt = this->GetDerivativeT();
    
    // Iterator typedefs
    typedef itk::ConstNeighborhoodIterator< InputImageType > InputIteratorType;
    typedef itk::ImageRegionConstIterator< DerivativeImageType > DerivIteratorType;
    typedef itk::ImageRegionIterator< OutputImageType > OutputIteratorType;
    
    // Iterate over regions that include the 8 neighbors of each image pixel
    typename InputImageType::SizeType radius;
    radius.Fill(1);
    InputIteratorType flowIt(radius, input, outputRegion);
    
    // Iterators over the derivatives and output
    DerivIteratorType dxIt(dx, outputRegion);
    DerivIteratorType dyIt(dy, outputRegion);
    DerivIteratorType dtIt(dt, outputRegion);
    OutputIteratorType outIt(output, outputRegion);
    
    double weight = this->GetSmoothWeighting() * this->GetSmoothWeighting();
    
    // Iterate over all iterators
    for (flowIt.GoToBegin(), dxIt.GoToBegin(), dyIt.GoToBegin(), dtIt.GoToBegin(), outIt.GoToBegin(); 
         !(flowIt.IsAtEnd() || dxIt.IsAtEnd() || dyIt.IsAtEnd() || dtIt.IsAtEnd() || outIt.IsAtEnd()); 
         ++flowIt, ++dxIt, ++dyIt, ++dtIt, ++outIt)
    {
        InputPixelType curr(flowIt.GetCenterPixel());
        // Logger::verbose << "[" << curr[0] << "," << curr[1] << "]";
        OutputPixelType mean;
        OutputPixelType next;
        
        double one6th = 1.0 / 6.0;
        double one12th = 1.0 / 12.0;
        for (int d = 0; d < curr.GetNumberOfComponents(); d++)
        {
            // The weighting here is like a Laplacian with the center taken out.
            mean[d] = 
                    one6th * (flowIt.GetPixel(1)[d] + 
                    flowIt.GetPixel(3)[d] +
                    flowIt.GetPixel(5)[d] +
                    flowIt.GetPixel(7)[d]) +
                    one12th * (flowIt.GetPixel(0)[d] + 
                    flowIt.GetPixel(2)[d] + 
                    flowIt.GetPixel(6)[d] + 
                    flowIt.GetPixel(8)[d]);
        }
        // Logger::verbose << " + [" << mean[0] << "," << mean[1] << "]";
        // Compute the next flow value
        next[0] = mean[0] - dxIt.Get() * (dxIt.Get() * mean[0] + dyIt.Get() * mean[1] + dtIt.Get()) /
                (weight + dxIt.Get() * dxIt.Get() + dyIt.Get() * dyIt.Get());
        next[1] = mean[1] - dyIt.Get() * (dxIt.Get() * mean[0] + dyIt.Get() * mean[1] + dtIt.Get()) /
                (weight + dxIt.Get() * dxIt.Get() + dyIt.Get() * dyIt.Get());
        
        // Set the next flow value
        // Logger::verbose << " => [" << next[0] << "," << next[1] << "]" << std::endl;;
        outIt.Set(next);
    }
}

// Getters and setters.

template < class TInputImage, class TDerivativeImage, class TOutputImage >
    void HornOpticalFlowIterativeStepImageFilter< TInputImage, TDerivativeImage, TOutputImage >
    ::SetDerivativeX(const DerivativeImageType* image)
{
    this->m_dx = const_cast<DerivativeImageType*>(image);
}

template < class TInputImage, class TDerivativeImage, class TOutputImage >
    void HornOpticalFlowIterativeStepImageFilter< TInputImage, TDerivativeImage, TOutputImage >
    ::SetDerivativeY(const DerivativeImageType* image)
{
    this->m_dy = const_cast<DerivativeImageType*>(image);
}

template < class TInputImage, class TDerivativeImage, class TOutputImage >
    void HornOpticalFlowIterativeStepImageFilter< TInputImage, TDerivativeImage, TOutputImage >
    ::SetDerivativeT(const DerivativeImageType* image)
{
    this->m_dt = const_cast<DerivativeImageType*>(image);
}

template < class TInputImage, class TDerivativeImage, class TOutputImage >
    TDerivativeImage* HornOpticalFlowIterativeStepImageFilter< TInputImage, TDerivativeImage, TOutputImage >
    ::GetDerivativeX()
{
    return this->m_dx;
}

template < class TInputImage, class TDerivativeImage, class TOutputImage >
    TDerivativeImage* HornOpticalFlowIterativeStepImageFilter< TInputImage, TDerivativeImage, TOutputImage >
    ::GetDerivativeY()
{
    return this->m_dy;
}

template < class TInputImage, class TDerivativeImage, class TOutputImage >
    TDerivativeImage* HornOpticalFlowIterativeStepImageFilter< TInputImage, TDerivativeImage, TOutputImage >
    ::GetDerivativeT()
{
    return this->m_dt;
}
