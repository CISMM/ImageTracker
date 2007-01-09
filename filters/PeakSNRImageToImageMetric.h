#pragma once

#include "itkObject.h"
#include "itkTimeStamp.h"

/**
 * /class PeakSNRImageToImageMetric
 * /brief Computes the peak signal-to-noise ratio (PSNR) for two images.
 * Given input "source" and "reconstruct" images, this metric computes the PSNR.
 * The PSNR is defined as 20 log (max[S] / RMSE), where max[S] is the maximum
 * value in the source signal and RMSE is the root mean squared error (difference)
 * between the two images.
 * Note this metric is identical to 10 log (max[s]^2 / MSE).  Same thing.
 * This class also makes available the RMSE metric.
 */
template < class TSource, class TReconstruct >
class PeakSNRImageToImageMetric :
        public itk::Object
{
public:
    // Common ITK Typedefs
    typedef PeakSNRImageToImageMetric Self;
    typedef itk::Object Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    
    itkNewMacro(Self);
    
    typedef TSource SourceImageType;
    typedef TReconstruct ReconstructImageType;
    typedef typename SourceImageType::Pointer SourcePointer;
    typedef typename ReconstructImageType::Pointer ReconstructPointer;
    typedef typename SourceImageType::PixelType SourcePixelType;
    typedef typename ReconstructImageType::PixelType ReconstructPixelType;
    
    /** Get the PSNR value **/
    double GetPSNR();
    /** Get the RMSE value **/
    double GetRMSE();
    
    void Update();
    
    SourcePointer GetSourceImage()
    { return this->m_SourceImage; }
    
    ReconstructPointer GetReconstructImage()
    { return this->m_ReconstructImage; }
    
    void SetSourceImage(const SourcePointer& image)
    { 
        this->m_SourceImage = image;
        this->Modified();
    }
    
    void SetReconstructImage(const ReconstructPointer& image)
    { 
        this->m_ReconstructImage = image; 
        this->Modified();
    }
    
protected:
    PeakSNRImageToImageMetric(){}
    virtual ~PeakSNRImageToImageMetric(){}
    
    itkGetMacro(UpdateTime, itk::TimeStamp);
    
private:
    // Not implemented
    PeakSNRImageToImageMetric(const Self& other);
    void operator=(const Self& other);
    
    SourcePointer m_SourceImage;
    ReconstructPointer m_ReconstructImage;
    itk::TimeStamp m_UpdateTime;
    double m_rmse;
    double m_psnr;
};

/** Implementation **/

#include <cmath>
#include "vnl/vnl_math.h"
#include "itkImageRegionConstIterator.h"
#include "itkNumericTraits.h"
#include "Logger.h"

template < class TSource, class TReconstruct >
double
PeakSNRImageToImageMetric< TSource, TReconstruct >
::GetRMSE()
{
    if (this->GetMTime() > this->GetUpdateTime())
        this->Update();
    return this->m_rmse;
}

template < class TSource, class TReconstruct >
double
PeakSNRImageToImageMetric< TSource, TReconstruct >
::GetPSNR()
{
    if (this->GetMTime() > this->GetUpdateTime())
        this->Update();
    return this->m_psnr;
}

template < class TSource, class TReconstruct >
void
PeakSNRImageToImageMetric< TSource, TReconstruct >
::Update()
{
    if (this->m_SourceImage.IsNull() ||
        this->m_ReconstructImage.IsNull())
    {
        Logger::warning << "PeakSNRImageToImageMetric::Update(): Source or Reconstruct image not set; not updating." << std::endl;
        return;
    }
    
    typedef itk::ImageRegionConstIterator< SourceImageType > SourceIterator;
    typedef itk::ImageRegionConstIterator< ReconstructImageType > ReconstructIterator;
    typedef typename itk::NumericTraits< SourcePixelType >::AccumulateType AccumulateType;
    
    SourcePixelType maxSource(itk::NumericTraits< SourcePixelType >::min());
    SourceIterator srcIt(this->GetSourceImage(), this->GetSourceImage()->GetLargestPossibleRegion());
    ReconstructIterator estIt(this->GetReconstructImage(), this->GetReconstructImage()->GetLargestPossibleRegion());
    
    unsigned long count(0);
    AccumulateType ssd(0);
    
    for (srcIt.GoToBegin(), estIt.GoToBegin();
         !(srcIt.IsAtEnd() || estIt.IsAtEnd());
         ++srcIt, ++estIt)
    {
        maxSource = srcIt.Get() > maxSource ? srcIt.Get() : maxSource;
        AccumulateType diff(srcIt.Get() - estIt.Get());
        ssd += diff*diff;
        count++;
    }
    
    // Compute RMSE
    this->m_rmse = static_cast<double>(vcl_sqrt(ssd / count));
    // Compute PSNE
    this->m_psnr = static_cast<double>(20 * vcl_log10( maxSource / this->m_rmse));
    this->m_UpdateTime.Modified();
}
