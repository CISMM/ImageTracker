#pragma once

#include "itkImage.h"
#include "itkImageToImageFilter.h"

#include "CommonTypes.h"
#include "StructureTensorImageFilter.h"

/**
 * \class GaussSeidelIterativeStepImageFilter
 * \brief Implements one step in the Gauss-Seidel iterative optical flow computation.
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
class GaussSeidelIterativeStepImageFilter :
        public itk::ImageToImageFilter< TInputImage, TOutputImage >
{
public:
    // Helpful typedefs
    typedef TInputImage InputImageType;
    typedef TOutputImage OutputImageType;
    typedef StructureTensorImageFilter< CommonTypes::InternalImageType > TensorFilterType;
    typedef typename TensorFilterType::TensorImageType TensorImageType;
    typedef CommonTypes::InternalImageType DenominatorImageType;
    
    typedef typename OutputImageType::RegionType OutputRegionType;
    
    // Standard itk typedefs
    typedef GaussSeidelIterativeStepImageFilter Self;
    typedef itk::ImageToImageFilter< TInputImage, TOutputImage > Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    
    itkNewMacro(Self);
    itkTypeMacro(GaussSeidelIterativeStepImageFilter, itk::ImageToImageFilter);
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
     * Pads the requested region by one pixel to enable computing new pixel values at every point.
     */
    virtual void GenerateInputRequestedRegion() throw (itk::InvalidRequestedRegionError);
    
protected:
    GaussSeidelIterativeStepImageFilter();
    virtual ~GaussSeidelIterativeStepImageFilter();
    
    /**
     * Computes the denominator term in the Gauss-Seidel computation.  This depends on the regularization 
     * and the structure image tensor.  Specifically: denom = (r^2 + Txx + Tyy)
     */
    void ComputeDenominator();

    /**
     * Generates new flow estimates within the requested output region.
     */
    void ThreadedGenerateData(const OutputRegionType& outputRegion, int threadId);
    
private:
    // Not implemented 
    GaussSeidelIterativeStepImageFilter(const Self& other);
    void operator=(const Self& other);
    
    typename TensorImageType::ConstPointer m_StructureTensor;
    typename DenominatorImageType::Pointer m_Denominator;
    double m_Regularization;
};

//-------------------------------------
// Implementation
//-------------------------------------

#include "itkConstNeighborhoodIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"

#include "Logger.h"

template < class TInputImage, class TOutputImage >
GaussSeidelIterativeStepImageFilter<TInputImage, TOutputImage>
::GaussSeidelIterativeStepImageFilter() :
        m_Regularization(100)
{}

template < class TInputImage, class TOutputImage >
GaussSeidelIterativeStepImageFilter<TInputImage, TOutputImage>
::~GaussSeidelIterativeStepImageFilter()
{}

template < class TInputImage, class TOutputImage >
void GaussSeidelIterativeStepImageFilter< TInputImage, TOutputImage >
::ComputeDenominator()
{
    std::string function("GaussSeidelIterativeStepImageFilter::ComputeDenominator");
    // Get pointer to the input image:
    typename InputImageType::Pointer input = const_cast<InputImageType*>(this->GetInput());

    // Make sure we can procede
    if (!input)
    {
        Logger::warning << function << ": input image was not set!" << std::endl;
        return;
    }

    // Get a copy of the input requested region
    typename InputImageType::RegionType region = input->GetLargestPossibleRegion();

    // Create the denominator image
    this->m_Denominator = DenominatorImageType::New();
    this->m_Denominator->SetRegions(region);
    this->m_Denominator->SetSpacing(input->GetSpacing());
    this->m_Denominator->SetOrigin(input->GetOrigin());
    this->m_Denominator->Allocate();

    // Setup iterators for the structure tensor and denominator image
    typedef itk::ImageRegionConstIterator<TensorImageType> TensorIteratorType;
    typedef itk::ImageRegionIterator< DenominatorImageType > DenominatorIteratorType;
    TensorIteratorType tensorIt(this->m_StructureTensor, region);
    DenominatorIteratorType denomIt(this->m_Denominator, region);
    typename TensorImageType::PixelType J;

    // Compute the denominator image
    double regSquared = this->GetRegularization()*this->GetRegularization();
    for (tensorIt.GoToBegin(), denomIt.GoToBegin();
        !(tensorIt.IsAtEnd() || denomIt.IsAtEnd());
        ++tensorIt, ++denomIt)
    {
        J = tensorIt.Get();
        denomIt.Set(regSquared + J[TensorFilterType::T11] + J[TensorFilterType::T22]);
    } 
}

template < class TInputImage, class TOutputImage >
void GaussSeidelIterativeStepImageFilter< TInputImage, TOutputImage >
::GenerateInputRequestedRegion() throw (itk::InvalidRequestedRegionError)
{
    std::string function("GaussSeidelIterativeStepImageFilter::GenerateInputRequestedRegion");
    
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
void GaussSeidelIterativeStepImageFilter< TInputImage, TOutputImage >
::ThreadedGenerateData(const OutputRegionType& outputRegion, int threadId)
{
    std::string function("GaussSiedelIterativeStepImageFilter::ThreadedGenerateData");
    
    typedef itk::ConstNeighborhoodIterator<InputImageType> InputIteratorType;
    typedef itk::ImageRegionIterator<OutputImageType> OutputIteratorType;
    typedef itk::ImageRegionConstIterator<TensorImageType> TensorIteratorType;
    
    // Set up input iterator (current flow field)
    typename InputIteratorType::RadiusType radius;
    radius.Fill(1);
    InputIteratorType inIt(radius, this->GetInput(), outputRegion);
    
    // Name the neighborhood positions
    typename InputIteratorType::OffsetType NW   = {{-1, -1}};
    typename InputIteratorType::OffsetType N    = {{0, -1}};
    typename InputIteratorType::OffsetType NE   = {{+1, -1}};
    typename InputIteratorType::OffsetType W    = {{-1, 0}};
    typename InputIteratorType::OffsetType E    = {{+1, 0}};
    typename InputIteratorType::OffsetType SW   = {{-1, +1}};
    typename InputIteratorType::OffsetType S    = {{0, +1}};
    typename InputIteratorType::OffsetType SE   = {{+1, +1}};
        
    // Set up output iterator (next flow field)
    OutputIteratorType outIt(this->GetOutput(), outputRegion);
    
    // Set up tensor and denominator iterators
    TensorIteratorType jIt(this->GetStructureTensor(), outputRegion);

    // scalar constants
    double sixth = 1.0 / 6.0;
    double twelfth = 1.0 / 12.0;
    double regSquared = this->GetRegularization()*this->GetRegularization();

    // Compute next flow
    typename InputImageType::PixelType curr, next, laplace;
    typename TensorImageType::PixelType J;
    double denom;

    for (inIt.GoToBegin(), outIt.GoToBegin(), jIt.GoToBegin();
         !(inIt.IsAtEnd() || outIt.IsAtEnd() || jIt.IsAtEnd());
         ++inIt, ++outIt, ++jIt)
    {
        laplace[0] = 
            sixth   * (inIt.GetPixel(N)[0] + inIt.GetPixel(E)[0] + inIt.GetPixel(S)[0] + inIt.GetPixel(W)[0]) +
            twelfth * (inIt.GetPixel(NW)[0] + inIt.GetPixel(NE)[0] + inIt.GetPixel(SW)[0] + inIt.GetPixel(SE)[0]);
        laplace[1] =
            sixth   * (inIt.GetPixel(N)[1] + inIt.GetPixel(E)[1] + inIt.GetPixel(S)[1] + inIt.GetPixel(W)[1]) +
            twelfth * (inIt.GetPixel(NW)[1] + inIt.GetPixel(NE)[1] + inIt.GetPixel(SW)[1] + inIt.GetPixel(SE)[1]);

        J = jIt.Get();
        curr = inIt.GetCenterPixel();

        denom = regSquared + J[TensorFilterType::T11] + J[TensorFilterType::T22];

        next[0] = laplace[0] - 
            (J[TensorFilterType::T11]*laplace[0] + 
             J[TensorFilterType::T12]*laplace[1] + 
             J[TensorFilterType::T13]) / denom;
        next[1] = laplace[1] - 
            (J[TensorFilterType::T21]*laplace[0] + 
             J[TensorFilterType::T22]*laplace[1] + 
             J[TensorFilterType::T23]) / denom;
        
        outIt.Set(next);
    }
}
