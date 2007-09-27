#pragma once

#include "itkImageToImageFilter.h"

/**
 * /class RungeKuttaSolver
 * /brief Estimates the value of a function at some timestep from an 
 * initial value, given that function's derivatives.
 * 
 * Solves the differential equation dg(x)/dt = f(t) for g(x).
 * Here, the function g(x) and derivatives are defined over the image 
 * domain, so the differential equation is solved at each image location.
 * We assume that the pixel type of the input image and derivative image
 * is a vector type.  Additionally, the derivative image type should have
 * one more dimension than the input image...that is, the direction of
 * integration.
 * 
 * For example, assume g(x) is a time-varying vector field.  The vector
 * field is defined over R2, so g(x) is a 3D image, with 2-component vector
 * pixels.  The input initial condition image should be a 2D image with 
 * 2-component vector pixels.
 */
template < class TInput, class TOutput, class TDerivative >
class RungeKuttaSolver :
    public itk::ImageToImageFilter< TInput, TOutput >
{
public:
    // Helpful typedefs
    typedef TInput InputImageType;
    typedef TOutput OutputImageType;
    typedef TDerivative DerivativeType;
    typedef typename InputImageType::Pointer InputPointerType;
    typedef typename OutputImageType::Pointer OutputPointerType;
    typedef typename DerivativeType::ConstPointer DerivativePointerType;
    
    // Common ITK typedefs
    typedef RungeKuttaSolver Self;
    typedef itk::ImageToImageFilter< InputImageType, OutputImageType > Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    
    itkNewMacro(Self);
    itkTypeMacro(RungeKuttaSolver, ImageToImageFilter);
    itkStaticConstMacro(ImageDimension, unsigned int, InputImageType::ImageDimension);
    
    /**
     * Get/Set the value of the derivative function.
     */
    itkGetConstObjectMacro(Derivative, DerivativeType);
    itkSetConstObjectMacro(Derivative, DerivativeType);
    
    /**
     * Get/Set the size of the time step to take.
     */
    itkGetMacro(StepSize, float);
    itkSetMacro(StepSize, float);
    
    itkGetMacro(StartTime, float);
    itkSetMacro(StartTime, float);
    
protected:
    RungeKuttaSolver()
        : m_StepSize(1),
          m_StartTime(0)
    {
    }
    
    virtual ~RungeKuttaSolver(){}
    
    /**
     * Integrate one step of a differential equation estimate.
     */
    void GenerateData();
    
private:
    // purposefully not implemented
    RungeKuttaSolver(const Self& other);
    void operator=(const Self& other);
    
    float m_StepSize;
    float m_StartTime;
    DerivativePointerType m_Derivative;
    
};

/** Implementation **/

#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkVectorLinearInterpolateImageFunction.h"

template < class TInput, class TOutput, class TDerivative >
void RungeKuttaSolver< TInput, TOutput, TDerivative >
::GenerateData()
{
    std::string function("RungeKuttaSolver::GenerateData");
    Logger::verbose << function << ": time: " << this->GetStartTime() << "\tstep: " << this->GetStepSize() << std::endl;
    
    // Typedefs
    typedef itk::VectorLinearInterpolateImageFunction< DerivativeType > InterpolateType;
    typedef itk::ImageRegionConstIterator< InputImageType > InputIteratorType;
    typedef itk::ImageRegionIterator< OutputImageType > OutputIteratorType;
    
    typedef typename InputImageType::PixelType InputPixelType;
    typedef typename OutputImageType::PixelType OutputPixelType;
    typedef typename InterpolateType::PointType DerivativePointType;
    typedef typename InterpolateType::OutputType DerivativeValueType;
    
    // Allocate output buffer
    // Logger::verbose << function << ": Allocating output" << std::endl;
    Superclass::AllocateOutputs();
    
    // Grab inital image and final image
    // Logger::verbose << function << ": Retreiving input" << std::endl;
    InputPointerType input = const_cast<InputImageType *>(this->GetInput());
    OutputPointerType output = this->GetOutput();
    
    if (!input || input.IsNull() || !output || output.IsNull())
    {
        Logger::error << function << ": Input or output not properly allocated" << std::endl;
        return;
    }
    
    // Set up derivative interpolation
    // Logger::verbose << function << ": Setting up interpolator" << std::endl;
    typename DerivativeType::SizeType dsize = this->GetDerivative()->GetLargestPossibleRegion().GetSize();
    // Logger::verbose << function << ": Derivative size: " << dsize[0] << ", " << dsize[1] << ", " << dsize[2] << std::endl;
    typename InterpolateType::Pointer interpolate = InterpolateType::New();
    interpolate->SetInputImage(this->GetDerivative());
    
    // Determine the region over which to solve the differential equation
    // Logger::verbose << function << ": Setting up iterators" << std::endl;
    typename OutputImageType::RegionType outRegion = output->GetRequestedRegion();
    InputIteratorType inIt(input, outRegion);
    OutputIteratorType outIt(output, outRegion);
    
    // PrintImageInfo<InputImageType>(input, "Initial conditions");
    // PrintRegionInfo<OutputImageType>(outRegion, "Requested region");

    // Helpful numbers for RK4 algorithm
    float halfStep = 0.5 * this->GetStepSize();
    float oneSixth = 1.0 / 6.0;
    float oneThird = 1.0 / 3.0;
    unsigned int pointDim = DerivativePointType::PointDimension;
    unsigned int d;
    // Logger::verbose << function << ": Point dimension => " << pointDim << std::endl;
    
    // We sometimes need a zero-value derivative for areas where the integration has pushed
    // sample points outside of the derivative image buffer.
    // TODO: Is there a more graceful and efficient way of doing this?  We could maybe pad the
    // input derivative image, but how much would we pad it?
    DerivativeValueType zero;
    for (d = 0; d < pointDim-1; d++)
        zero[d] = 0;

    // Compute RK4 at each image location
    for (inIt.GoToBegin(), outIt.GoToBegin();
         !(inIt.IsAtEnd() || outIt.IsAtEnd());
         ++inIt, ++outIt)
    {
        // xn is the 2D starting point
        InputPixelType xn = inIt.Get();
        // location at which to find the derivative;
        DerivativePointType dPoint;
        // value of the derivative at that point
        DerivativeValueType dValue;
        
        OutputPixelType k1, k2, k3, k4, outPixel;
        
        // Logger::verbose << "Pixel (" << xn[0] << "," << xn[1] << "), Time " << this->GetStartTime() << std::endl;
        
        // Note in the following that we compute each k-value
        // while we set up the position at which to find the
        // derivative for the next k-value.  We do this to
        // minimize the number of for loops needed.
        for (d = 0; d < pointDim-1; d++)
        {
            dPoint[d] = xn[d];
        }
        dPoint[pointDim-1] = this->GetStartTime();
        
        dValue = interpolate->IsInsideBuffer(dPoint) ? interpolate->Evaluate(dPoint) : zero;
        // Logger::verbose << function << ": Deriv (" << dPoint[0] << ", " << dPoint[1] << ", " << dPoint[2] << ") = (" << dValue[0] << ", " << dValue[1] << ")" << std::endl;
        
        for (d = 0; d < pointDim-1; d++)
        {
            k1[d] = this->GetStepSize() * dValue[d];
            dPoint[d] = xn[d] + 0.5 * k1[d];
        }
        dPoint[pointDim-1] = this->GetStartTime() + halfStep;
        dValue = interpolate->IsInsideBuffer(dPoint) ? interpolate->Evaluate(dPoint) : zero;
        
        // Logger::verbose << function << ": Deriv (" << dPoint[0] << ", " << dPoint[1] << ", " << dPoint[2] << ") = (" << dValue[0] << ", " << dValue[1] << ")" << std::endl;
        
        for (d = 0; d < pointDim-1; d++)
        {
            k2[d] = this->GetStepSize() * dValue[d];
            dPoint[d] = xn[d] + 0.5 * k2[d];
        }
        dValue = interpolate->IsInsideBuffer(dPoint) ? interpolate->Evaluate(dPoint) : zero;
        
        // Logger::verbose << function << ": Deriv (" << dPoint[0] << ", " << dPoint[1] << ", " << dPoint[2] << ") = (" << dValue[0] << ", " << dValue[1] << ")" << std::endl;
        
        for (d = 0; d < pointDim-1; d++)
        {
            k3[d] = this->GetStepSize() * dValue[d];
            dPoint[d] = xn[d] + k3[d];
        }
        dPoint[pointDim-1] = this->GetStartTime() + this->GetStepSize();
        dValue = interpolate->IsInsideBuffer(dPoint) ? interpolate->Evaluate(dPoint) : zero;
        
        // Logger::verbose << function << ": Deriv (" << dPoint[0] << ", " << dPoint[1] << ", " << dPoint[2] << ") = (" << dValue[0] << ", " << dValue[1] << ")" << std::endl;
        
        for (d = 0; d < pointDim-1; d++)
        {
            k4[d] = this->GetStepSize() * dValue[d];
            outPixel[d] = xn[d] + oneSixth * (k1[d] + k4[d]) + oneThird * (k2[d] + k3[d]);
        }
        
        // Logger::verbose << "Deriv (" << dPoint[0] << ", " << dPoint[1] << ", " << dPoint[2] << ") = (" << dValue[0] << ", " << dValue[1] << ")" << std::endl;
        outIt.Set(outPixel);
    }
    
    // Logger::verbose << function << ": Grafting output" << std::endl;
    this->GraftOutput(output);
}
