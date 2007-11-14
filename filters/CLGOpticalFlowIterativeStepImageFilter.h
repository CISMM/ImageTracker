#pragma once

#include "itkImage.h"
#include "itkImageToImageFilter.h"

#include "CommonTypes.h"
#include "StructureTensorImageFilter.h"

/**
 * \class CLGOpticalFlowIterativeStepImageFilter
 * \brief Implements one step in the Bruhn, Weickert, and Schnorr combined local-global optical flow algorithm.
 *
 * Computes the next value of the flow field, (u,v) for each image location, given the previous
 * flow estimate and the image structure tensor.
 *
 * This filter uses threaded execution; the calling filter should not.  Otherwise, the iteration
 * in one portion of the image could get ahead of the iterations in another part of the image,
 * corrupting the result.  The value at one location in an iteration depends on the neighboring
 * values from the previous iteration.
 */
template < class TInputImage, class TOutputImage = TInputImage >
class CLGOpticalFlowIterativeStepImageFilter :
        public itk::ImageToImageFilter< TInputImage, TOutputImage >
{
public:
    // Helpful typedefs
    typedef TInputImage InputImageType;
    typedef TOutputImage OutputImageType;
    typedef StructureTensorImageFilter< CommonTypes::InternalImageType > TensorFilterType;
    typedef typename TensorFilterType::TensorImageType TensorImageType;
    
    typedef typename OutputImageType::RegionType OutputRegionType;
    
    // Standard itk typedefs
    typedef CLGOpticalFlowIterativeStepImageFilter Self;
    typedef itk::ImageToImageFilter< TInputImage, TOutputImage > Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    
    itkNewMacro(Self);
    itkTypeMacro(CLGOpticalFlowIterativeStepImageFilter, itk::ImageToImageFilter);
    itkStaticConstMacro(ImageDimension, unsigned int, InputImageType::ImageDimension);
    
    /** 
     * Get/Set image structure tensor, used for data term of optical flow computation.
     */
    itkGetConstObjectMacro(StructureTensor, TensorImageType);
    itkSetConstObjectMacro(StructureTensor, TensorImageType);
    
    /**
     * Get/Set the regularization constant. This is the constant
     * weighting of the smoothing term in Horn and Schunck's energy
     * functional.
     */
    itkGetMacro(Regularization, double);
    itkSetMacro(Regularization, double);

    /**
     * Get/Set the relaxation constant (omega) used in the
     * successive over-relaxation (SOR) solution to the combined
     * local-global energy functional.
     */
    itkGetMacro(Relaxation, double);
    itkSetMacro(Relaxation, double);
    
    /**
     * Pads the requested region by one pixel to enable computing new pixel values at every point.
     */
    virtual void GenerateInputRequestedRegion() throw (itk::InvalidRequestedRegionError);
    
protected:
    CLGOpticalFlowIterativeStepImageFilter();
    virtual ~CLGOpticalFlowIterativeStepImageFilter();
    
    /**
     * Generates new flow estimates within the requested output region.
     */
    void ThreadedGenerateData(const OutputRegionType& outputRegion, int threadId);
    
private:
    // Not implemented 
    CLGOpticalFlowIterativeStepImageFilter(const Self& other);
    void operator=(const Self& other);
    
    typename TensorImageType::ConstPointer m_StructureTensor;
    double m_Regularization;
    double m_Relaxation;
};

//-------------------------------------
// Implementation
//-------------------------------------

#include "itkConstNeighborhoodIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"

#include "Logger.h"

template < class TInputImage, class TOutputImage >
CLGOpticalFlowIterativeStepImageFilter<TInputImage, TOutputImage>
::CLGOpticalFlowIterativeStepImageFilter() :
        m_Regularization(100),
        m_Relaxation(1.9)
{}

template < class TInputImage, class TOutputImage >
CLGOpticalFlowIterativeStepImageFilter<TInputImage, TOutputImage>
::~CLGOpticalFlowIterativeStepImageFilter()
{}

template < class TInputImage, class TOutputImage >
void CLGOpticalFlowIterativeStepImageFilter< TInputImage, TOutputImage >
::GenerateInputRequestedRegion() throw (itk::InvalidRequestedRegionError)
{
    std::string function("CLGOpticalFlowIterativeStepImageFilter::GenerateInputRequestedRegion");
    
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
    typename InputImageType::RegionType region = input->GetRequestedRegion();
    
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

template < class TInputImage, class TOutputImage >
void CLGOpticalFlowIterativeStepImageFilter< TInputImage, TOutputImage >
::ThreadedGenerateData(const OutputRegionType& outputRegion, int threadId)
{
    std::string function("CLGOpticalFlowIterativeStepImageFilter::ThreadedGenerateData");
    
    // Compute the scaling factor for the structure tensor.
    // pixel area / regularization
    typename InputImageType::SpacingType spacing = this->GetInput()->GetSpacing();
    double area = 1;
    for (int i = 0; i < ImageDimension; i++)
    {
        area *= spacing[i];
    }
    const double factor = area / this->m_Regularization;
    const double w = this->m_Relaxation;
    
    typedef itk::ConstNeighborhoodIterator<InputImageType> InputIteratorType;
    typedef itk::ImageRegionIterator<OutputImageType> OutputIteratorType;
    typedef itk::ImageRegionConstIterator<TensorImageType> TensorIteratorType;
    
    // Set up input iterator (current flow field)
    typename InputIteratorType::RadiusType radius;
    radius.Fill(1);
    InputIteratorType inIt(radius, this->GetInput(), outputRegion);
    
    typename InputIteratorType::OffsetType left  = {{-1, 0}};
    typename InputIteratorType::OffsetType right = {{+1, 0}};
    typename InputIteratorType::OffsetType above = {{0, -1}};
    typename InputIteratorType::OffsetType below = {{0, +1}};
    const unsigned int nCount = 4; // number of neighbors
    
    // Set up output iterator (next flow field)
    OutputIteratorType outIt(this->GetOutput(), outputRegion);
    
    // Set up tensor iterator
    TensorIteratorType jIt(this->GetStructureTensor(), outputRegion);

    // Compute next flow
    typename InputImageType::PixelType curr, next, sum;
    typename TensorImageType::PixelType J;
    for (inIt.GoToBegin(), outIt.GoToBegin(), jIt.GoToBegin();
         !(inIt.IsAtEnd() || outIt.IsAtEnd() || jIt.IsAtEnd());
         ++inIt, ++outIt, ++jIt)
    {
        J = jIt.Get();
        
        sum[0] = 
            inIt.GetPixel(left)[0] + inIt.GetPixel(right)[0] +
            inIt.GetPixel(above)[0] + inIt.GetPixel(below)[0];
        sum[1] =
            inIt.GetPixel(left)[1] + inIt.GetPixel(right)[1] +
            inIt.GetPixel(above)[1] + inIt.GetPixel(below)[1];

        curr = inIt.GetCenterPixel();
        next[0] = (1.0 - w)*curr[0] + w * (sum[0] - factor*(J[TensorFilterType::T12]*curr[1] + J[TensorFilterType::T13]))/(nCount + factor*J[TensorFilterType::T11]);
        next[1] = (1.0 - w)*curr[1] + w * (sum[1] - factor*(J[TensorFilterType::T21]*next[0] + J[TensorFilterType::T23]))/(nCount + factor*J[TensorFilterType::T22]);
        outIt.Set(next);
    }
}
