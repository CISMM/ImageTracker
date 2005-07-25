#pragma once

#include "itkBilateralImageFilter.h"
#include "itkImageToImageFilter.h"
#include "itkImage.h"
#include "itkFixedArray.h"
#include "itkNeighborhoodIterator.h"
#include "itkConstNeighborhoodIterator.h"
#include "itkNeighborhood.h"
#include "itkVector.h"

namespace itk
{
/**
 * \class BilateralVectorImageFilter
 * \brief Blurs a vector image while preserving edges
 *
 * This filter uses bilateral filtering to blur an image using both
 * domain and range "neighborhoods". Pixels that are close to a pixel
 * in the image domain and similar to a pixel in the image range are
 * used to calculate the filtered value. Two gaussian kernels (one in
 * the image domain and one in the image range) are used to smooth
 * the image. The result is an image that is smoothed in homogeneous
 * regions yet has edges preserved. The result is similar to
 * anisotropic diffusion but the implementation in non-iterative.
 * Another benefit to bilateral filtering is that any distance metric
 * can be used for kernel smoothing the image range.  Hence, color
 * images can be smoothed as vector images, using the CIE distances
 * between intensity values as the similarity metric (the Gaussian
 * kernel for the image domain is evaluated using CIE distances).
 * A separate version of this filter will be designed for color
 * and vector images.
 *
 * Bilateral filtering is capable of reducing the noise in an image
 * by an order of magnitude while maintaining edges.
 *
 * The bilateral operator used here was described by Tomasi and
 * Manduchi (Bilateral Filtering for Gray and ColorImages. IEEE
 * ICCV. 1998.)
 *
 * \sa GaussianOperator
 * \sa AnisotropicDiffusionImageFilter
 * \sa Image
 * \sa Neighborhood
 * \sa NeighborhoodOperator
 * 
 * \ingroup ImageEnhancement 
 * \ingroup ImageFeatureExtraction 
 * \todo Support color images
 * \todo Support vector images
 */

template <class TInputImage, class TOutputImage >
class ITK_EXPORT BilateralVectorImageFilter :
    public ImageToImageFilter< TInputImage, TOutputImage > 
{
public:
  /** Standard class typedefs. */
  typedef BilateralVectorImageFilter Self;
  typedef ImageToImageFilter< TInputImage, TOutputImage > Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(BilateralVectorImageFilter, ImageToImageFilter);
  
  /** Image type information. */
  typedef TInputImage  InputImageType;
  typedef TOutputImage OutputImageType;

  /** Superclass typedefs. */
  typedef typename Superclass::OutputImageRegionType OutputImageRegionType;

  /** Extract some information from the image types.  Dimensionality
   * of the two images is assumed to be the same. */
  typedef typename TOutputImage::PixelType OutputPixelType;
  typedef typename TOutputImage::InternalPixelType OutputInternalPixelType;
  typedef typename TOutputImage::PixelType::ValueType OutputPixelRealType;
  typedef typename TInputImage::PixelType InputPixelType;
  typedef typename TInputImage::InternalPixelType InputInternalPixelType;

  /** Extract some information from the image types.  Dimensionality
   * of the two images is assumed to be the same. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TOutputImage::ImageDimension);
  
  /** Typedef of double containers */
  typedef FixedArray<double, itkGetStaticConstMacro(ImageDimension)> ArrayType;

  /** Neighborhood iterator types. */
  typedef ConstNeighborhoodIterator<TInputImage> 
  NeighborhoodIteratorType ;
  
  /** Kernel typedef. */
  typedef
  Neighborhood<double, itkGetStaticConstMacro(ImageDimension)> KernelType;
  typedef typename KernelType::SizeType SizeType;
  
  /** Kernel iterator. */
  typedef typename KernelType::Iterator KernelIteratorType ;
  typedef typename KernelType::ConstIterator KernelConstIteratorType ;

  /** Gaussian image type */
  typedef
  Image<double, itkGetStaticConstMacro(ImageDimension)> GaussianImageType;
  
  /** Standard get/set macros for filter parameters.
   * DomainSigma is specified in the same units as the Image spacing.
   * RangeSigma is specified in the units of intensity. */
  itkSetMacro(DomainSigma, ArrayType);
  itkGetMacro(DomainSigma, const ArrayType);
  itkSetMacro(RangeSigma, double);
  itkGetMacro(RangeSigma, double);
  itkGetMacro(FilterDimensionality, unsigned int);
  itkSetMacro(FilterDimensionality, unsigned int);
  
  /** Convenience get/set methods for setting all domain parameters to the
   * same values.  */
  void SetDomainSigma(const double v)
  {
    m_DomainSigma.Fill(v);
  }

  /** Control automatic kernel size determination. When
   * automatic is "on", the kernel size is a function of the domain
   * sigma. When automatic is "off", the kernel size is whatever is
   * specified by the user.
   * \sa SetRadius() */
  itkBooleanMacro(AutomaticKernelSize);
  itkGetMacro(AutomaticKernelSize, bool);
  itkSetMacro(AutomaticKernelSize, bool);

  /** Set/Get the kernel radius, specified in pixels.  This parameter
   * is used only when AutomaticNeighborhoodSize is "off". */
  void SetRadius(const unsigned long r)
  {
      this->m_Radius.Fill(r);
  }
  itkSetMacro(Radius, SizeType);
  itkGetConstReferenceMacro(Radius, SizeType);
  
  
  /** Set/Get the number of samples in the approximation to the Gaussian
   * used for the range smoothing. Samples are only generated in the
   * range of [0, 4*m_RangeSigma]. Default is 100. */
  itkSetMacro(NumberOfRangeGaussianSamples, unsigned long);
  itkGetMacro(NumberOfRangeGaussianSamples, unsigned long);
  
protected:
  /** Constructor.  Default value for DomainSigma is 4. Default value
   * RangeSigma is . */
  BilateralVectorImageFilter()
  {
    m_Radius.Fill(1);
    m_AutomaticKernelSize = true;
    m_DomainSigma.Fill(4.0);
    m_RangeSigma = 0.5;
    m_FilterDimensionality = ImageDimension;
    m_NumberOfRangeGaussianSamples = 100;
    m_DynamicRange = 0.0;
    m_DynamicRangeUsed = 0.0;
    m_DomainMu = 2.5;  // keep small to keep kernels small
    m_RangeMu = 4.0;   // can be bigger then DomainMu since we only
                       // index into a single table

  }
  virtual ~BilateralVectorImageFilter() {}
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** Do some setup before the ThreadedGenerateData */
  void BeforeThreadedGenerateData();
  
  /** Standard pipeline method. This filter is implemented as a multi-threaded
   * filter. */
  void ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
                            int threadId); 

  /** BilateralImageFilter needs a larger input requested region than
   * the output requested region (larger by the size of the domain
   * Gaussian kernel).  As such, BilateralImageFilter needs to provide
   * an implementation for GenerateInputRequestedRegion() in order to
   * inform the pipeline execution model.  
   * \sa ImageToImageFilter::GenerateInputRequestedRegion() */
  virtual void GenerateInputRequestedRegion() throw(InvalidRequestedRegionError);

  
private:
  BilateralVectorImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** The standard deviation of the gaussian blurring kernel in the image
      range. Units are intensity. */
  double m_RangeSigma;
  
  /** The standard deviation of the gaussian blurring kernel in each
      dimensional direction. Units match image spacing units. */
  ArrayType m_DomainSigma;

  /** Multiplier used to define statistical thresholds.  Gaussians are
   * only evaluated to m_DomainMu*m_DomainSigma or m_RangeMu*m_RangeSigma. */
  double m_DomainMu;
  double m_RangeMu;

  /** Number of dimensions to process. Default is all dimensions */
  unsigned int m_FilterDimensionality;

  /** Gaussian kernel used for smoothing in the spatial domain */
  KernelType m_GaussianKernel;
  SizeType   m_Radius;
  bool       m_AutomaticKernelSize;

  /** Variables for the lookup table of range gaussian values */
  unsigned long m_NumberOfRangeGaussianSamples;
  double m_DynamicRange;
  double m_DynamicRangeUsed;
  std::vector<double> m_RangeGaussianTable;
};
  
} // end namespace itk

/************************************************************************/
/* Implementation                                                                     */
/************************************************************************/

#include "BilateralVectorImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkGaussianImageSource.h"
#include "itkNeighborhoodAlgorithm.h"
#include "itkZeroFluxNeumannBoundaryCondition.h"
#include "itkProgressReporter.h"

namespace itk
{
template <class TInputImage, class TOutputImage>
    void 
    BilateralVectorImageFilter<TInputImage,TOutputImage>
    ::GenerateInputRequestedRegion() throw(InvalidRequestedRegionError)
{
    // call the superclass' implementation of this method. this should
    // copy the output requested region to the input requested region
    Superclass::GenerateInputRequestedRegion();

    // get pointers to the input and output
    typename Superclass::InputImagePointer  inputPtr = 
        const_cast< TInputImage *>( this->GetInput() );

    if ( !inputPtr )
    {
        return;
    }

    // Pad the image by 2.5*sigma in all directions
    typename TInputImage::SizeType radius;
    unsigned int i;

    if (m_AutomaticKernelSize)
    {
        for (i = 0; i < ImageDimension; i++)
        {
            radius[i] =
                (typename TInputImage::SizeType::SizeValueType)
                ceil(m_DomainMu*m_DomainSigma[i]/this->GetInput()->GetSpacing()[i]);
        }
    }
    else
    {
        for (i = 0; i < ImageDimension; i++)
        {
            radius[i] = m_Radius[i];
        }
    }

    // get a copy of the input requested region (should equal the output
    // requested region)
    typename TInputImage::RegionType inputRequestedRegion;
    inputRequestedRegion = inputPtr->GetRequestedRegion();

    // pad the input requested region by the operator radius
    inputRequestedRegion.PadByRadius( radius );

    // crop the input requested region at the input's largest possible region
    if ( inputRequestedRegion.Crop(inputPtr->GetLargestPossibleRegion()) )
    {
        inputPtr->SetRequestedRegion( inputRequestedRegion );
        return;
    }
    else
    {
        // Couldn't crop the region (requested region is outside the largest
        // possible region).  Throw an exception.

        // store what we tried to request (prior to trying to crop)
        inputPtr->SetRequestedRegion( inputRequestedRegion );

        // build an exception
        InvalidRequestedRegionError e(__FILE__, __LINE__);
        OStringStream msg;
        msg << static_cast<const char *>(this->GetNameOfClass())
            << "::GenerateInputRequestedRegion()";
        e.SetLocation(msg.str().c_str());
        e.SetDescription("Requested region is (at least partially) outside the largest possible region.");
        e.SetDataObject(inputPtr);
        throw e;
    }
}

template< class TInputImage, class TOutputImage >
    void
    BilateralVectorImageFilter<TInputImage, TOutputImage>
    ::BeforeThreadedGenerateData()
{
    // Build a small image of the N-dimensional Gaussian used for domain filter
    //
    // Gaussian image size will be (2*ceil(2.5*sigma)+1) x (2*ceil(2.5*sigma)+1)
    unsigned int i;
    typename InputImageType::SizeType radius;
    typename InputImageType::SizeType domainKernelSize;

    const InputImageType * inputImage = this->GetInput();

    const typename InputImageType::SpacingType inputSpacing = inputImage->GetSpacing();
    const typename InputImageType::PointType   inputOrigin  = inputImage->GetOrigin();

    if (m_AutomaticKernelSize)
    {
        for (i = 0; i < ImageDimension; i++)
        {
            radius[i] =
                (typename TInputImage::SizeType::SizeValueType)
                ceil( m_DomainMu * m_DomainSigma[i] / inputSpacing[i] );
            domainKernelSize[i] = 2 * radius[i] + 1;
        }
    }
    else
    {
        for (i = 0; i < ImageDimension; i++)
        {
            radius[i] = m_Radius[i];
            domainKernelSize[i] = 2 * radius[i] + 1;
        }
    }

    typename GaussianImageSource<GaussianImageType>::Pointer gaussianImage;
    typename GaussianImageSource<GaussianImageType>::ArrayType mean;
    typename GaussianImageSource<GaussianImageType>::ArrayType sigma;

    gaussianImage = GaussianImageSource<GaussianImageType>::New();
    gaussianImage->SetSize(domainKernelSize.GetSize());
    gaussianImage->SetSpacing( inputSpacing );
    gaussianImage->SetOrigin( inputOrigin );
    gaussianImage->SetScale( 1.0 );
    gaussianImage->SetNormalized( true );

    for (i=0; i < ImageDimension; i++)
    {
        mean[i] = inputSpacing[i] * radius[i] + inputOrigin[i]; // center pixel pos
        sigma[i] = m_DomainSigma[i];
    }
    gaussianImage->SetSigma( sigma );
    gaussianImage->SetMean( mean );

    gaussianImage->Update();

    // copy this small Gaussian image into a neighborhood
    m_GaussianKernel.SetRadius(radius);

    KernelIteratorType kernel_it;
    ImageRegionIterator<GaussianImageType> git
        = ImageRegionIterator<GaussianImageType>(gaussianImage->GetOutput(),
        gaussianImage->GetOutput()
        ->GetBufferedRegion() );
    double norm = 0.0;
    for (git.GoToBegin(); !git.IsAtEnd(); ++git)
    {
        norm += git.Get();
    }
    for (git.GoToBegin(), kernel_it = m_GaussianKernel.Begin(); !git.IsAtEnd();
        ++git, ++kernel_it)
    {
        *kernel_it = git.Get() / norm;
    }

    // Build a lookup table for the range gaussian
    //

    // Create the lookup table whose domain runs from 0.0 to
    // 1.0 and range is gaussian evaluated at that point
    double rangeVariance = m_RangeSigma * m_RangeSigma;

    // denominator (normalization factor) for Gaussian used for range
    double rangeGaussianDenom;
    rangeGaussianDenom = m_RangeSigma*sqrt(2.0*3.1415927);

    // Maximum delta for the dynamic range
    double tableDelta;
    double v;

    // Maximum range value when vectors point in same direction: 1.0
    m_DynamicRange = 1.0;

    m_DynamicRangeUsed = m_RangeMu * m_RangeSigma;

    tableDelta = m_DynamicRangeUsed
        / static_cast<double>(m_NumberOfRangeGaussianSamples);

    // Finally, build the table
    m_RangeGaussianTable.reserve( m_NumberOfRangeGaussianSamples );
    for (i = 0, v = 0.0; i < m_NumberOfRangeGaussianSamples;
        ++i, v += tableDelta)
    {
        m_RangeGaussianTable[i] = exp(-0.5*v*v/rangeVariance)/rangeGaussianDenom;
    }
}

template< class TInputImage, class TOutputImage >
    void
    BilateralVectorImageFilter<TInputImage, TOutputImage>
    ::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
    int threadId) 
{
    typename TInputImage::ConstPointer input = this->GetInput();
    typename TOutputImage::Pointer output = this->GetOutput();
    unsigned long i;
    const double rangeDistanceThreshold = 1.0;

    // Now we are ready to bilateral filter!

    // Boundary condition
    ZeroFluxNeumannBoundaryCondition<TInputImage> BC;

    // Find the boundary "faces"
    typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType>::FaceListType faceList;
    NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType> fC;
    faceList = fC(this->GetInput(), outputRegionForThread,
        m_GaussianKernel.GetRadius());

    typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType>::FaceListType::iterator fit;

    OutputPixelType centerPixel, centerPixelNorm;
    OutputPixelType pixel, pixelNorm;
    OutputPixelType val;
    double rangeDistance;
    OutputPixelRealType tableArg, normFactor, 
        rangeGaussian, gaussianProduct;

    const double distanceToTableIndex
        = static_cast<double>(m_NumberOfRangeGaussianSamples)/m_DynamicRangeUsed;


    // Process all the faces, the NeighborhoodIterator will deteremine
    // whether a specified region needs to use the boundary conditions or
    // not.
    NeighborhoodIteratorType b_iter;
    ImageRegionIterator<OutputImageType> o_iter;
    KernelConstIteratorType k_it;
    KernelConstIteratorType kernelEnd = m_GaussianKernel.End();

    ProgressReporter progress(this, threadId, outputRegionForThread.GetNumberOfPixels());

    for (fit = faceList.begin(); fit != faceList.end(); ++fit)
    { 
        // walk the boundary face and the corresponding section of the output
        b_iter = NeighborhoodIteratorType(m_GaussianKernel.GetRadius(),
            this->GetInput(), *fit);
        b_iter.OverrideBoundaryCondition(&BC);
        o_iter = ImageRegionIterator<OutputImageType>(this->GetOutput(), *fit);

        while ( ! b_iter.IsAtEnd() )
        {
            // Setup
            centerPixel = static_cast<OutputPixelType>(b_iter.GetCenterPixel());
            centerPixelNorm = centerPixel;
            centerPixelNorm.Normalize();
            val = OutputPixelType(0.0);
            normFactor = 0.0;

            // Walk the neighborhood of the input and the kernel
            for (i=0, k_it = m_GaussianKernel.Begin(); k_it < kernelEnd;
                ++k_it, ++i)
            {
                // range distance between neighborhood pixel and neighborhood center
                pixel = static_cast<OutputPixelType>(b_iter.GetPixel(i));
                pixelNorm = pixel;
                pixelNorm.Normalize();

                rangeDistance = centerPixelNorm * pixelNorm;

                // if the range distance is close enough, then use the pixel
                if (rangeDistance >= 0.0 && rangeDistance < rangeDistanceThreshold)
                {
                    // Make range distance appropriate for Gaussian:
                    rangeDistance = 1.0 - rangeDistance;
                    // look up the range gaussian in a table
                    tableArg = rangeDistance * distanceToTableIndex;
                    rangeGaussian = m_RangeGaussianTable[BilateralFloor(tableArg)];

                    // normalization factor so filter integrates to one
                    // (product of the domain and the range gaussian)
                    gaussianProduct = (*k_it) * rangeGaussian;
                    normFactor += gaussianProduct;

                    // Input Image * Domain Gaussian * Range Gaussian 
                    val += pixel * gaussianProduct;
                }
            }
            // normalize the value
            val /= normFactor;

            // store the filtered value
            o_iter.Set( static_cast<OutputPixelType>(val) );

            ++b_iter;
            ++o_iter;
            progress.CompletedPixel();
        }
    }
}

template< class TInputImage, class TOutputImage >
    void
    BilateralVectorImageFilter<TInputImage, TOutputImage>
    ::PrintSelf(std::ostream& os, Indent indent) const
{
    Superclass::PrintSelf(os,indent);

    os << indent << "DomainSigma: " << m_DomainSigma << std::endl;
    os << indent << "RangeSigma: " << m_RangeSigma << std::endl;
    os << indent << "FilterDimensionality: " << m_FilterDimensionality << std::endl;
    os << indent << "NumberOfRangeGaussianSamples: " << m_NumberOfRangeGaussianSamples << std::endl;
    os << indent << "Input dynamic range: " << m_DynamicRange << std::endl;
    os << indent << "Amount of dynamic range used: " << m_DynamicRangeUsed << std::endl;
    os << indent << "AutomaticKernelSize: " << m_AutomaticKernelSize << std::endl;
    os << indent << "Radius: " << m_Radius << std::endl;
}

} // end namespace itk

