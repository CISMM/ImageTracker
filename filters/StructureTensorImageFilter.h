#pragma once

#include "itkImage.h"
#include "itkImageToImageFilter.h"
#include "itkVector.h"

/**
 * \class StructureTensorImageFilter
 * \brief Computes the image structure tensor for a pair of images.
 * The structure tensor is the cross product of the spatio-temporal gradient of an image: 
 * G(I) = [Ix Iy It]^T
 * T = Kp * [G(I)^T x G(I)]
 * Here, ^T denotes transposition, Ks* denotes convolution with a Gaussian of scale p, and x is the vector cross product.
 *
 * There are two scale parameters associated with the structure tensor, the spatial differential scale s, and the tensor
 * integration scale p.  The spatial scale determines this smoothing applied to the gradient computation.  The integration
 * scale determines the size of the window over which tensor terms are averaged (smoothed).
 *
 * Note that in general, the structure tensor T is a 3x3 matrix, but we only compute the 6 components needed for optical
 * flow computations.
 */
template < class TInputImage >
class StructureTensorImageFilter 
    : public itk::ImageToImageFilter< TInputImage, itk::Image< itk::Vector< float, 6 >, TInputImage::ImageDimension > >
{
public:
    // Helpful typedefs and constants
    typedef TInputImage InputImageType;
    
    itkStaticConstMacro(ImageDimension, unsigned int, InputImageType::ImageDimension);
    enum { TensorSize = 6};
    enum TensorIndex 
    {
        T11 = 0,
        T12,
        T13,
        T21,
        T22,
        T23
    };
    
    typedef float TensorComponentType;
    typedef itk::Vector< TensorComponentType, TensorSize > TensorType;
    typedef itk::Image< TensorType, ImageDimension > TensorImageType;
    typedef TensorImageType OutputImageType;
    
    // Standard itk class typedefs
    typedef StructureTensorImageFilter Self;
    typedef itk::ImageToImageFilter< InputImageType, TensorImageType > Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    
    itkNewMacro(Self);
    itkTypeMacro(StructureTensorImageFilter, ImageToImageFilter);
    
    /** 
     * Get/Set the spatial filter deviation. This is the smoothing
     * applied to the input images before differentiation.
     */
    itkGetMacro(SpatialSigma, double);
    itkSetMacro(SpatialSigma, double);
    
    /**
     * Get/Set the integration standard deviation. This sets the size
     * of the data term integration (Lucas-Kanade).
     */
    itkGetMacro(IntegrationSigma, double);
    itkSetMacro(IntegrationSigma, double);
    
    TInputImage* GetInput1()
    {
        return const_cast<TInputImage*>(this->GetInput(0));
    }

    /** 
     * Set the first (moving) image.
     */
    void SetInput1(const TInputImage* image1)
    {
        this->SetNthInput(0, const_cast<TInputImage*>(image1));
    }

    TInputImage* GetInput2()
    {
        return const_cast<TInputImage*>(this->GetInput(1));
    }

    /** 
     * Set the second (fixed) image.
     */
    void SetInput2(const TInputImage* image2)
    {
        this->SetNthInput(1, const_cast<TInputImage*>(image2));
    }
    
protected:
    StructureTensorImageFilter();
    virtual ~StructureTensorImageFilter();
    
    void GenerateData();
    
private:
    // Not implemented
    StructureTensorImageFilter(const Self& other);
    void operator=(const Self& other);
    
    double m_SpatialSigma;
    double m_IntegrationSigma;
};

/************************************************************************/
/* Implementation
/************************************************************************/

#include "itkAdaptImageFilter.h"
#include "itkImageDuplicator.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "itkNthElementPixelAccessor.h"
#include "itkRecursiveGaussianImageFilter.h"
#include "itkSubtractImageFilter.h"
                 
#include "ImageUtils.h"
#include "Logger.h"
#include "NaryJoinImageFilter.h"

template < class TInputImage >
StructureTensorImageFilter< TInputImage >::StructureTensorImageFilter()
    : m_SpatialSigma(1.0),
      m_IntegrationSigma(2.0)
{
}

template < class TInputImage >
StructureTensorImageFilter< TInputImage >::~StructureTensorImageFilter()
{}
                 
template < class TInputImage >
void StructureTensorImageFilter< TInputImage >
::GenerateData()
{
    std::string function("StructureTensorImageFilter::GenerateData");
    Logger::debug << function << ": entering..." << std::endl;
    
    // Allocate the output buffer
    Logger::debug << function << ": allocating output buffer" << std::endl;
    Superclass::AllocateOutputs();
    typename TensorImageType::Pointer output = this->GetOutput();
    typename TensorImageType::RegionType outRegion = output->GetRequestedRegion();

    // Build the spatiotemporal gradient of the input images
    typedef itk::Image<TensorComponentType, ImageDimension> InternalImageType;
    typedef itk::RecursiveGaussianImageFilter<InternalImageType, InternalImageType> GaussianFilterType;
    typedef itk::SubtractImageFilter<InputImageType, InputImageType, InternalImageType> DifferenceFilterType;
    typedef itk::ImageDuplicator<InternalImageType> DuplicatorType;

    Logger::debug << function << ": setting up Gaussian filters" << std::endl;
    // Setup Derivative filters
    typename GaussianFilterType::Pointer gX = GaussianFilterType::New();
    typename GaussianFilterType::Pointer gY = GaussianFilterType::New();
    typename DifferenceFilterType::Pointer dT = DifferenceFilterType::New();
    typename DuplicatorType::Pointer copier = DuplicatorType::New();

    gX->SetDirection(0);
    gY->SetDirection(1);
    
    gX->SetInput(this->GetInput1());
    gY->SetInput(gX->GetOutput());
    
    // Setup filters for spatial derivatives
    Logger::debug << function << ": computing spatial derivatives" << std::endl;
    typename InternalImageType::Pointer Ix, Iy, It;
    gX->SetSigma(this->m_SpatialSigma);
    gY->SetSigma(this->m_SpatialSigma);
    
    // Ix
    gX->SetFirstOrder();
    gY->SetZeroOrder();
    gY->Update();
    copier->SetInputImage(gY->GetOutput());
    copier->Update();
    Ix = copier->GetOutput();
    
    // Iy
    gX->SetZeroOrder();
    gY->SetFirstOrder();
    gY->Update();
    copier->SetInputImage(gY->GetOutput());
    copier->Update();
    Iy = copier->GetOutput();
    
    // It, I2 - I1
    gX->SetZeroOrder();
    gY->SetZeroOrder();
    gY->Update();
    copier->SetInputImage(gY->GetOutput());
    copier->Update();
    dT->SetInput2(copier->GetOutput());
    gX->SetInput(this->GetInput2());
    gY->Update();
    copier->SetInputImage(gY->GetOutput());
    copier->Update();
    dT->SetInput1(copier->GetOutput());
    dT->Update();
    It = dT->GetOutput();
    
    // Compute structure tensor
    Logger::debug << function << ": computing structure tensor" << std::endl;
    typedef itk::ImageRegionIterator<TensorImageType> TensorIteratorType;
    typedef itk::ImageRegionConstIterator<InternalImageType> ImageIteratorType;
    ImageIteratorType itIx(Ix, Ix->GetLargestPossibleRegion());
    ImageIteratorType itIy(Iy, Iy->GetLargestPossibleRegion());
    ImageIteratorType itIt(It, It->GetLargestPossibleRegion());
    TensorIteratorType itTensor(output, outRegion);

    TensorType tensor;
    for (itIx.GoToBegin(), itIy.GoToBegin(), itIt.GoToBegin(), itTensor.GoToBegin();
         !(itIx.IsAtEnd() || itIy.IsAtEnd() || itIt.IsAtEnd() || itTensor.IsAtEnd());
         ++itIx, ++itIy, ++itIt, ++itTensor)
    {
        // First row, x terms
        tensor[T11] = itIx.Get() * itIx.Get();
        tensor[T12] = itIx.Get() * itIy.Get();
        tensor[T13] = itIx.Get() * itIt.Get();
        
        // Second row, y terms
        tensor[T21] = itIy.Get() * itIx.Get();
        tensor[T22] = itIy.Get() * itIy.Get();
        tensor[T23] = itIy.Get() * itIt.Get();
        
        itTensor.Set(tensor);
    }
    // WriteImage< TensorImageType >(output, "tensor-pre-smooth.mha");
    
    // Smooth each structure tensor component independently...must split, smooth, and rejoin.
    Logger::debug << function << ": smoothing structure tensor (integrating)" << std::endl;
    typedef itk::AdaptImageFilter<TensorImageType, InternalImageType, itk::NthElementPixelAccessor<TensorComponentType, TensorType> > SplitType;
    typedef NaryJoinImageFilter< InternalImageType, TensorSize > JoinType;
    
    typename SplitType::Pointer split = SplitType::New();
    typename JoinType::Pointer join = JoinType::New();
    split->SetInput(output);
    gX->SetInput(split->GetOutput());
    gY->SetInput(gX->GetOutput());
    
    gX->SetZeroOrder();
    gY->SetZeroOrder();
    gX->SetSigma(this->m_IntegrationSigma);
    gY->SetSigma(this->m_IntegrationSigma);
    
    for (int t = 0; t < TensorSize; t++)
    {
        split->GetAccessor().SetElementNumber(t);
        split->Modified(); // we have to let the image adapter know we changed the pixel accessor
        gY->Update();
        copier->SetInputImage(gY->GetOutput());
        copier->Update();
        join->SetNthInput(t, const_cast<InternalImageType*>(copier->GetOutput()));
    }
    
    join->Update();
    this->GraftOutput(join->GetOutput());
    // WriteImage< TensorImageType >(join->GetOutput(), "tensor-post-smooth.mha");
    Logger::debug << function << ": done" << std::endl;
}
