#pragma once

#include <vector>
#include "itkObject.h"
#include "itkTimeStamp.h"

/**
 * /class PercentileImageMetric
 * /brief Computes percentile intensity values for input images.
 * The nth percentile, where 0 <= n <= 100, is a number that divides 
 * a data set of N items into two regions.  N * n / 100 data items are 
 * below the nth percentile and N * (1-n)/100 items are above the nth
 * percentile.  Familiar percentiles include the 0th percentile (the 
 * minimum), the 100th percentile (the maximum), and the 50th percentile
 * (the median).
 */
template < class TInputImage >
class PercentileImageMetric:
    public itk::Object
{
public:
    // Common itk types
    typedef PercentileImageMetric Self;
    typedef itk::Object Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    
    itkNewMacro(Self);
    
    typedef TInputImage InputImageType;
    typedef typename InputImageType::Pointer InputImagePointer;
    typedef typename InputImageType::PixelType InputPixelType;
    typedef typename InputImageType::RegionType InputRegionType;
    typedef typename InputImageType::SizeType InputSizeType;
    
    itkStaticConstMacro(ImageDimension, unsigned int, InputImageType::ImageDimension);
    
    void Update();
    
    /** Retrieves the n*100th percentile, where n is between 0 and 1. */
    InputPixelType GetPercentile(double n);
    
    InputImagePointer GetInputImage()
    { return m_InputImage; }
    
    void SetInputImage(const InputImagePointer& image)
    { 
        this->m_InputImage = image;
        this->Modified();
    }
    
protected:
    PercentileImageMetric(){}
    virtual ~PercentileImageMetric(){}
    
    itkGetMacro(UpdateTime, itk::TimeStamp);
    
private:
    // Not implemented
    PercentileImageMetric(const Self& other);
    void operator=(const Self& other);
    
    InputImagePointer m_InputImage;
    itk::TimeStamp m_UpdateTime;
    std::vector< InputPixelType > m_SortedPixels;
};

/** Implementation **/

#include "Logger.h"
#include "itkImageRegionConstIterator.h"
#include "itkNumericTraits.h"

template < class TInputImage >
typename PercentileImageMetric< TInputImage >::InputPixelType
PercentileImageMetric< TInputImage >::GetPercentile(double n)
{
    if (this->GetMTime() > this->GetUpdateTime())
        this->Update();
    if (this->m_SortedPixels.size() > 0 && 
       n >= 0.0 &&
       n <= 1.0)
        return this->m_SortedPixels[(unsigned int)(n * (this->m_SortedPixels.size() - 1))];
    else
        return itk::NumericTraits<InputPixelType>::min();
}

template < class TInputImage >
void
PercentileImageMetric< TInputImage >::Update()
{
    // clear any previously sorted pixel set
    this->m_SortedPixels.clear();
    
    // Check input image
    if (this->m_InputImage.IsNull())
    {
        Logger::warning << "PercentileImageMetric::Update(): Input image not set; not updating." << std::endl;
        return;
    }
    
    InputImagePointer image = this->GetInputImage();
    
    // Reserve enough room in the pixel set
    unsigned long pixelCount = 1;
    InputSizeType size = image->GetLargestPossibleRegion().GetSize();
    for (unsigned int d = 0; d < ImageDimension; d++)
    {
        pixelCount *= size[d];
    }
    this->m_SortedPixels.reserve(pixelCount);
    
    // Add all pixels in input image to pixel list
    typedef itk::ImageRegionConstIterator< InputImageType > IteratorType;
    IteratorType it(image, image->GetLargestPossibleRegion());
    for (it.GoToBegin(); !it.IsAtEnd(); ++it)
    {
        this->m_SortedPixels.push_back(it.Get());
    }
    
    // Sort pixels
    std::sort(this->m_SortedPixels.begin(), this->m_SortedPixels.end());
}
