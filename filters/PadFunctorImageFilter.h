#pragma once

#include "itkPadImageFilter.h"

/** \class PadFunctorImageFilter
 * \brief Pad an image with a functor that attenuates boundary values to a constant value.
 *
 * PadFunctorImageFilter pads the boundary of an image with pixel values that transition
 * from the image's boundary value to a constant value.  The character of the transition
 * is defined by a functor defined on the window 0..1.  For example, a linear functor, 
 * f(x) = x, transitions from the image values to the constant value linearly over the
 * width of the padded region of the image.
 * 
 * Any functor used as a template parameter for this class should have a default ()
 * operator method defined that accepts input values between 0 and 1 and returns
 * values between 0 and 1.
 */
template <class TInputImage, class TOutputImage, class TFunctor>
class PadFunctorImageFilter:
    public itk::PadImageFilter<TInputImage,TOutputImage>
{
public:
    /** Standard class typedefs. */
    typedef PadFunctorImageFilter Self;
    typedef itk::PadImageFilter<TInputImage, TOutputImage> Superclass;
    typedef itk::SmartPointer<Self> Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;
    
    /** Method for creation through the object factory. */
    itkNewMacro(Self); 
    
    /** Run-time type information (and related methods). */
    itkTypeMacro(PadFunctorImageFilter, PadImageFilter);
    
    typedef TFunctor FunctorType;
    
    /** Image typedefs **/
    typedef TInputImage InputImageType;
    typedef TOutputImage OutputImageType;
    typedef typename InputImageType::ConstPointer InputImagePointer;
    typedef typename OutputImageType::Pointer OutputImagePointer;
    
    /** Typedef to describe the output image region type. */
    typedef typename Superclass::OutputImageRegionType OutputImageRegionType;
    typedef typename Superclass::InputImageRegionType InputImageRegionType;
    
    /** Typedef to describe the type of pixel. */
    typedef typename Superclass::OutputImagePixelType OutputImagePixelType;
    typedef typename Superclass::InputImagePixelType InputImagePixelType;
    
    /** Typedef to describe the output and input image index and size types. */
    typedef typename Superclass::OutputImageIndexType OutputImageIndexType;
    typedef typename Superclass::InputImageIndexType InputImageIndexType;
    typedef typename Superclass::OutputImageSizeType OutputImageSizeType;
    typedef typename Superclass::InputImageSizeType InputImageSizeType;
    
    /** ImageDimension constants */
    itkStaticConstMacro(ImageDimension, unsigned int,
                        TInputImage::ImageDimension);
    itkStaticConstMacro(OutputImageDimension, unsigned int,
                        TOutputImage::ImageDimension);
    
    /** Set/Get the pad value.  Default is Zero. */
    itkSetMacro(BoundaryValue, OutputImagePixelType);
    itkGetMacro(BoundaryValue, OutputImagePixelType);
    
    /**
     * Convenience method for setting the pad radius on
     * a particular dimension.
     */
    void SetPadRadius(int dimension, const unsigned long radius);
  
protected:
    PadFunctorImageFilter();
    virtual ~PadFunctorImageFilter() {}
    
    /**
     * PadFunctorImageFilter is not threaded because in the current implementation
     * we depend on the data in the output image to be generated in a special order.
     */
    void GenerateData();
    
private:
    PadFunctorImageFilter(const Self&);
    void operator=(const Self&);
    
    OutputImagePixelType m_BoundaryValue;
    FunctorType functor;
};

/** Implementation **/
#include <cmath>
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "vnl/vnl_math.h"

#include "ImageUtils.h"
#include "Logger.h"

template< class TInputImage, class TOutputImage, class TFunctor >
PadFunctorImageFilter< TInputImage, TOutputImage, TFunctor >
::PadFunctorImageFilter()
{
    this->m_BoundaryValue = itk::NumericTraits<OutputImagePixelType>::Zero;
}

template< class TInputImage, class TOutputImage, class TFunctor >
void PadFunctorImageFilter< TInputImage, TOutputImage, TFunctor >
::SetPadRadius(int dimension, const unsigned long radius)
{
    unsigned long *plb = const_cast< unsigned long* >(this->GetPadLowerBound());
    unsigned long *pub = const_cast< unsigned long* >(this->GetPadUpperBound());
    plb[dimension] = radius;
    pub[dimension] = radius;
    this->SetPadLowerBound(plb);
    this->SetPadUpperBound(pub);
}

/**
 * We generate the data for the output in several different blocks.  Because we
 * are padding the input image with a functional outside, we have 9 regions to
 * consider, as illustrated here:
 * 
 *  6 | 3 | 7
 * -----------
 *  2 | 0 | 4
 * -----------
 *  5 | 1 | 8
 * 
 * This seemingly random numbering of the image regions makes a bit of sense.
 * Region 0 corresponds to the original image.  Region 1 is where the bottom
 * egde of the image is attenuated to the constant boundary value.  Ditto for
 * 2, 3, 4 for the left, upper, and right edges, respectively.  Region 5 is
 * where data calculated from regions 1 and 2 can be filled in.  Ditto for 6,
 * 7, 8 for their respective neighboring regions.
 */
template< class TInputImage, class TOutputImage, class TFunctor >
void PadFunctorImageFilter< TInputImage, TOutputImage, TFunctor >
::GenerateData()
{
    typedef itk::ImageRegionConstIterator< InputImageType > InputIteratorType;
    typedef itk::ImageRegionIteratorWithIndex< OutputImageType > OutputIteratorType;
    
    // Allocate outputs
    Superclass::AllocateOutputs();
    
    // Grab input and output
    InputImagePointer input = this->GetInput();
    OutputImagePointer output = this->GetOutput();
    
    InputImageRegionType inRegion = input->GetLargestPossibleRegion();
    InputImageSizeType inSize;
    InputImageIndexType inIdx;
    
    OutputImageRegionType outRegion = output->GetLargestPossibleRegion();
    OutputImageSizeType outSize;
    OutputImageIndexType outIdx;
    
    PrintRegionInfo<InputImageType>(inRegion, "Input image region");
    PrintRegionInfo<OutputImageType>(outRegion, "Output image region");
    
    const InputImageSizeType origSize = inRegion.GetSize();
    const OutputImageIndexType outOffset = outRegion.GetIndex();
    const unsigned long* plb = this->GetPadLowerBound();
    const unsigned long* pub = this->GetPadUpperBound();
    
    
    // Copy input image to middle of output region (Region 0)
    outRegion.SetSize(inRegion.GetSize());
    outIdx[0] = outOffset[0] + plb[0];
    outIdx[1] = outOffset[1] + plb[1];
    outRegion.SetIndex(outIdx);
    
    PrintRegionInfo<InputImageType>(inRegion, "Input region 0");
    PrintRegionInfo<OutputImageType>(outRegion, "Output region 0");
    InputIteratorType inIt(input, inRegion);
    OutputIteratorType outIt(output, outRegion);
    for (inIt.GoToBegin(), outIt.GoToBegin();
         !(inIt.IsAtEnd() || outIt.IsAtEnd());
         ++inIt, ++outIt)
    {
        outIt.Set(inIt.Get());
    }
    
    // Fill region 1.  The input image region is the bottom row of the
    // image.  The x index of the output region moves in lock step with
    // the input image region.  The y index of the output region determines
    // to what extent the image value is interpolated with the constant
    // value.
    inSize[0] = origSize[0];
    inSize[1] = 1;
    inRegion.SetSize(inSize);
    
    outIdx[0] = outOffset[0] + plb[0];
    outIdx[1] = outOffset[1] + 0;
    outSize[0] = origSize[0];
    outSize[1] = plb[1];
    outRegion.SetIndex(outIdx);
    outRegion.SetSize(outSize);
    
    PrintRegionInfo<InputImageType>(inRegion, "Input region 1");
    PrintRegionInfo<OutputImageType>(outRegion, "Output region 1");
    inIt = InputIteratorType(input, inRegion);
    outIt = OutputIteratorType(output, outRegion);
    
    // The y range goes from 0 to PadLowerBound[1]-1
    double spanInv = 1.0 / (plb[1] - 1);
    
    inIt.GoToBegin();
    for (outIt.GoToBegin(); !outIt.IsAtEnd(); ++outIt)
    {
        // reset input iterator, if we've wrapped back around
        if (inIt.IsAtEnd())
            inIt.GoToBegin();
        
        OutputImageIndexType pos = outIt.GetIndex();
        double f = this->functor((double) (pos[1] - outOffset[1]) * spanInv);
        outIt.Set(static_cast< OutputImagePixelType >(f * inIt.Get() + (1-f) * this->GetBoundaryValue()));
        
        // Advance input iterator
        ++inIt;
    }
    
    // Fill region 2.
    // The input image region is the left column of the image.  The y
    // index of the output region moves in lock step with the input image region.
    // The x index of the output region determines to what extent the image value
    // is interpolated to the constant value.
    inSize[0] = 1;
    inSize[1] = origSize[1];
    inRegion.SetSize(inSize);
    
    outIdx[0] = outOffset[0] + 0;
    outIdx[1] = outOffset[1] + plb[1];
    outSize[0] = plb[0];
    outSize[1] = origSize[1];
    outRegion.SetIndex(outIdx);
    outRegion.SetSize(outSize);
    
    PrintRegionInfo<InputImageType>(inRegion, "Input region 2");
    PrintRegionInfo<OutputImageType>(outRegion, "Output region 2");
    inIt = InputIteratorType(input, inRegion);
    outIt = OutputIteratorType(output, outRegion);
    // The x range goes from 0 to PadLowerBound[0] - 1
    spanInv = 1.0 / (plb[0] - 1);
    
    inIt.GoToBegin();
    for (outIt.GoToBegin(); !outIt.IsAtEnd(); ++outIt)
    {
        // reset input iterator, if we've wrapped back around
        if (inIt.IsAtEnd())
            inIt.GoToBegin();
        
        OutputImageIndexType pos = outIt.GetIndex();
        double f = this->functor((double) (pos[0] - outOffset[0]) * spanInv);
        outIt.Set(static_cast< OutputImagePixelType >(f * inIt.Get() + (1-f) * this->GetBoundaryValue()));
        
        // Advance input iterator at end of each X scan line
        if ((pos[0] - outOffset[0]) == plb[0] - 1)
            ++inIt;
    }
    
    // Fill region 3
    // This region runs along the top edge of the image.
    inIdx[0] = 0;
    inIdx[1] = origSize[1] - 1;
    inSize[0] = origSize[0];
    inSize[1] = 1;
    inRegion.SetIndex(inIdx);
    inRegion.SetSize(inSize);
    
    outIdx[0] = outOffset[0] + plb[0];
    outIdx[1] = outOffset[1] + plb[1] + origSize[1];
    outSize[0] = origSize[0];
    outSize[1] = pub[1];
    outRegion.SetIndex(outIdx);
    outRegion.SetSize(outSize);
    
    PrintRegionInfo<InputImageType>(inRegion, "Input region 3");
    PrintRegionInfo<OutputImageType>(outRegion, "Output region 3");
    inIt = InputIteratorType(input, inRegion);
    outIt = OutputIteratorType(output, outRegion);
    
    // The x range goes from size[1] to pub[1] + size[1]
     spanInv = 1.0 / (pub[1] - 1);
    
    inIt.GoToBegin();
    for (outIt.GoToBegin(); !outIt.IsAtEnd(); ++outIt)
    {        
        OutputImageIndexType pos = outIt.GetIndex();
        
        if (inIt.IsAtEnd())
            inIt.GoToBegin();
        
        double f = this->functor((double) (pos[1] - origSize[1]) * spanInv);
        // Logger::verbose << "Pos:\t" << pos[0] << "\t" << pos[1] << "\tf(x) = " << f << std::endl;
        outIt.Set(static_cast< OutputImagePixelType >((1-f) * inIt.Get() + f * this->GetBoundaryValue()));
        
        // Advance input iterator
        ++inIt;
    }
    
    // Fill region 4
    // This region runs along the right side of the image.
    inIdx[0] = origSize[0] - 1;
    inIdx[1] = 0;
    inSize[0] = 1;
    inSize[1] = origSize[1];
    inRegion.SetIndex(inIdx);
    inRegion.SetSize(inSize);
    
    outIdx[0] = outOffset[0] + plb[0] + origSize[0];
    outIdx[1] = outOffset[1] + plb[1];
    outSize[0] = pub[0];
    outSize[1] = origSize[1];
    outRegion.SetIndex(outIdx);
    outRegion.SetSize(outSize);
    
    PrintRegionInfo<InputImageType>(inRegion, "Input region 4");
    PrintRegionInfo<OutputImageType>(outRegion, "Output region 4");
    inIt = InputIteratorType(input, inRegion);
    outIt = OutputIteratorType(output, outRegion);
    
    // The x range goes from size[0] to size[0] + pub[0]
    spanInv = 1.0 / (pub[0] - 1);
    
    inIt.GoToBegin();
    for (outIt.GoToBegin(); !outIt.IsAtEnd(); ++outIt)
    {
        // reset input iterator, if we've wrapped back around
        if (inIt.IsAtEnd())
            inIt.GoToBegin();
        
        OutputImageIndexType pos = outIt.GetIndex();
        double f = this->functor((double) (pos[0] - origSize[0]) * spanInv);
        outIt.Set(static_cast< OutputImagePixelType >((1-f) * inIt.Get() + f * this->GetBoundaryValue()));
        
        // Advance input iterator at end of each X scan line
        if ((pos[0] - origSize[0]) == (pub[0] - 1))
            ++inIt;
    }
    
    // Fill Region 5
    // This region is the lower left corner of the output image.
    // We take the value from the nearest pixel already filled in the output image
    outIdx[0] = outOffset[0];
    outIdx[1] = outOffset[1];
    outSize[0] = plb[0];
    outSize[1] = plb[1];
    outRegion.SetIndex(outIdx);
    outRegion.SetSize(outSize);
    
    PrintRegionInfo<OutputImageType>(outRegion, "Output region 5");
    outIt = OutputIteratorType(output, outRegion);
    
    for (outIt.GoToBegin(); !outIt.IsAtEnd(); ++outIt)
    {
        OutputImageIndexType pos = outIt.GetIndex();
        if (pos[0] > pos[1])
            pos[0] = 0;
        else
            pos[1] = 0;

        outIt.Set(output->GetPixel(pos));
    }
    
    // Fill Region 6
    // This is the upper left hand corner of the image
    outIdx[0] = outOffset[0];
    outIdx[1] = outOffset[1] + plb[1] + origSize[1];
    outSize[0] = plb[0];
    outSize[1] = pub[1];
    outRegion.SetIndex(outIdx);
    outRegion.SetSize(outSize);
    
    PrintRegionInfo<OutputImageType>(outRegion, "Output region 6");
    outIt = OutputIteratorType(output, outRegion);
    
    for (outIt.GoToBegin(); !outIt.IsAtEnd(); ++outIt)
    {
        OutputImageIndexType pos = outIt.GetIndex();
        if ((0 - pos[0]) > pos[1] - origSize[1])
            pos[1] = origSize[1] - 1;
        else
            pos[0] = 0;

        outIt.Set(output->GetPixel(pos));
    }
    
    // Fill Region 7
    // This is the upper right hand corner of the image
    outIdx[0] = outOffset[0] + plb[0] + origSize[0];
    outIdx[1] = outOffset[1] + plb[1] + origSize[1];
    outSize[0] = pub[0];
    outSize[1] = pub[1];
    outRegion.SetIndex(outIdx);
    outRegion.SetSize(outSize);
    
    PrintRegionInfo<OutputImageType>(outRegion, "Output region 7");
    outIt = OutputIteratorType(output, outRegion);
    
    for (outIt.GoToBegin(); !outIt.IsAtEnd(); ++outIt)
    {
        OutputImageIndexType pos = outIt.GetIndex();
        if (pos[0] > pos[1])
            pos[1] = origSize[1]-1;
        else
            pos[0] = origSize[0]-1;

        outIt.Set(output->GetPixel(pos));
    }

    // Fill Region 8
    // This is the lower right hand corner of the image
    outIdx[0] = outOffset[0] + plb[0] + origSize[0];
    outIdx[1] = outOffset[1];
    outSize[0] = pub[0];
    outSize[1] = plb[1];
    outRegion.SetIndex(outIdx);
    outRegion.SetSize(outSize);
    
    PrintRegionInfo<OutputImageType>(outRegion, "Output region 8");
    outIt = OutputIteratorType(output, outRegion);

    for (outIt.GoToBegin(); !outIt.IsAtEnd(); ++outIt)
    {
        OutputImageIndexType pos = outIt.GetIndex();
        if (pos[0] - origSize[0] > (0 - pos[1]))
            pos[1] = 0;
        else
            pos[0] = origSize[0]-1;

        outIt.Set(output->GetPixel(pos));
    }
    
    this->GraftOutput(output);
}

/**
 * Functor for linear interpolation.
 * f(x) = x.
 */
class LinearFunctor
{
public:
    double operator()(double in)
    {
        return in;
    }
};

/**
 * Functor for cos(x) interpolation.
 * f(x) = 0.5 * (1 - cos(pi * x))
 */
class CosFunctor
{
public:
    double operator()(double in)
    {
        return 0.5 * (1 - vcl_cos(vnl_math::pi * in));
    }
};
