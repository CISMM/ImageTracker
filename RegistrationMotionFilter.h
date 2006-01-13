#pragma once

#include "itkImage.h"
#include "itkImageFileWriter.h"
#include "itkImageRegistrationMethod.h"
#include "itkImageToImageFilter.h"
#include "itkInPlaceImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkNormalizedCorrelationImageToImageMetric.h"
#include "itkRegularStepGradientDescentOptimizer.h"
#include "itkTranslationTransform.h"
#include "itkVector.h"

/**
 * RegistrationMotionFilter determines motion information by registering image patches
 * from two input images.  The output from this filter is a vector image where each
 * vector pixel is the motion of a patch centered at that location from Image1 to the
 * best-matched patch on Image2.  Filter configuration allows one to choose an image
 * patch radius as well as maximum motion displacement.  Motion is calculated at
 * each image location.
 *
 * Note: the OutputValueType specifies the precision of the transform used in
 * registration.  It seems that only double precision works--using float results
 * in compilation errors.
 */
template <class TInputImage1, class TInputImage2, class TOutputValueType = double>
class RegistrationMotionFilter :
    public itk::ImageToImageFilter< TInputImage1,
        itk::Image< itk::Vector <TOutputValueType, itk::GetImageDimension<TInputImage1>::ImageDimension>, ::itk::GetImageDimension<TInputImage1>::ImageDimension> >
{
public:
	/** Convenient typedefs */
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
	typedef typename InputImageType::Pointer InputImagePointer;

	/** Output typedefs */
	typedef TOutputValueType OutputValueType;
	typedef itk::Vector<OutputValueType, ImageDimension> OutputPixelType;
	typedef itk::Image<OutputPixelType, ImageDimension> OutputImageType;
	typedef typename OutputImageType::Pointer OutputImagePointer;
	typedef typename OutputImageType::RegionType OutputImageRegionType;
	typedef OutputPixelType VectorType;

	/** Registration typedefs */
	typedef itk::TranslationTransform<TOutputValueType, ImageDimension> TransformType;
	typedef itk::NormalizedCorrelationImageToImageMetric<Input2ImageType, Input1ImageType> MetricType;
	typedef itk::RegularStepGradientDescentOptimizer OptimizerType;
	typedef itk::LinearInterpolateImageFunction<Input1ImageType, double> InterpolatorType;
	typedef itk::ImageRegistrationMethod<Input2ImageType, Input1ImageType> RegistrationType;

	/** Standard ITK class typedefs */
	typedef RegistrationMotionFilter Self;
	typedef itk::ImageToImageFilter<InputImageType, OutputImageType> Superclass;
	typedef itk::SmartPointer<Self> Pointer;
	typedef itk::SmartPointer<const Self> ConstPointer;

	/** itk New() factory method and type info */
	itkNewMacro(Self);
	itkTypeMacro(RegistrationMotionFilter, InPlaceImageFilter);

	/** Getters and setters */
	TInputImage1 * GetInput1() { return const_cast<TInputImage1 *> (this->GetInput(0)); }
	TInputImage2 * GetInput2() { return const_cast<TInputImage2 *> (this->GetInput(1)); }

	void SetInput1(const TInputImage1 * image1) { this->SetNthInput(0, const_cast<TInputImage1 *>(image1)); }
	void SetInput2(const TInputImage2 * image2) { this->SetNthInput(1, const_cast<TInputImage2 *>(image2)); }

	itkGetMacro(PatchRadius, int);
	itkSetMacro(PatchRadius, int);
	
	itkGetMacro(MaximumDisplacement, int);
	itkSetMacro(MaximumDisplacement, int);

protected:
	/** 
	 * Create a RegistrationMotionFilter.  A reasonable default configuration
	 * is established.
	 */
	RegistrationMotionFilter()
	{
		// reasonable defaults
		this->m_PatchRadius = 3;
		this->m_MaximumDisplacement = 5;
	}

	virtual ~RegistrationMotionFilter() {}

	void PrintSelf(std::ostream& os, itk::Indent indent) const;
	void LogSelf();

	/**
	 * Generate an input region based on a requested region.  The
	 * RegistrationMotionFilter requires a larger input region by PatchRadius + 
	 * MaximumDisplacement in each direction.  This ensures that any pixel on
	 * the edge of the requested region has enough data to find an optimal 
	 * displacement from the input images.
	 */
	virtual void GenerateInputRequestedRegion() throw (itk::InvalidRequestedRegionError);

	/**
	 * Perform some setup tasks before generating motion data.  Specifically, 
	 * configure and initialize image registration components.
	 */
	void BeforeGenerateData();

	/** 
	 * For image patches centered at each pixel location on Image1, compute the 
	 * displacement that optimally matches the patch with a location on Image2.
	 * The result across the entire image is a vector field representing motion
	 * from Image1 to Image2.
	 */
	void GenerateData();

	/**
	 * Clean up.
	 */
	void AfterGenerateData();

private:
	// The size of an image patch to use for registration.
	int m_PatchRadius;

	// The maximum possible translation expected for image patches.
	int m_MaximumDisplacement;

	typename RegistrationType::Pointer m_Registration;
	typename OptimizerType::Pointer m_Optimizer;
	typename InterpolatorType::Pointer m_Interpolator;
	typename MetricType::Pointer m_Metric;
	typename TransformType::Pointer m_Transform;
};

/************************************************************************/
/* Implementation                                                                     */
/************************************************************************/

#include "itkCastImageFilter.h"
#include "itkConstNeighborhoodIterator.h"
#include "itkExceptionObject.h"
#include "itkImageRegionIterator.h"
#include "itkProgressReporter.h"
#include "itkRegionOfInterestImageFilter.h"
#include "Logger.h"

template <class TInputImage1, class TInputImage2, class TOutputValueType>
void RegistrationMotionFilter<TInputImage1, TInputImage2, TOutputValueType>
::GenerateInputRequestedRegion() throw (itk::InvalidRequestedRegionError)
{
	Logger::logDebug("RegistrationMotionFilter::GenerateInputRequestedRegion()");

	// Call superclass method
	Superclass::GenerateInputRequestedRegion();
	
	// Get input image pointers
	Input1ImageType::Pointer movePtr = this->GetInput1();
	Input2ImageType::Pointer fixPtr  = this->GetInput2();

	if (!movePtr || !fixPtr)
	{
		Logger::logWarn("RegistrationMotionFilter::GenerateInputRequestedRegion(): One or more input images was not set!");
		return;
	}

	// Pad a copy of the requested region
	// For the moving image, we need to pad the region by PatchRadius to ensure each pixel location has
	// available data for the patch neighborhood.
	// For the fixed image, we need to pad the region by PatchRadius + MaximumDisplacement to ensure each
	// pixel location has neighborhood data for every possible location of the moving patch.
	Input1ImageType::RegionType moveRegion = movePtr->GetLargestPossibleRegion(); // movePtr->GetRequestedRegion();
	Input2ImageType::RegionType fixRegion = fixPtr->GetLargestPossibleRegion(); // fixPtr->GetRequestedRegion();
	moveRegion.PadByRadius(this->m_PatchRadius);
	fixRegion.PadByRadius(this->m_PatchRadius + this->m_MaximumDisplacement);

	// Crop the requested region at the input's largest possible region
	if (moveRegion.Crop(movePtr->GetLargestPossibleRegion()) && 
		fixRegion.Crop(fixPtr->GetLargestPossibleRegion()))
	{
		movePtr->SetRequestedRegion(moveRegion);
		fixPtr->SetRequestedRegion(fixRegion);
	}
	else // Couldn't crop--throw exception
	{
		movePtr->SetRequestedRegion(moveRegion);
		fixPtr->SetRequestedRegion(fixRegion);

		itk::InvalidRequestedRegionError e(__FILE__, __LINE__);
		itk::OStringStream msg;
		msg << static_cast<const char *>(this->GetNameOfClass()) 
			<< "::GenerateInputRequestedRegion()";
		e.SetLocation(msg.str().c_str());
		e.SetDescription("Requested region is (at least partially) outside the largest possible region.");
		e.SetDataObject(movePtr);
		throw e;
	}

	Logger::logDebug("RegistrationMotionFilter::GenerateInputRequestedRegion(): done.");
}

template <class TInputImage1, class TInputImage2, class TOutputValueType>
void RegistrationMotionFilter<TInputImage1, TInputImage2, TOutputValueType>
::BeforeGenerateData()
{
	Logger::logDebug("RegistrationMotionFilter::BeforeGenerateData()");

	// Initialize the registration components
	this->m_Metric =		MetricType::New();
	this->m_Optimizer =		OptimizerType::New();
	this->m_Transform =		TransformType::New();
	this->m_Interpolator =	InterpolatorType::New();
	this->m_Registration =	RegistrationType::New();

	this->m_Registration->SetMetric(this->m_Metric);
	this->m_Registration->SetOptimizer(this->m_Optimizer);
	this->m_Registration->SetTransform(this->m_Transform);
	this->m_Registration->SetInterpolator(this->m_Interpolator);

	// Setup optimizer
	this->m_Optimizer->SetMaximumStepLength(0.5);
	this->m_Optimizer->SetMinimumStepLength(0.005);
	this->m_Optimizer->SetNumberOfIterations(200);

	Logger::logDebug("RegistrationMotionFilter::BeforeGenerateData(): done.");
}

template <class TInputImage1, class TInputImage2, class TOutputValueType>
void RegistrationMotionFilter<TInputImage1, TInputImage2, TOutputValueType>
::GenerateData()
{
	Logger::logDebug("RegistrationMotionFilter::GenerateData()");
	this->LogSelf();

	// Update the input images
	this->GetInput1()->Update();
	this->GetInput2()->Update();

	// Allocate the output buffer
	Logger::logVerbose("Allocating output buffer...");
	Superclass::AllocateOutputs();
	OutputImageType::Pointer output = this->GetOutput();
	OutputImageType::RegionType outRegion = output->GetRequestedRegion();

	// Before generating data, setup registration components
	this->BeforeGenerateData();

	// Make equivalent sized input regions to iterate over
	Logger::logVerbose("Creating input regions to match output region...");
	Input1ImageRegionType moveRegion;
	Input1ImageRegionType::SizeType moveSize;
	Input1ImageRegionType::IndexType moveIdx;
	Input2ImageRegionType fixRegion;
	Input2ImageRegionType::SizeType fixSize;
	Input2ImageRegionType::IndexType fixIdx;

	for (int d = 0; d < ImageDimension; d++)
	{
		moveSize[d] = outRegion.GetSize()[d];
		fixSize[d]  = outRegion.GetSize()[d];
		moveIdx[d]  = outRegion.GetIndex()[d];
		fixIdx[d]   = outRegion.GetIndex()[d];
	}

	moveRegion.SetSize(moveSize);
	moveRegion.SetIndex(moveIdx);
	fixRegion.SetSize(fixSize);
	fixRegion.SetIndex(fixIdx);

	Logger::logVerbose("Setting up iterators");
	// Setup neighborhood iterator for moving image (Input1)
	typedef itk::ConstNeighborhoodIterator<Input1ImageType> MovingIteratorType;
	MovingIteratorType::RadiusType moveRadius;
	moveRadius.Fill(this->m_PatchRadius);
	MovingIteratorType moveIt(moveRadius, this->GetInput1(), moveRegion);
	
	// Setup neighborhood iterator for fixed image (Input2)
	typedef itk::ConstNeighborhoodIterator<Input1ImageType> FixedIteratorType;
	FixedIteratorType::RadiusType fixRadius;
	fixRadius.Fill(this->m_PatchRadius + this->m_MaximumDisplacement);
	FixedIteratorType fixIt(fixRadius, this->GetInput2(), fixRegion);

	// Setup output image iterator
	typedef itk::ImageRegionIterator<OutputImageType> OutputIteratorType;
	OutputIteratorType outIt(output, outRegion);

	Logger::logVerbose("Setting up patch extractors.");
	// Setup neighborhood region of interest filters
	typedef itk::RegionOfInterestImageFilter<Input1ImageType, Input1ImageType> MoveROIType;
	typedef itk::RegionOfInterestImageFilter<Input2ImageType, Input2ImageType> FixROIType;
	MoveROIType::Pointer moveROI = MoveROIType::New();
	FixROIType::Pointer fixROI = FixROIType::New();
	moveROI->SetInput(this->GetInput1());
	fixROI->SetInput(this->GetInput2());

	// Initial transform
	TransformType::Pointer transform = TransformType::New();
	TransformType::ParametersType init(transform->GetNumberOfParameters());
	for (int d = 0; d < ImageDimension; d++)
		init[d] = 0.0;
	
	// Reused variables
	TransformType::ParametersType final;
	VectorType pixel;

	// Calculate motion at each image location using registration
	Logger::logVerbose("Beginning registration-based motion calculation...");

	for (outIt.GoToBegin(), moveIt.GoToBegin(), fixIt.GoToBegin();
		!(outIt.IsAtEnd() || moveIt.IsAtEnd() || fixIt.IsAtEnd());
		++outIt, ++moveIt, ++fixIt)
	{
		// Extract region of interest
		moveRegion = moveIt.GetBoundingBoxAsImageRegion();
		fixRegion = fixIt.GetBoundingBoxAsImageRegion();

		if (moveRegion.Crop(moveIt.GetImagePointer()->GetLargestPossibleRegion()) &&
			fixRegion.Crop(fixIt.GetImagePointer()->GetLargestPossibleRegion()))
		{
			moveROI->SetRegionOfInterest(moveRegion);
			fixROI->SetRegionOfInterest(fixRegion);

			this->m_Registration->SetInitialTransformParameters(init);
			this->m_Registration->SetMovingImage(moveROI->GetOutput());
			this->m_Registration->SetFixedImage(fixROI->GetOutput());
			
			// Set the active fixed region (which contributes to the registration metric) to the entire fixed image patch
			fixROI->Update();
			this->m_Registration->SetFixedImageRegion(fixROI->GetOutput()->GetBufferedRegion());
			
			// Register patches
			try
			{
				// Register!
				this->m_Registration->StartRegistration();

				// Get and save results
				final = this->m_Registration->GetLastTransformParameters();
				for (int d = 0; d < ImageDimension; d++)
					pixel[d] = final[d];			
			}
			catch (itk::ExceptionObject &err)
			{
				for (int d = 0; d < ImageDimension; d++)
					pixel[d] = 0.0;

				std::string message;
				message.append("Exception caught!\n").append(err.GetDescription());
				Logger::logError(message);
			}
		}
		else
		{
			Logger::logWarn("Unexpected situation: the fixed or moving image region is outside the bounds of the original image");
			for (int d = 0; d < ImageDimension; d++)
				pixel[d] = 0.0;
		}

		// Output
		outIt.Set(pixel);
	}

	// Finished generating data
	this->AfterGenerateData();

	Logger::logVerbose("Grafting output...");
	this->GraftOutput(output);
	Logger::logDebug("RegistrationMotionFilter::GenerateData(): done.");
}

template <class TInputImage1, class TInputImage2, class TOutputValueType>
void RegistrationMotionFilter<TInputImage1, TInputImage2, TOutputValueType>
::AfterGenerateData()
{
	// We have nothing to do here...yet.
}

template <class TInputImage1, class TInputImage2, class TOutputValueType>
void RegistrationMotionFilter<TInputImage1, TInputImage2, TOutputValueType>
::PrintSelf(std::ostream& os, itk::Indent indent) const
{
	Superclass::PrintSelf(os, indent);
	os << indent << "PatchRadius: " << this->m_PatchRadius << std::endl;
	os << indent << "MaximumDisplacement: " << this->m_MaximumDisplacement << std::endl;
}

template <class TInputImage1, class TInputImage2, class TOutputValueType>
void RegistrationMotionFilter<TInputImage1, TInputImage2, TOutputValueType>
::LogSelf()
{
	char text[80];
	sprintf(text, "PatchRadius: %d", this->GetPatchRadius());
	Logger::logInfo(text);
	sprintf(text, "MaximumDisplacement: %d", this->GetMaximumDisplacement());
	Logger::logInfo(text);
}