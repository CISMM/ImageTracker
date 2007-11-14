#pragma once

#include "itkImage.h"
#include "itkVector.h"

#include "CLGOpticalFlowIterativeStepImageFilter.h"
#include "OpticalFlowImageFilter.h"
#include "StructureTensorImageFilter.h"

template <class TInputImage1, class TInputImage2, class TOutputValueType = float >
class CLGOpticFlowImageFilter :
    public OpticalFlowImageFilter< TInputImage1, TInputImage2, TOutputValueType >
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
    typedef itk::Vector<OutputValueType, ImageDimension> OutputPixelType;
    typedef itk::Image<OutputPixelType, ImageDimension> OutputImageType;
    typedef typename InputImageType::Pointer InputImagePointer;
    typedef typename OutputImageType::Pointer OutputImagePointer;
    typedef typename OutputImageType::RegionType OutputImageRegionType;
    typedef OutputPixelType VectorType;

    /** Internal processing types */
    typedef float InternalValueType;
    typedef StructureTensorImageFilter< Input1ImageType > TensorFilterType;
    typedef typename TensorFilterType::TensorImageType TensorImageType;
    typedef CLGOpticalFlowIterativeStepImageFilter< OutputImageType > IterativeStepType;

    /** Standard itk class typedefs */
    typedef CLGOpticFlowImageFilter Self;
    typedef OpticalFlowImageFilter<Input1ImageType, Input2ImageType, OutputValueType > Superclass;
    typedef itk::SmartPointer<Self> Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;

    /** itk New factory method and type info. */
    itkNewMacro(Self);
    itkTypeMacro(CLGOpticFlowImageFilter, OpticalFlowImageFilter);

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
    CLGOpticFlowImageFilter() :
        m_SpatialSigma(1.0),
        m_IntegrationSigma(4.0),
        m_Regularization(200),
        m_Relaxation(1.9),
        m_Iterations(200)
    {}
    
    virtual ~CLGOpticFlowImageFilter() {}

    void PrintSelf(std::ostream& os, itk::Indent indent) const;
    void LogSelf();

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
    // Not implemented
    CLGOpticFlowImageFilter(const Self& other);
    void operator=(const Self& other);
    
    // Algorithm terms.
    double m_SpatialSigma;
    double m_IntegrationSigma;
    double m_Regularization;
    double m_Relaxation;
    unsigned int m_Iterations;
};

/************************************************************************/
/* Implementation                                                       */
/************************************************************************/

#include "ImageUtils.h"
#include "Logger.h"

template <class TInputImage1, class TInputImage2, class TOutputValueType>
void CLGOpticFlowImageFilter<TInputImage1, TInputImage2, TOutputValueType>
::GenerateData()
{
    std::string function("CLGOpticFlowImageFilter::GenerateData");
    Logger::debug << function << ": entering" << std::endl;
    this->LogSelf();

    // Allocate the output buffer
    Logger::debug << function << ": allocating output buffer" << std::endl;
    Superclass::AllocateOutputs();

    // Create the image structure tensor
    Logger::debug << function << ": computing image structure tensor" << std::endl;
    typename TensorFilterType::Pointer tensor = TensorFilterType::New();
    tensor->SetSpatialSigma(this->GetSpatialSigma());
    tensor->SetIntegrationSigma(this->GetIntegrationSigma());
    tensor->SetInput1(this->GetInput1());
    tensor->SetInput2(this->GetInput2());
    tensor->Update();
    
    // Set up the iterative step filter
    Logger::debug << function << ": setting up interative step filter" << std::endl;
    typename IterativeStepType::Pointer step = IterativeStepType::New();
    step->SetStructureTensor(tensor->GetOutput());
    step->SetRegularization(this->GetRegularization());
    step->SetRelaxation(this->GetRelaxation());
    // step->SetNumberOfThreads(4);
    Logger::debug << function << ": iterative step filter threads: " << step->GetNumberOfThreads() << std::endl;

    // Initialize output to zero flow field	
    Logger::debug << function << ": initializing flow field" << std::endl;
    typename OutputImageType::Pointer output = OutputImageType::New();
    output->SetRegions(this->GetOutput()->GetLargestPossibleRegion());
    output->SetSpacing(this->GetOutput()->GetSpacing());
    output->SetOrigin(this->GetOutput()->GetOrigin());
    output->Allocate();
    typename OutputImageType::PixelType zero;
    zero.Fill(0);
    output->FillBuffer(zero);
    PrintImageInfo<OutputImageType>(output, "Initial flow");

    // Iteratively calculate flow field
    Logger::debug << function << ": caluculating flow field" << std::endl;
    for (int i = 0; i < this->m_Iterations; i++)
    {
        step->SetInput(output);
        step->Update();
        output = step->GetOutput();
        output->DisconnectPipeline();

	char msg[80];
	sprintf(msg, "Flow after step %d", i);
	PrintImageInfo<OutputImageType>(output, std::string(msg));
    }

    // Call AfterGenerateData to calculate the confidence image.
    this->AfterGenerateData();

    Logger::debug << function << ": grafting output" << std::endl;
    this->GraftOutput(output);
    Logger::debug << function << ": done" << std::endl;
}

template <class TInputImage1, class TInputImage2, class TOutputValueType>
void CLGOpticFlowImageFilter<TInputImage1, TInputImage2, TOutputValueType>
::AfterGenerateData()
{
    // TODO: Warp input image 1 with the flow field and 
    // create an error image: the energy difference between
    // each pixel in the warped image and image 2.
}

template <class TInputImage1, class TInputImage2, class TOutputValueType>
void CLGOpticFlowImageFilter<TInputImage1, TInputImage2, TOutputValueType>
::PrintSelf(std::ostream& os, itk::Indent indent) const
{
    Superclass::PrintSelf(os, indent);
    os << indent << "SpatialSigma: " << this->m_SpatialSigma << std::endl;
    os << indent << "IntegrationSigma: " << this->m_IntegrationSigma << std::endl;
    os << indent << "Regularization: " << this->m_Regularization << std::endl;
    os << indent << "Relaxation: " << this->m_Relaxation << std::endl;
    os << indent << "Iterations: " << this->m_Iterations << std::endl;
}

template <class TInputImage1, class TInputImage2, class TOutputValueType>
void CLGOpticFlowImageFilter<TInputImage1, TInputImage2, TOutputValueType>
::LogSelf()
{
    char text[80];
    sprintf(text, "SpatialSigma:        %5.2f", this->GetSpatialSigma());
    Logger::logInfo(text);
    sprintf(text, "IntegrationSigma:    %5.2f", this->GetIntegrationSigma());
    Logger::logInfo(text);
    sprintf(text, "Regularization:      %5.2f", this->GetRegularization());
    Logger::logInfo(text);
    sprintf(text, "Relaxation:          %5.2f", this->GetRelaxation());
    Logger::logInfo(text);
    sprintf(text, "Iterations:          %i", this->GetIterations());
    Logger::logInfo(text);
}
