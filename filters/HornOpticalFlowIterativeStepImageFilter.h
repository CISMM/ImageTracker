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
 * corrupting the result.  The value at one location in an iteration depends on the neighboring 
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
    typedef typename InputImageType::Pointer InputImagePointer;
    typedef typename InputImageType::ConstPointer ConstInputImagePointer;
    typedef TOutputImage OutputImageType;
    typedef typename OutputImageType::PixelType OutputPixelType;
    typedef typename OutputImageType::RegionType OutputRegionType;
    typedef TDerivativeImage DerivativeImageType;
    typedef typename DerivativeImageType::Pointer DerivativeImagePointer;
    typedef typename DerivativeImageType::ConstPointer ConstDerivativeImagePointer;
    
    itkStaticConstMacro(ImageDimension, unsigned int, InputImageType::ImageDimension);
    
    // Common ITK typedefs
    typedef HornOpticalFlowIterativeStepImageFilter Self;
    typedef itk::ImageToImageFilter< TInputImage, TOutputImage > Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    
    itkNewMacro(Self);
    itkTypeMacro(HornOpticalFlowIterativeStepImageFilter, itk::ImageToImageFilter);
    
    itkGetConstObjectMacro(DerivativeX, DerivativeImageType);
    itkSetConstObjectMacro(DerivativeX, DerivativeImageType);
    itkGetConstObjectMacro(DerivativeY, DerivativeImageType);
    itkSetConstObjectMacro(DerivativeY, DerivativeImageType);
    itkGetConstObjectMacro(DerivativeT, DerivativeImageType);
    itkSetConstObjectMacro(DerivativeT, DerivativeImageType);
    
    /** 
     * Gets/Sets the weighting, alpha, to apply to the smoothing term of the energy function.
     * Larger values increase the importance of a smooth flow field and generate more uniform 
     * flow.  This term gets squared.
     */    
    itkGetMacro(SmoothWeighting, double);
    itkSetMacro(SmoothWeighting, double);
    
    /** 
     * Get/Set an initial flow field, used only for the smoothing term of the energy
     * minimization.
     */
    itkGetConstObjectMacro(InitialFlow, InputImageType);
    itkSetConstObjectMacro(InitialFlow, InputImageType);
    
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

    double m_SmoothWeighting;
    
    ConstDerivativeImagePointer m_DerivativeX;
    ConstDerivativeImagePointer m_DerivativeY;
    ConstDerivativeImagePointer m_DerivativeT;
    ConstInputImagePointer m_InitialFlow;
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
    ConstDerivativeImagePointer dx = this->GetDerivativeX();
    ConstDerivativeImagePointer dy = this->GetDerivativeY();
    ConstDerivativeImagePointer dt = this->GetDerivativeT();
    
    // Iterator typedefs
    typedef itk::ConstNeighborhoodIterator< InputImageType > InputIteratorType;
    typedef itk::ImageRegionConstIterator< DerivativeImageType > DerivIteratorType;
    typedef itk::ImageRegionIterator< OutputImageType > OutputIteratorType;
    
    // Iterate over regions that include the 8 neighbors of each image pixel
    typename InputImageType::SizeType radius;
    radius.Fill(1);
    InputIteratorType flowIt(radius, input, outputRegion);
    InputIteratorType initIt(radius, this->GetInitialFlow(), outputRegion);
    
    // Iterators over the derivatives and output
    DerivIteratorType dxIt(dx, outputRegion);
    DerivIteratorType dyIt(dy, outputRegion);
    DerivIteratorType dtIt(dt, outputRegion);
    OutputIteratorType outIt(output, outputRegion);
    
    double weight = this->GetSmoothWeighting() * this->GetSmoothWeighting();
    double one6th = 1.0 / 6.0;
    double one12th = 1.0 / 12.0;
    
    // Iterate over all iterators
    for (flowIt.GoToBegin(), initIt.GoToBegin(), dxIt.GoToBegin(), dyIt.GoToBegin(), dtIt.GoToBegin(), outIt.GoToBegin(); 
         !(flowIt.IsAtEnd() || initIt.IsAtEnd() || dxIt.IsAtEnd() || dyIt.IsAtEnd() || dtIt.IsAtEnd() || outIt.IsAtEnd()); 
         ++flowIt, ++initIt, ++dxIt, ++dyIt, ++dtIt, ++outIt)
    {
        InputPixelType curr(flowIt.GetCenterPixel());
        // Logger::verbose << "[" << curr[0] << "," << curr[1] << "]";
        OutputPixelType laplace, next;
        
        for (int d = 0; d < curr.GetNumberOfComponents(); d++)
        {
            // The weighting here is like a Laplacian with the center taken out.
            // (smoothing term)
            laplace[d] = one6th * (flowIt.GetPixel(1)[d] + initIt.GetPixel(1)[d] + 
                    flowIt.GetPixel(3)[d] + initIt.GetPixel(3)[d] +
                    flowIt.GetPixel(5)[d] + initIt.GetPixel(5)[d] +
                    flowIt.GetPixel(7)[d] + initIt.GetPixel(7)[d]) +
                    one12th * (flowIt.GetPixel(0)[d] + initIt.GetPixel(0)[d] +
                    flowIt.GetPixel(2)[d] + initIt.GetPixel(2)[d] +
                    flowIt.GetPixel(6)[d] + initIt.GetPixel(6)[d] +
                    flowIt.GetPixel(8)[d] + initIt.GetPixel(8)[d]);

        }
        // Logger::verbose << " + [" << mean[0] << "," << mean[1] << "]";
        // Compute the next flow value (data term); note that we subtract out the value from the initial flow
        // field...this is because we may be calculating the u,v incrementally
        next[0] = laplace[0] - dxIt.Get() * (dxIt.Get() * laplace[0] + dyIt.Get() * laplace[1] + dtIt.Get()) /
                (weight + dxIt.Get() * dxIt.Get() + dyIt.Get() * dyIt.Get()) - initIt.GetCenterPixel()[0];
        next[1] = laplace[1] - dyIt.Get() * (dxIt.Get() * laplace[0] + dyIt.Get() * laplace[1] + dtIt.Get()) /
                (weight + dxIt.Get() * dxIt.Get() + dyIt.Get() * dyIt.Get()) - initIt.GetCenterPixel()[1];
        
        // Set the next flow value
        // Logger::verbose << " => [" << next[0] << "," << next[1] << "]" << std::endl;;
        outIt.Set(next);
    }
}

