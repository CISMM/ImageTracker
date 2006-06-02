#pragma once

#include "itkGaussianSpatialFunction.h"
#include "itkImageToImageFilter.h"

/**
 * /class GaussianFunctionImageFilter
 * Computes the value of a Gaussian function over the intensity of each pixel in an
 * image. The result is a Gaussian function image. Note this is not a Gaussian smoothing
 * filter. The value of each output pixel is computed from the intensity value of the
 * input pixel only.
 */
template < class TInputImage, class TOutputImage >
class GaussianFunctionImageFilter:
    public itk::ImageToImageFilter< TInputImage, TOutputImage >
{
public:
    // Standard itk typedefs
    typedef GaussianFunctionImageFilter Self;
    typedef itk::ImageToImageFilter< TInputImage, TOutputImage > Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;

    itkNewMacro(Self);
    itkTypeMacro(GaussianFunctionImageFilter, ImageToImageFilter);
    itkStaticConstMacro(ImageDimension, unsigned int, TInputImage::ImageDimension);

    // Handy typedefs
    typedef TInputImage InputImageType;
    typedef TOutputImage OutputImageType;
    typedef typename InputImageType::PixelType InputPixelType;
    typedef typename OutputImageType::PixelType OutputPixelType;

    // the Gaussian function is defined over the range of intensity values,
    // and so has dimension 1.
    const static unsigned int FunctionDimension = 1;
    typedef itk::GaussianSpatialFunction< OutputPixelType, FunctionDimension > FunctionType;
    typedef typename FunctionType::InputType DomainType;

    /** Set the input image. */
    void SetInput(const TInputImage* input)
    {
        this->SetNthInput(0, const_cast<TInputImage*> (input));
    }

    /** Get the mean used by the Gaussian function. */
    double GetMean() { return this->m_Function->GetMean()[0]; }
    /** Set the mean used by the Gaussian function. */
    void SetMean(double mean) 
    {
        typename FunctionType::ArrayType means;
        means.Fill(mean);
        this->m_Function->SetMean(means);
    }

    /** Get the standard deviation of the Gaussian function. */
    double GetSigma() { return this->m_Function->GetSigma()[0]; }
    /** Set the standard deviation of the Gaussian function. */
    void SetSigma(double sigma)
    {
        typename FunctionType::ArrayType sigmas;
        sigmas.Fill(sigma);
        this->m_Function->SetSigma(sigmas);
    }

protected:
    GaussianFunctionImageFilter()
    {
        this->m_Function = FunctionType::New();

        // Default gaussian parameters
        this->SetMean(0.0);
        this->SetSigma(10.0);
        this->m_Function->SetNormalized(true);
    }
    virtual ~GaussianFunctionImageFilter(){}

    /** Compute the value of the Gaussian function using the input 
        image intensity at each pixel location. */
    void GenerateData();

private:
    GaussianFunctionImageFilter(const Self&); // not implemented
    void operator=(const Self&); // not implemented

    typename FunctionType::Pointer m_Function;
};

/** Implementation **/

#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"

template < class TInputImage, class TOutputImage >
void GaussianFunctionImageFilter< TInputImage, TOutputImage >
::GenerateData()
{
    // Allocate output data
    Superclass::AllocateOutputs();
    typename OutputImageType::Pointer output = this->GetOutput();
    typename OutputImageType::RegionType regionOut = output->GetRequestedRegion();

    // Copy output region data to input region
    typename InputImageType::RegionType regionIn;
    typename InputImageType::RegionType::SizeType size;
    typename InputImageType::RegionType::IndexType index;

    for (unsigned int d = 0; d < ImageDimension; d++)
    {
        size[d] = regionOut.GetSize()[d];
        index[d] = regionOut.GetIndex()[d];
    }
    regionIn.SetSize(size);
    regionIn.SetIndex(index);

    typedef itk::ImageRegionConstIterator< InputImageType > InputIterator;
    typedef itk::ImageRegionIterator< OutputImageType > OutputIterator;

    InputIterator itIn(this->GetInput(), regionIn);
    OutputIterator itOut(output, regionOut);

    DomainType mean;
    mean.Fill(this->GetMean());
    OutputPixelType norm = this->m_Function->Evaluate(mean);

    for (itIn.Begin(), itOut.Begin(); 
        !(itIn.IsAtEnd() || itOut.IsAtEnd()); 
        ++itIn, ++itOut)
    {
        DomainType value;
        value.Fill(itIn.Get());

        if (value[0] <= 0) // value is less than zero...regular attenuation
            itOut.Set(this->m_Function->Evaluate(value) / norm);
        else          // value is greater than zero...negative attenuation
            itOut.Set((2 * norm - this->m_Function->Evaluate(value)) / norm);
    }

    // Graft output result
    this->GraftOutput(output);
}
