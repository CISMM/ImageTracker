#pragma once

#include "itkImage.h"
#include "itkImageToImageFilter.h"

/**
 * \class HarrisFeatureInterestImageFilter
 * \brief Computes a Harris feature detector interest image from an input image.
 *
 * This class implements the improved Harris feature interest image described
 * in Schmid et al. "Evaluation of Interest Point Detectors", IJCV 2000. The goal
 * of the Harris feature detector is to find image features that have large
 * image gradients in two directions.
 *
 * Given an input image, the spatial derivatives, Ix, Iy, are computed with a 
 * spatial derivative scale. The weighted auto-correlation matrix, A, is compted 
 * as:
 * A = [ Sum_w (Ix)^2  Sum_w (Ix Iy); Sum_w (Ix Iy) Sum_w (Iy)^2 ],
 * where Sum_w specifies the Gaussian-weighted sum over an image window. Applying
 * this weighting is equivalent to filtering the images Ix and Iy with a Gaussian
 * of the appropriate scale for the window. The Harris feature interest weight
 * at each image location is then given by det(A) - a Trace(A)^2, where a is
 * a scalar weighting chosen to surpress matrices A with only one strong eigenvalue.
 */
template < class TInputImage, class TOutputImage >
class HarrisFeatureInterestImageFilter
    : public itk::ImageToImageFilter< TInputImage, TOutputImage >
{
public:
    // Helpful typedefs
    typedef TInputImage InputImageType;
    typedef typename InputImageType::PixelType InputPixelType;
    typedef typename InputImageType::Pointer InputPointerType;
    typedef TOutputImage OutputImageType;
    typedef typename OutputImageType::PixelType OutputPixelType;
    typedef typename OutputImageType::Pointer OutputPointerType;
    
    // Common itk typedefs
    typedef HarrisFeatureInterestImageFilter Self;
    typedef itk::ImageToImageFilter< InputImageType, OutputImageType > Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    itkNewMacro(Self);
    itkTypeMacro(HarrisFeatureInterestImageFilter, itk::ImageToImageFilter);
    
    itkStaticConstMacro(ImageDimension, unsigned int, InputImageType::ImageDimension);
    
    itkGetMacro(DerivativeSigma, float);
    itkSetMacro(DerivativeSigma, float);
    
    itkGetMacro(IntegrationSigma, float);
    itkSetMacro(IntegrationSigma, float);
    
    itkGetMacro(TraceWeight, float);
    itkSetMacro(TraceWeight, float);
    
protected:
    HarrisFeatureInterestImageFilter()
    : m_DerivativeSigma(1.0),
      m_IntegrationSigma(4.0),
      m_TraceWeight(0.06)
    {
    }
    
    virtual ~HarrisFeatureInterestImageFilter() {}
    
    void GenerateData();
    
private:
    // not implemented
    HarrisFeatureInterestImageFilter(const Self& other);
    void operator=(const Self& other);
    
    float m_DerivativeSigma;
    float m_IntegrationSigma;
    float m_TraceWeight;
};

/*----- Implementation -----*/

#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "itkMultiplyImageFilter.h"
#include "itkRecursiveGaussianImageFilter.h"

#include "ImageUtils.h"
#include "Logger.h"

template < class TInputImage, class TOutputImage >
void HarrisFeatureInterestImageFilter<TInputImage, TOutputImage>
::GenerateData()
{
    std::string function("HarrisFeatureInterstImageFilter::GenerateData");
    
    Logger::debug << function << ": Allocating output buffer" << std::endl;;
    Superclass::AllocateOutputs();
    OutputPointerType output = this->GetOutput();
    typename OutputImageType::RegionType outRegion = output->GetRequestedRegion();
    
    Logger::debug << function << ": Checking input" << std::endl;
    InputPointerType input = const_cast<InputImageType*>(this->GetInput());
    if (!input)
    {
        Logger::warning << function << ": Input image not set, aborting!" << std::endl;
        return;
    }
    
    // Typedefs for filters we'll be using
    typedef itk::RecursiveGaussianImageFilter< InputImageType, InputImageType > GaussianType;
    typedef itk::MultiplyImageFilter< InputImageType, InputImageType, InputImageType > MultiplyType;
    typedef typename GaussianType::Pointer GaussianPointer;
    typedef itk::ImageRegionConstIterator< InputImageType > InputIterator;
    typedef itk::ImageRegionIterator< OutputImageType > OutputIterator;
    typename MultiplyType::Pointer mult = MultiplyType::New();
    
    Logger::debug << function << ": Setting up spatial derivative pipeline" << std::endl;
    GaussianPointer dx = GaussianType::New(); // for spatial derivatives
    GaussianPointer dy = GaussianType::New(); // for spatial derivatives
    GaussianPointer gx = GaussianType::New(); // for window weighting
    GaussianPointer gy = GaussianType::New(); // for window weighting
    
    // Set parameters
    dx->SetDirection(0);
    dx->SetSigma(this->GetDerivativeSigma());
    dy->SetDirection(1);
    dy->SetSigma(this->GetDerivativeSigma());
    
    dx->SetInput(input);
    dy->SetInput(dx->GetOutput());
    
    Logger::debug << function << ": Computing spatial derivatives" << std::endl;
    // Compute Ix
    dx->SetFirstOrder();
    dy->SetZeroOrder();
    InputPointerType Ix = CopyImage(dy->GetOutput());

    typedef itk::Image< unsigned short, 2 > WriteImageType;
//     WriteImage< InputImageType, WriteImageType >(dx->GetOutput(), "dx.tiff", true);
//     WriteImage< InputImageType, WriteImageType >(dy->GetOutput(), "dxsy.tiff", true);
    
    // Compute Iy
    dx->SetZeroOrder();
    dy->SetFirstOrder();
    InputPointerType Iy = CopyImage(dy->GetOutput());
    
//     WriteImage< InputImageType, WriteImageType >(dx->GetOutput(), "sx.tiff", true);
//     WriteImage< InputImageType, WriteImageType >(dy->GetOutput(), "sxdy.tiff", true);
    
    // Set up windowing smoothing
    gx->SetDirection(0);
    gx->SetZeroOrder();
    gx->SetSigma(this->GetIntegrationSigma());
    gy->SetDirection(1);
    gy->SetZeroOrder();
    gy->SetSigma(this->GetIntegrationSigma());
    
    gx->SetInput(mult->GetOutput());
    gy->SetInput(gx->GetOutput());
        
    Logger::debug << function << ": Compute auto-correlation matrix members" << std::endl;
    mult->SetInput1(Ix);
    mult->SetInput2(Ix);
    InputPointerType IxIx = CopyImage(gy->GetOutput());

    mult->SetInput2(Iy);
    InputPointerType IxIy = CopyImage(gy->GetOutput());
    
    mult->SetInput1(Iy);
    InputPointerType IyIy = CopyImage(gy->GetOutput());
    
    PrintImageInfo(input.GetPointer(), "input");
    PrintImageInfo(Ix.GetPointer(), "Ix");
    PrintImageInfo(Iy.GetPointer(), "Iy");
    PrintImageInfo(IxIx.GetPointer(), "IxIx");
    PrintImageInfo(IxIy.GetPointer(), "IxIy");
    PrintImageInfo(IyIy.GetPointer(), "IyIy");
    
    Logger::debug << function << ": Computing Harris feature interest image" << std::endl;
    InputIterator itIxIx(IxIx, outRegion);
    InputIterator itIyIy(IyIy, outRegion);
    InputIterator itIxIy(IxIy, outRegion);
    OutputIterator outIt(output, outRegion);
    
    for (itIxIx.GoToBegin(), itIyIy.GoToBegin(), itIxIy.GoToBegin(), outIt.GoToBegin();
         !(itIxIx.IsAtEnd() || itIyIy.IsAtEnd() || itIxIy.IsAtEnd() || outIt.IsAtEnd());
         ++itIxIx, ++itIyIy, ++ itIxIy, ++outIt)
    {
        // det(A) - a Trace(A)^2
        outIt.Set(static_cast<OutputPixelType>(
            (itIxIx.Get() * itIyIy.Get() - itIxIy.Get() * itIxIy.Get()) -       // determinant
            this->GetTraceWeight() * 
            ((itIxIx.Get() + itIyIy.Get()) * (itIxIx.Get() + itIyIy.Get()))));   // trace^2
    }
    
    Logger::debug << function << ": Grafting output" << std::endl;
    this->GraftOutput(output);
}
