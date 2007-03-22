#pragma once

#include "itkImage.h"
#include "itkImageToImageFilter.h"
#include "itkVector.h"

/**
 * \class HornOpticalFlowImageFilter
 * \brief Implements the Horn & Schunck 1981 Computation of Optical Flow algorithm.
 * 
 * This algorithm is based on the image constancy equation, which attributes changing intensities in
 * an image sequence to motion in the scene.  Assuming a single Lambertian surface,
 * I(x,y,t) = I(x+dx, y+dy, t+dt)
 * u E_x + v E_y + E_t = 0
 * 
 * This equation is underconstrained; Horn and Schunck apply a global smoothness criterion, where 
 * discontinuities in the Laplacian of the flow field (u,v) are penalized.  The smoothness weighting
 * enables adjusting how strongly this criterion is enforced.
 *
 * Spatial image derivatives are estimated using derivative-of-Gaussian filter kernels.  The temporal
 * derivatives are estimated by image differencing.  The user configures the spatial derivative scale,
 * the weighting of the smoothness term, and the number of iterations.
 */
template < class TInputImage1, class TInputImage2 = TInputImage1, class TComponentType = float >
class HornOpticalFlowImageFilter :
    public itk::ImageToImageFilter< TInputImage1, 
        itk::Image< itk::Vector< TComponentType, itk::GetImageDimension<TInputImage1>::ImageDimension >, itk::GetImageDimension<TInputImage1>::ImageDimension > >
{
public:
    /** Some convenient typedefs */
    typedef TInputImage1 Input1ImageType;
    typedef typename Input1ImageType::ConstPointer Input1ImagePointer;
    typedef typename Input1ImageType::RegionType Input1RegionType;
    typedef typename Input1ImageType::PixelType Input1PixelType;
    typedef TInputImage2 Input2ImageType;
    typedef typename Input2ImageType::ConstPointer Input2ImagePointer;
    typedef typename Input2ImageType::RegionType Input2RegionType;
    typedef typename Input2ImageType::PixelType Input2PixelType;
    
    itkStaticConstMacro(ImageDimension, unsigned int, Input1ImageType::ImageDimension);
    typedef typename Input1ImageType::PixelType InputPixelType;
    
    typedef itk::Vector< TComponentType, ImageDimension > OutputPixelType;
    typedef itk::Image< OutputPixelType, ImageDimension > OutputImageType;
    typedef typename OutputImageType::Pointer OutputImagePointer;
    typedef typename OutputImageType::RegionType OutputRegionType;

    /** Common ITK Typedefs */
    typedef HornOpticalFlowImageFilter Self;
    typedef itk::ImageToImageFilter< Input1ImageType, OutputImageType > Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    itkNewMacro(Self);
    itkTypeMacro(HornOpticalFlowImageFilter, itk::ImageToImageFilter);
    
    TInputImage1 * GetInput1()
    {
        return const_cast<TInputImage1 *> (this->GetInput(0));
    }

    /** 
     * Set the first (moving) image for optical flow. Note, the image
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
     * Set the second (fixed) image for optical flow. Note, the image
     * data must be loaded (updated) before this filter's Update() 
     * method is called.
     */
    void SetInput2(const TInputImage2 * image2)
    {
        this->SetNthInput(1, const_cast<TInputImage2 *>(image2));
    }
    
    /**
     * Get/Set the spatial scale, sigma, over which to compute image derivatives.
     */
    itkGetMacro(SpatialSigma, float);
    itkSetMacro(SpatialSigma, float);
    
    /**
     * Get/Set the smoothness weigting; the regularization term that weights how
     * smooth the computed flow should be.
     */
    itkGetMacro(SmoothWeighting, float);
    itkSetMacro(SmoothWeighting, float);
    
    /**
     * Get/Set the number of iterations to use in flow computation.
     */
    itkGetMacro(Iterations, unsigned int);
    itkSetMacro(Iterations, unsigned int);
    
protected:
    HornOpticalFlowImageFilter()
    : m_SpatialSigma(1.0),
      m_SmoothWeighting(100),
      m_Iterations(200)
    {}
    virtual ~HornOpticalFlowImageFilter(){}
    
    /**
     * Compute optical flow.
     */
    void GenerateData();
    
private:
    // Not implemented
    HornOpticalFlowImageFilter(const Self& other);
    void operator=(const Self& other);
    
    float m_SpatialSigma;
    float m_SmoothWeighting;
    unsigned int m_Iterations;
};

/** Implementation **/

#include <string>

#include "itkImageRegionIterator.h"
#include "itkRecursiveGaussianImageFilter.h"
#include "itkSubtractImageFilter.h"

#include "HornOpticalFlowIterativeStepImageFilter.h"
#include "ImageUtils.h"
#include "Logger.h"

template < class TInputImage1, class TInputImage2, class TComponentType >
void HornOpticalFlowImageFilter< TInputImage1, TInputImage2, TComponentType >
::GenerateData()
{
    std::string function("HornOpticalFlowImageFilter::GenerateData");
    
    Logger::debug << function << ": Checking input" << std::endl;
    Input1ImagePointer input1 = this->GetInput1();
    Input2ImagePointer input2 = this->GetInput2();
    if (!input1 || !input2)
    {
        Logger::warning << function << ": one of the input images was not set...aborting" << std::endl;
        return;
    }
    
    Logger::debug << function << ": Allocating output buffer..." << std::endl;
    Superclass::AllocateOutputs();
    OutputImagePointer output = this->GetOutput();
    
    Logger::debug << function << ": Setting up derivative pipelines" << std::endl;
    // Pipeline typedefs
    typedef itk::Image< float, 2 > InternalImageType;
    typedef itk::RecursiveGaussianImageFilter< Input1ImageType, InternalImageType > SpatialDerivFilter;
    typedef itk::SubtractImageFilter< Input2ImageType, Input1ImageType, InternalImageType > TemporalDerivFilter;
    typedef HornOpticalFlowIterativeStepImageFilter< OutputImageType, InternalImageType > IterativeStepFilter;
    
    typename SpatialDerivFilter::Pointer dx = SpatialDerivFilter::New();
    typename SpatialDerivFilter::Pointer dy = SpatialDerivFilter::New();
    typename TemporalDerivFilter::Pointer dt = TemporalDerivFilter::New();
    
    // Set parameters
    dx->SetDirection(0);
    dy->SetDirection(1);
    dx->SetSigma(this->GetSpatialSigma());
    dy->SetSigma(this->GetSpatialSigma());
    
    // Connect pipeline
    dx->SetInput(input1);
    dy->SetInput(dx->GetOutput());
    
    // Smooth the input images and compute spatial and temporal derivatives
    Logger::debug << function << ": Computing derivative images" << std::endl;
    InternalImageType::Pointer dIdx, dIdy, gI1, gI2;
    
    // dI/dx
    dx->SetFirstOrder();
    dy->SetZeroOrder();
    dy->Update();
    dIdx = dy->GetOutput();
    dIdx->DisconnectPipeline();
    
    // dI/dy
    dx->SetZeroOrder();
    dy->SetFirstOrder();
    dy->Update();
    dIdy = dy->GetOutput();
    dIdy->DisconnectPipeline();
    
    // smooth image 1
    dx->SetZeroOrder();
    dy->SetZeroOrder();
    dy->Update();
    gI1 = dy->GetOutput();
    gI1->DisconnectPipeline();
    
    // smooth image 2
    dx->SetInput(input2);
    dy->Update();
    gI2 = dy->GetOutput();
    gI2->DisconnectPipeline();
    
    // temporal derivative I2 - I1
    dt->SetInput1(gI2);
    dt->SetInput2(gI1);
    dt->Update();
    
    // Write out derivative images, for debugging
    typedef itk::Image< unsigned short, 2 > WriteImageType;
    WriteImage< InternalImageType, WriteImageType >(dIdx, "dIdx.tiff");
    WriteImage< InternalImageType, WriteImageType >(dIdy, "dIdy.tiff");
    WriteImage< InternalImageType, WriteImageType >(dt->GetOutput(), "dIdt.tiff");
    
    Logger::debug << function << ": Initializing input flow field" << std::endl;
    OutputImagePointer flow = OutputImageType::New();
    flow->SetRegions(output->GetLargestPossibleRegion());
    flow->Allocate();
    
    OutputPixelType zero;
    zero.Fill(0);
    typedef itk::ImageRegionIterator< OutputImageType > FlowIterator;
    FlowIterator fit(flow, output->GetLargestPossibleRegion());
    for (fit.GoToBegin(); !fit.IsAtEnd(); ++fit)
    {
        fit.Set(zero);
    }
    
    Logger::debug << function << ": Setting up step iterator" << std::endl;
    typename IterativeStepFilter::Pointer step = IterativeStepFilter::New();
    step->SetSmoothWeighting(this->GetSmoothWeighting());
    step->SetDerivativeX(dIdx);
    step->SetDerivativeY(dIdy);
    step->SetDerivativeT(dt->GetOutput());
    
    Logger::debug << function << ": Computing flow" << std::endl;
    for (unsigned int i = 0; i < this->GetIterations(); i++)
    {
        step->SetInput(flow);
        step->Update();
        flow = step->GetOutput();
        flow->DisconnectPipeline();
        
        // Write intermediate image to watch flow evolve
//         char file[50];
//         sprintf(file, "step-%04d.mha", i);
//         WriteImage<OutputImageType>(flow, std::string(file));
    }
    
    Logger::debug << function << ": Grafting output" << std::endl;
    this->GraftOutput(flow);
}
