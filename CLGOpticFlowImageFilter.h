#pragma once

#include "itkImage.h"
#include "itkImageToImageFilter.h"
#include "itkInPlaceImageFilter.h"
#include "itkVector.h"

template <class TInputImage1, class TInputImage2, class TOutputValueType = float >
class CLGOpticFlowImageFilter :
    public itk::ImageToImageFilter< TInputImage1,
        itk::Image< itk::Vector <TOutputValueType, itk::GetImageDimension<TInputImage1>::ImageDimension>, ::itk::GetImageDimension<TInputImage1>::ImageDimension> >
{
public:
    /** Some convenient typedefs */
    typedef TInputImage1 Input1ImageType;
    typedef typename Input1ImageType::ConstPointer Input1ImagePointer;
    typedef typename Input1ImageType::RegionType Input1ImageRegionType;
    typedef typename Input1ImageType::PixelType Input1ImagePixelType;
    typedef TInputImage2 Input2ImageType;
    typedef typename Input2ImageType::ConstPointer Input2ImagePointer;
    typedef typename Input2ImageType::RegionType Input2ImageRegionType;
    typedef typename Input2ImageType::PixelType Input2ImagePixelType;

    /** Image typedef support */
    itkStaticConstMacro(ImageDimension, unsigned int, Input1ImageType::ImageDimension);
    typedef typename Input1ImageType::PixelType InputPixelType;
    typedef Input1ImageType InputImageType;

    typedef TOutputValueType OutputValueType;
    //typedef itk::Vector<OutputValueType, ImageDimension> InputPixelType;
    //typedef itk::Image<InputPixelType, ImageDimension> InputImageType;
    typedef itk::Vector<OutputValueType, ImageDimension> OutputPixelType;
    typedef itk::Image<OutputPixelType, ImageDimension> OutputImageType;
    typedef typename InputImageType::Pointer InputImagePointer;
    typedef typename OutputImageType::Pointer OutputImagePointer;
    typedef typename OutputImageType::RegionType OutputImageRegionType;
    typedef OutputPixelType VectorType;

    /** Internal processing types */
    typedef float InternalValueType;
    /** 
    * A tensor represents the vector square of the image
    * gradient in three directions ([dx, dy, dt]^T * [dx dy dt]).
    * Yes, that would be a 3x3 matrix with 9 components, but we
    * actually only need the first two rows.
    */
    enum { TensorSize = 6};
    typedef itk::Vector< InternalValueType, TensorSize > TensorType;
    typedef itk::Image< TensorType, ImageDimension > TensorImageType;
    typedef typename TensorImageType::Pointer TensorImagePointer;

    /** Standard itk class typedefs */
    typedef CLGOpticFlowImageFilter Self;
    typedef itk::ImageToImageFilter<InputImageType, OutputImageType> Superclass;
    typedef itk::SmartPointer<Self> Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;

    /** itk New factory method and type info. */
    itkNewMacro(Self);
    itkTypeMacro(CLGOpticFlowImageFilter, InPlaceImageFilter);

    TInputImage1 * GetInput1()
    {
        return const_cast<TInputImage1 *> (this->GetInput(0));
    }

    /** 
    * Set the first (moving) image for Optic Flow. Note, the image
    * data must be loaded (updated) before this filter's Update() 
    * method is called.
    */
    void SetInput1(const TInputImage1 * image1)
    {
        this->SetNthInput(0, const_cast<TInputImage1 *>(image1));
    }

    TInputImage2 * GetInput2()
    {
        return const_cast<TInputImage2 *> (this->GetInput(1));
    }

    /** 
    * Set the second (fixed) image for Optic Flow. Note, the image
    * data must be loaded (updated) before this filter's Update() 
    * method is called.
    */
    void SetInput2(const TInputImage2 * image2)
    {
        this->SetNthInput(1, const_cast<TInputImage2 *>(image2));
    }

    /** 
     * Get/Set the spatial filter deviation. This is the smoothing
     * applied to the input images before differentiation.
     */
    itkGetMacro(SpatialSigma, double);
    itkSetMacro(SpatialSigma, double);

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
     * Get/Set the iteration count; the number of SOR iterations
     * to use to compute the flow field.
     */
    itkGetMacro(Iterations, unsigned int);
    itkSetMacro(Iterations, unsigned int);

protected:
    CLGOpticFlowImageFilter()
    {
        this->m_SpatialSigma = 1.0;
        this->m_Regularization = 10E3;
        this->m_Relaxation = 1.9;
        this->m_Iterations = 200;
    }
    virtual ~CLGOpticFlowImageFilter() {}

    void PrintSelf(std::ostream& os, itk::Indent indent) const;
    void LogSelf();

    /** 
     * Do some setup before generating data. Specifically, we need
     * to find the spatial and temporal derivatives of the input
     * images, and create a structure tensor from this.
     */
    void BeforeGenerateData();

    /** 
     * CLGOpticalFlowImageFilter needs a larger input requested
     * region than the output requested region, by one pixel in
     * radius. This includes the 4-neighborhood for each image
     * pixel.
     */
    virtual void GenerateInputRequestedRegion() throw (itk::InvalidRequestedRegionError);

    /** 
     * Generate the data; construct a vector field representing the
     * motion from Image1 to Image2. The CLGOpticFlowImageFilter cannot
     * be threaded because the output flow field is calculated iteratively.
     * In each iteration, a given pixel's value depends on the value of
     * the surrounding pixels at the same or previous iteration step.
     */
    void GenerateData();

    /** 
     * Perform extra processing after the flow field has been
     * calculated. This computes the error image.
     */
    void AfterGenerateData();

private:    
    // Algorithm terms.
    double m_SpatialSigma;
    double m_Regularization;
    double m_Relaxation;
    unsigned int m_Iterations;

    // Internal processing objects
    /** Structure tensor */
    TensorImagePointer m_J;
};

/************************************************************************/
/* Implementation                                                                     */
/************************************************************************/

#include "itkConstNeighborhoodIterator.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkImageDuplicator.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "itkProgressReporter.h"
#include "itkRecursiveGaussianImageFilter.h"
#include "itkSubtractImageFilter.h"

#include "Logger.h"

template <class TInputImage1, class TInputImage2, class TOutputValueType>
void CLGOpticFlowImageFilter<TInputImage1, TInputImage2, TOutputValueType>
::GenerateInputRequestedRegion() throw (itk::InvalidRequestedRegionError)
{
    Logger::logDebug("CLGOpticFlowImageFilter::GenerateInputRequestedRegion()");
    // We need to pad the requested region by one pixel, because
    // the CLG algorithm requires the 4-neighbors of each image
    // pixel.

    // Call Superclass implementation; this will copy the output
    // requested region to the input requested region.
    Superclass::GenerateInputRequestedRegion();

    // Get pointer to the input images:
    Input1ImageType::Pointer ptr1 = this->GetInput1();
    Input2ImageType::Pointer ptr2 = this->GetInput2();

    OutputImageType::Pointer outputPtr;

    // Make sure we can procede
    if (!ptr1 || !ptr2)
    {
        Logger::logWarn("CLGOpticFlowImageFilter::GenerateInputRequestedRegion(): One or more input images was not set!");
        return;
    }
    //else
    //{
    //    // Generate the output image
    //    outputPtr = OutputImageType::New();
    //    OutputImageType::RegionType region;
    //    OutputImageType::RegionType::IndexType idx;
    //    OutputImageType::RegionType::SizeType size;
    //    for (int d = 0; d < ImageDimension; d++)
    //    {
    //        idx[d] = ptr1->GetLargestPossibleRegion().GetIndex()[d];
    //        size[d] = ptr1->GetLargestPossibleRegion().GetSize()[d];
    //    }
    //    region.SetIndex(idx);
    //    region.SetSize(size);
    //    outputPtr->SetRegions(region);
    //    outputPtr->Allocate();
    //    this->SetOutput(outputPtr);
    //}

    // Get a copy of the input requested region
    Input1ImageType::RegionType region = ptr1->GetRequestedRegion();
    
    // Pad by one
    region.PadByRadius(1);

    // Crop the requested region at the input's largest possible region
    if (region.Crop(ptr1->GetLargestPossibleRegion()))
    {
        ptr1->SetRequestedRegion(region);
        ptr2->SetRequestedRegion(region);
    }
    else
    {
        // Couldn't crop the region; throw an exception.
        ptr1->SetRequestedRegion(region);

        itk::InvalidRequestedRegionError e(__FILE__, __LINE__);
        itk::OStringStream msg;
        msg << static_cast<const char *>(this->GetNameOfClass())
            << "::GenerateInputRequestedRegion()";
        e.SetLocation(msg.str().c_str());
        e.SetDescription("Requested region is (at least partially) outside the largest possible region.");
        e.SetDataObject(ptr1);
        throw e;
    }
    
    Logger::logDebug("CLGOpticFlowImageFilter::GenerateInputRequestedRegion() done.");
}

template <class TInputImage1, class TInputImage2, class TOutputValueType>
void CLGOpticFlowImageFilter<TInputImage1, TInputImage2, TOutputValueType>
::BeforeGenerateData()
{
    Logger::logDebug("CLGOpticFlowImageFilter::BeforeGenerateData()");
    // Build the three dimensional gradient and structure tensor 
    // of the input images
    typedef itk::Image<InternalValueType, ImageDimension> DerivativeImageType;
    typedef itk::RecursiveGaussianImageFilter<Input1ImageType, DerivativeImageType> GaussianFilterType;
    typedef itk::SubtractImageFilter<Input1ImageType, Input2ImageType, DerivativeImageType> DifferenceFilterType;
    typedef itk::ImageDuplicator<DerivativeImageType> DuplicatorType;

    Logger::logDebug("\tSetting up derivative filters...");    
    // Setup Derivative filters
    GaussianFilterType::Pointer dGx = GaussianFilterType::New();
    GaussianFilterType::Pointer dGy = GaussianFilterType::New();
    DuplicatorType::Pointer copier = DuplicatorType::New();
    DifferenceFilterType::Pointer dIt = DifferenceFilterType::New();

    dGx->SetDirection(0);
    dGy->SetDirection(1);
    dGx->SetSigma(this->GetSpatialSigma());
    dGy->SetSigma(this->GetSpatialSigma());

    // Setup derivative pipeline
    dGx->SetInput(this->GetInput1());
    dGy->SetInput(dGx->GetOutput());
    copier->SetInputImage(dGy->GetOutput());

    // Compute derivatives
    Logger::logDebug("\tComputing derivatives...");
    DerivativeImageType::Pointer dx, dy, dt;
    // dx = dGx * I1; derivative in x, smooth in y
    dGx->SetFirstOrder();
    dGy->SetZeroOrder();
    dGy->Update();
    copier->Update();
    dx = copier->GetOutput();

    // dy = dGy * I1; smooth in x, derivative in y
    dGx->SetZeroOrder();
    dGy->SetFirstOrder();
    dGy->Update();
    copier->Update();
    dy = copier->GetOutput();

    // Smooth I1 and I2 prior to differencing
    // dt = I2 - I1
    Logger::logDebug("\tCreating temporal difference...");
    dGx->SetZeroOrder();
    dGy->SetZeroOrder();
    dGy->Update();
    copier->Update();
    dIt->SetInput1(copier->GetOutput());

    dGx->SetInput(this->GetInput2());
    dGy->Update();
    copier->Update();
    dIt->SetInput2(copier->GetOutput());

    dIt->Update();
    dt = dIt->GetOutput();

    // Compute the scaling factor for the structure tensor.
    // pixel area / regularization
    Input1ImageType::SpacingType spacing = this->GetInput1()->GetSpacing();
    double area = 1;
    for (int i = 0; i < ImageDimension; i++)
    {
        area *= spacing[i];
    }

    double factor = area / this->GetRegularization();
    
    // Compose the structure tensor from the image gradient.
    Logger::logDebug("\tCreating structure tensor...");
    this->m_J = TensorImageType::New();
    TensorImageType::RegionType region;
    TensorImageType::RegionType::SizeType size;
    TensorImageType::RegionType::IndexType idx;
    for (int d = 0; d < ImageDimension; d++)
    {
        size[d] = this->GetInput1()->GetLargestPossibleRegion().GetSize()[d];
        idx[d] = this->GetInput1()->GetLargestPossibleRegion().GetIndex()[d];
    }
    region.SetSize(size);
    region.SetIndex(idx);
    this->m_J->SetRegions(region);
    this->m_J->Allocate();

    typedef itk::ImageRegionIterator<TensorImageType> TensorIteratorType;
    typedef itk::ImageRegionConstIterator<DerivativeImageType> DerivativeIteratorType;
    DerivativeIteratorType dxIt(dx, dx->GetLargestPossibleRegion());
    DerivativeIteratorType dyIt(dy, dy->GetLargestPossibleRegion());
    DerivativeIteratorType dtIt(dt, dt->GetLargestPossibleRegion());
    TensorIteratorType jIt(this->m_J, this->m_J->GetLargestPossibleRegion());

    Logger::logDebug("\tComputing structure tensor...");
    TensorType tensor;
    for (dxIt.GoToBegin(), dyIt.GoToBegin(), dtIt.GoToBegin(), jIt.GoToBegin();
        !(dxIt.IsAtEnd() || dyIt.IsAtEnd() || dtIt.IsAtEnd() || jIt.IsAtEnd());
        ++dxIt, ++dyIt, ++dtIt, ++jIt)
    {
        // First row
        tensor[0] = factor * dxIt.Get() * dxIt.Get();
        tensor[1] = factor * dxIt.Get() * dyIt.Get();
        tensor[2] = factor * dxIt.Get() * dtIt.Get();

        // Second row
        tensor[3] = factor * dyIt.Get() * dxIt.Get();
        tensor[4] = factor * dyIt.Get() * dyIt.Get();
        tensor[5] = factor * dyIt.Get() * dtIt.Get();

        jIt.Set(tensor);
    }

    Logger::logDebug("CLGOpticFlowImageFilter::BeforeGenerateData() done.");
}

template <class TInputImage1, class TInputImage2, class TOutputValueType>
void CLGOpticFlowImageFilter<TInputImage1, TInputImage2, TOutputValueType>
::GenerateData()
{
    // Convert to treaded
    Logger::logDebug("CLGOpticFlowImageFilter::GenerateData()");
    this->LogSelf();

    // Allocate the output buffer
    Logger::logDebug("\tAllocating output buffer...");
    Superclass::AllocateOutputs();
    OutputImageType::Pointer output = this->GetOutput();
    OutputImageType::RegionType outRegion = output->GetRequestedRegion();

    // Initialize output to zero flow field
    Logger::logDebug("\tInitializing flow field...");
    typedef itk::ImageRegionIterator<OutputImageType> OutputIteratorType;
    OutputIteratorType outIt(output, outRegion);
    VectorType vector;
    vector.Fill(0);
    for (outIt.GoToBegin(); !outIt.IsAtEnd(); ++outIt)
    {
        outIt.Set(vector);
    }

    // Call BeforeGenerateData to create the structure tensor
    this->BeforeGenerateData();

    Logger::logDebug("\tSetting up iterators...");
    // Setup neighborhood iterator
    typedef itk::NeighborhoodIterator<OutputImageType> NeighborhoodIteratorType;
    NeighborhoodIteratorType::RadiusType radius;
    radius.Fill(1);
    NeighborhoodIteratorType nIt(radius, output, outRegion);
    NeighborhoodIteratorType::OffsetType left =  {{-1, 0}};
    NeighborhoodIteratorType::OffsetType right = {{+1, 0}};
    NeighborhoodIteratorType::OffsetType above = {{0, -1}};
    NeighborhoodIteratorType::OffsetType below = {{0, +1}};
    const unsigned int nCount = 4; // number of neighbors

    // Setup structure tensor iterator
    typedef itk::ImageRegionConstIterator<TensorImageType> TensorIterator;
    TensorImageType::RegionType tensorRegion;
    TensorImageType::RegionType::SizeType size;
    TensorImageType::RegionType::IndexType index;
    for (int d = 0; d < ImageDimension; d++)
    {
        size[d] = outRegion.GetSize()[d];
        index[d] = outRegion.GetIndex()[d];
    }
    tensorRegion.SetSize(size);
    tensorRegion.SetIndex(index);
    TensorIterator jIt(this->m_J, tensorRegion);

    // indices into structure tensor
    const unsigned int j11 = 0, j12 = 1, j13 = 2, j21 = 3, j22 = 4, j23 = 5;
    
    // SOR constants
    const double w = this->m_Relaxation;
    const double one_w = 1 - w;

    VectorType curr, next;

    // Iteratively calculate flow field
    itk::ProgressReporter progress(this, 0, this->m_Iterations);
    Logger::logDebug("\tCalculating flow field...");
    for (int i = 0; i < this->m_Iterations; i++)
    {
        // Update each pixel
        for (nIt.GoToBegin(), jIt.GoToBegin();
            !(nIt.IsAtEnd() || jIt.IsAtEnd());
            ++nIt, ++jIt)
        {
            TensorType J = jIt.Get();

            InternalValueType sumU = 
                nIt.GetPixel(left)[0] + nIt.GetPixel(right)[0] + 
                nIt.GetPixel(above)[0] + nIt.GetPixel(below)[0];
            InternalValueType sumV =
                nIt.GetPixel(left)[1] + nIt.GetPixel(right)[1] + 
                nIt.GetPixel(above)[1] + nIt.GetPixel(below)[1];
            
            curr = nIt.GetCenterPixel();
            next[0] = one_w*curr[0] + w * (sumU - (J[j12]*curr[1] + J[j13]))/(nCount + J[j11]);
            next[1] = one_w*curr[1] + w * (sumV - (J[j21]*next[0] + J[j23]))/(nCount + J[j22]);
            nIt.SetCenterPixel(next);
        }
        progress.CompletedPixel();
    }

    // Call AfterGenerateData to calculate the confidence image.
    this->AfterGenerateData();

    Logger::logDebug("\tGrafting output...");
    this->GraftOutput(output);
    Logger::logDebug("CLGOpticFlowImageFilter::GenerateData() done.");
}

template <class TInputImage1, class TInputImage2, class TOutputValueType>
void CLGOpticFlowImageFilter<TInputImage1, TInputImage2, TOutputValueType>
::AfterGenerateData()
{
    Logger::logDebug("CLGOpticFlowImageFilter::AfterGenerateData()");

    // Clean up the structure tensor
    this->m_J = NULL;

    // Warp input image 1 with the flow field
    
    // Create an error image: the energy difference between
    // each pixel in the warped image and image 2.
    Logger::logDebug("CLGOpticFlowImageFilter::AfterGenerateData() done.");
}

template <class TInputImage1, class TInputImage2, class TOutputValueType>
void CLGOpticFlowImageFilter<TInputImage1, TInputImage2, TOutputValueType>
::PrintSelf(std::ostream& os, itk::Indent indent) const
{
    Superclass::PrintSelf(os, indent);
    os << indent << "SpatialSigma: " << this->m_SpatialSigma << std::endl;
    os << indent << "Regularization: " << this->m_Regularization << std::endl;
    os << indent << "Relaxation: " << this->m_Relaxation << std::endl;
    os << indent << "Iterations: " << this->m_Iterations << std::endl;
}

template <class TInputImage1, class TInputImage2, class TOutputValueType>
void CLGOpticFlowImageFilter<TInputImage1, TInputImage2, TOutputValueType>
::LogSelf()
{
    char text[80];
    sprintf(text, "SpatialSigma: %5.2f", this->GetSpatialSigma()); 
    Logger::logInfo(text);
    sprintf(text, "Regularization: %5.2f", this->GetRegularization());
    Logger::logInfo(text);
    sprintf(text, "Relaxation: %5.2f", this->GetRelaxation());
    Logger::logInfo(text);
    sprintf(text, "Iterations: %i", this->GetIterations());
    Logger::logInfo(text);
}
