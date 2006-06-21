//
// C++ Interface: 
//
// Description: NaryMeanVectorImageFilter computes an average vector at each
// pixel location from a set of vector images.
//
//
// Author:  <beastwoo>, (C) 2006
//
#pragma once

#include "itkImage.h"
#include "itkImageToImageFilter.h"
#include "itkVector.h"

/**
 * /class NaryMeanVectorImageFilter
 * NaryMeanVectorImageFilter computes an average vector at each
 * pixel location from a set of vector images. The class is
 * templated over the input and output image. It is expected that
 * both types of images have the same number of image dimensions and
 * itk::Vector-type pixels with the same nubmer of vector dimensions. 
 * This class is specialized for Vector image types because the generic
 * NaryFunctor solution fails due to numeric traits issues (itk::Vector 
 * pixels do not have proper numeric traits). For images with scalar-type 
 * pixels, use NaryMeanImageFilter.
 */
template < class TInput, class TOutput >
class NaryMeanVectorImageFilter :
    public itk::ImageToImageFilter< TInput, TOutput >
{
public:
    // standard itk typedefs
    typedef NaryMeanVectorImageFilter Self;
    typedef itk::ImageToImageFilter< TInput, TOutput > Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;

    itkNewMacro(Self);

    // helpful typedefs
    typedef TInput InputImageType;
    typedef TOutput OutputImageType;

    // vector types...
    typedef typename InputImageType::PixelType InputPixelType;
    typedef typename OutputImageType::PixelType OutputPixelType;
    typedef typename InputPixelType::ComponentType InputComponentType;
    typedef typename OutputPixelType::ComponentType OutputComponentType;
    typedef typename itk::NumericTraits< InputComponentType >::AccumulateType AccumulateType;

    // Extract info from image types.
    itkStaticConstMacro(ImageDimension, 
        unsigned int, 
        OutputImageType::ImageDimension);
    itkStaticConstMacro(VectorDimension,
        unsigned int,
        OutputPixelType::Dimension);

    typedef itk::Vector< AccumulateType, VectorDimension > AccumulateVectorType;
    typedef itk::Image< AccumulateVectorType, ImageDimension > AccumulateImageType;

    /**
     * Add an image to the set averaged by this filter.
     */
    void AddInput(const TInput* image)
    {
        Superclass::SetInput(this->inputIdx, const_cast< TInput* >(image));
        this->inputIdx++;
    }

protected:
    void GenerateData();

    NaryMeanVectorImageFilter() : inputIdx(0) {}
    virtual ~NaryMeanVectorImageFilter() {}

private:
    // Non-implemented functions
    NaryMeanVectorImageFilter(const Self&);
    void operator=(const Self&);

    unsigned int inputIdx;
};

// Implementation //

#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "itkVectorCastImageFilter.h"
#include "ImageUtils.h"
#include "Logger.h"

template < class TInput, class TOutput >
void NaryMeanVectorImageFilter< TInput, TOutput >::
GenerateData()
{
    Logger::verbose << "NaryMeanVectorImageFilter::GenerateData()" << std::endl;
    
    // Initialize output
    Logger::verbose << "Allocating outputs." << std::endl;
    Superclass::AllocateOutputs();
    typename OutputImageType::Pointer output = this->GetOutput();
    output->SetBufferedRegion(output->GetRequestedRegion());
    
    // Create an accumulation image
    Logger::verbose << "Creating accumulation image." << std::endl;
    typename AccumulateImageType::Pointer accumulate = AccumulateImageType::New();
    typename AccumulateImageType::RegionType accumRegion;
    CopyRegion< OutputImageType, AccumulateImageType >(output->GetRequestedRegion(), accumRegion);
    Logger::verbose << "Size:  (" << accumRegion.GetSize()[0] << ", " << accumRegion.GetSize()[1] << ")" << std::endl;
    Logger::verbose << "Index: (" << accumRegion.GetIndex()[0] << ", " << accumRegion.GetIndex()[1] << ")" << std::endl;
    
    accumulate->SetRegions(accumRegion);
    accumulate->Allocate();
    
    // Define an image caster to change the input pixel type
    typedef itk::VectorCastImageFilter< InputImageType, AccumulateImageType > InputCasterType;
    typename InputCasterType::Pointer caster = InputCasterType::New();
    
    // Define iterator types
    typedef itk::ImageRegionConstIterator< AccumulateImageType > ConstAccumulateIterator;
    typedef itk::ImageRegionIterator< AccumulateImageType > AccumulateIterator;
    typedef itk::ImageRegionIterator< OutputImageType > OutputIterator;
    
    // Zero accumulate image
    Logger::verbose << "Zeroing accumulation image." << std::endl;
    AccumulateIterator accumIt(accumulate, accumRegion);
    AccumulateVectorType zero;
    zero.Fill(0);
    for (accumIt.GoToBegin(); !accumIt.IsAtEnd(); ++accumIt)
    {
        accumIt.Set(zero);
    }
    
    // Create accumulation of all input images
    Logger::verbose << "Calculating accumulation image..." << std::endl;
    const unsigned int inputs = this->GetNumberOfInputs();
    for (unsigned int i = 0; i < inputs; i++)
    {
        // WriteImage<InputImageType>(this->GetInput(i), "./input.mha");
        caster->SetInput(this->GetInput(i));
        caster->Update();
        ConstAccumulateIterator inputIt(caster->GetOutput(), accumRegion);
        
        for (inputIt.GoToBegin(), accumIt.GoToBegin();
            !(inputIt.IsAtEnd() || accumIt.IsAtEnd());
            ++inputIt, ++accumIt)
        {
            // Logger::verbose << ".";
            accumIt.Set(accumIt.Get() + inputIt.Get());
        }
    }
    Logger::verbose << std::endl;;
    
    // Define a vector caster to change the accumulation pixels to output
    typedef itk::Functor::VectorCast< AccumulateVectorType, OutputPixelType > OutputCastType;
    OutputCastType outcast;
    
    // Define iterators for calculating the mean vector at each location
    ConstAccumulateIterator sumIt(accumulate, accumRegion);
    OutputIterator outIt(output, output->GetRequestedRegion());
    
    // Calculate mean at each location
    Logger::verbose << "Calculating mean image." << std::endl;
    for (sumIt.GoToBegin(), outIt.GoToBegin();
        !(sumIt.IsAtEnd() || outIt.IsAtEnd());
        ++sumIt, ++outIt)
    {
        outIt.Set(outcast(sumIt.Get() / inputs));
    }
    
    // Graft output
    Logger::verbose << "Grafting output." << std::endl;
    this->GraftOutput(output);
}
