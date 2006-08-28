#pragma once

#include "itkMeanSquaresImageToImageMetric.h"
#include "itkNormalizedCorrelationImageToImageMetric.h"

#include "itkCommand.h"
#include "itkImage.h"
#include "itkIndent.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkMultiResolutionImageRegistrationMethod.h"
#include "itkMultiResolutionPyramidImageFilter.h"
#include "itkProcessObject.h"
#include "itkRegularStepGradientDescentOptimizer.h"

/**
 * \class MultiResolutionIterationCommand
 * \brief Updates registration optimizer parameters between resolution
 * scale levels.
 */
template <typename TRegistration>
class MultiResolutionIterationCommand : 
	public itk::Command
{
public:
	// Standard itk object definitions
	typedef MultiResolutionIterationCommand	Self;
	typedef itk::Command					Superclass;
	typedef itk::SmartPointer<Self>			Pointer;
	itkNewMacro(Self);

	// Useful typdefs
	typedef TRegistration		RegistrationType;
	typedef RegistrationType*	RegistrationPointer;
	typedef itk::RegularStepGradientDescentOptimizer OptimizerType;
	typedef OptimizerType*		OptimizerPointer;

	/**
	 * The Execute() method handles iteration events between resolution
	 * levels of a multi-resolution registration process.  This command
	 * object changes the parameters of the registration optimizer.  This
	 * allows for a more logical progression from coarse to fine resolution.
	 */
	void Execute(itk::Object* object, const itk::EventObject &event);

	/**
	 * The version of Execute() with const itk::Object does nothing--it is 
	 * required by base class itk::Command.
	 */
	void Execute(const itk::Object *, const itk::EventObject &event) { return; }

	itkGetMacro(InitialMaximumStepLength, double);
	itkSetMacro(InitialMaximumStepLength, double);

	itkGetMacro(InitialMinimumStepLength, double);
	itkSetMacro(InitialMinimumStepLength, double);

	itkGetMacro(StepLengthScale, double);
	itkSetMacro(StepLengthScale, double);

	itkGetMacro(NumberOfIterations, unsigned long);
	itkSetMacro(NumberOfIterations, unsigned long);

protected:
	// Protected constructor--callers must instantiate using New() method.
	MultiResolutionIterationCommand() {}

private:
	double m_InitialMaximumStepLength;
	double m_InitialMinimumStepLength;
	double m_StepLengthScale;
	unsigned long m_NumberOfIterations;
};

/**
 * \class MultiResolutionRegistration
 * \brief An more concrete implementation of an itk::MultiResolutionImageRegistrationMethod.
 *
 * Implements multi-resolution registration with several components of the registration pre-selected.
 * The implementation is templated over the image type and the transform type.  The registration
 * image metric, optimizer, and interpolator are pre-selected.  All registration components are
 * initialized such that this registration component is ready to use directly after instantiation.
 * This sacrifices some degree of flexibility for ease of use.  The metric, optimizer, and 
 * interpolator are selected as the best-performing components based on image processing experience
 * with fluorescent and bright field microscopy of mitosis and cilia activity.  It is
 * expected that this set of components will work well in many other cases, too.
 */
template <typename TImage, typename TTransform>
class MultiResolutionRegistration : 
	public itk::ProcessObject
{
public:
	// Standard itk type definitions
	typedef MultiResolutionRegistration Self;
	typedef itk::ProcessObject Superclass;
	typedef itk::SmartPointer<Self> Pointer;
	typedef itk::SmartPointer<const Self> ConstPointer;

	itkNewMacro(Self);
	itkTypeMacro(MultiResolutionRegistration, Object);

	// Registration typedefs
	// Image type of both fixed and moving images.
	typedef	TImage ImageType;
	// User-defined transform type.
	typedef	TTransform TransformType;
	typedef typename ImageType::Pointer ImagePointer;
	typedef typename TransformType::Pointer TransformPointer;

	// Normalized correlation metric corrects for intensity shifts.
	// typedef itk::MeanSquaresImageToImageMetric<ImageType, ImageType> MetricType;
	typedef itk::NormalizedCorrelationImageToImageMetric<ImageType, ImageType> MetricType;
	// A good optimizer.
	typedef itk::RegularStepGradientDescentOptimizer OptimizerType;
	// Resonably smooth interpolation between pixel locations.
	typedef itk::LinearInterpolateImageFunction<ImageType, double> InterpolatorType;
	// Filter for creating image pyramids.
	typedef itk::MultiResolutionPyramidImageFilter<ImageType, ImageType> ImagePyramidType;
	// Registration method that uses image pyramids recursively.
	typedef itk::MultiResolutionImageRegistrationMethod<ImageType, ImageType> RegistrationType;

	// Observer that adjusts optimization parameters between iterations.
	typedef MultiResolutionIterationCommand<RegistrationType> ObserverType;
	// Type of the transform parameters.
	typedef typename MetricType::TransformParametersType ParametersType;

	// Set the number of levels used in the multi-resolution method.
	void SetNumberOfLevels(unsigned int levels);
	unsigned int GetNumberOfLevels() const;

	// Set the starting shrink factor used in the multi-resolution method.
	void SetStartingShrinkFactor(unsigned int factor);
	unsigned int GetStartingShrinkFactor() const;

	// Set the optimizer's initial maximum step length.
	void SetOptimizerInitialMaximumStepLength(double step);
	double GetOptimizerInitialMaximumStepLength() const;

	// Set the optimizer's initial minimum step length.
	void SetOptimizerInitialMinimumStepLength(double step);
	double GetOptimizerInitialMinimumStepLength() const;

	// Set the optimizer step scaling factor between resolution levels.
	void SetOptimizerStepLengthScale(double scale);
	double GetOptimizerStepLengthScale() const;

	void SetOptimizerNumberOfIterations(unsigned long iters);
	unsigned long GetOptimizerNumberOfIterations() const;

	// Set the fixed image for this registration.
	void SetFixedImage(ImagePointer fixed);
	// Set the moving image for this registration.
	void SetMovingImage(ImagePointer moving);

	// Get the latest modified time for this registration object or
	// any of its components.
	unsigned long GetMTime() const;

	/**
	 * Start the registration.  This kicks of the registration optimization
	 * process.  We do it in this method instead of GenerateData() to be
	 * consistent with the other registration methods.  GenerateData() just
	 * calls this method (in those implementations and this one), so Update()
	 * will also work as expected.
	 */
	void StartRegistration();

	/**
	 * Set the initial transform for this registration.  Note: if using
	 * this registation object for multiple registrations, you should set
	 * the initial transform for *each* registration iteration.  If an
	 * initial transform parameter is not set, an identity default will be
	 * created.
	 */
	itkSetMacro(InitialTransform, TransformPointer);

	// Get the results of the registration.
	itkGetMacro(LastTransform, TransformPointer);
	
protected:
	/**
	 * Initializes the registration components, constructs the registration
	 * pipeline, and sets default parameters.
	 */
	MultiResolutionRegistration();
	virtual ~MultiResolutionRegistration();
	void PrintSelf(std::ostream& os, itk::Indent indent) const;
	void LogSelf() const;

	// Run the registration on the given input images.  Creates the Transform output.
	void GenerateData();

	// Set the transform resulting from this registration.
	itkSetMacro(LastTransform, TransformPointer);

private:
	MultiResolutionRegistration(const Self&); //purposely not implemented
	void operator=(const Self&); //purposely not implemented

	typename TransformType::InputPointType FindImageCenter(ImagePointer image);
	TransformPointer DefaultInitialTransform();

	typename MetricType::Pointer metric;
	typename OptimizerType::Pointer optimizer;
	typename TransformType::Pointer transform;
	typename InterpolatorType::Pointer interpolator;
	typename RegistrationType::Pointer registration;
	typename ImagePyramidType::Pointer fixedPyramid;
	typename ImagePyramidType::Pointer movingPyramid;
	typename ObserverType::Pointer observer;
	ImagePointer fixedImage;
	ImagePointer movingImage;

	TransformPointer m_InitialTransform;
	TransformPointer m_LastTransform;
};

///////////////////////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////////////////////

#include "Logger.h"
#include "itkExceptionObject.h"
#include "itkObject.h"
#include "itkTimeProbe.h"

template <typename TRegistration> 
void MultiResolutionIterationCommand<TRegistration>::Execute(itk::Object* object, const itk::EventObject &event)
{
	// Ensure we're dealing with an iteration event
	if (!itk::IterationEvent().CheckEvent(&event))
	{
		return;
	}

	// The object supplied is a registration object.
	RegistrationPointer registration = 
		dynamic_cast<RegistrationPointer>(object);

	OptimizerPointer optimizer = 
		dynamic_cast<OptimizerPointer>(registration->GetOptimizer());

	Logger::info << 
		"MultiresolutionIterationCommand::Execute: Updating optimizer at level: " 
		<< registration->GetCurrentLevel() << std::endl;

	// Set optimization parameters
	if (registration->GetCurrentLevel() == 0)
	{
		// initial parameters
		optimizer->SetMaximumStepLength(this->GetInitialMaximumStepLength());
		optimizer->SetMinimumStepLength(this->GetInitialMinimumStepLength());
		optimizer->SetNumberOfIterations(this->GetNumberOfIterations());
	}
	else
	{
		// next-level parameters:
		// using the current step length as the next level's
		// maximum ensures a smooth transition
		optimizer->SetMaximumStepLength(
			optimizer->GetCurrentStepLength());
		// adapt the accuracy from coarse to fine
		if (this->GetStepLengthScale() > 0)
			optimizer->SetMinimumStepLength(
				optimizer->GetMinimumStepLength() / this->GetStepLengthScale());
		else // avoid a division by zero with a nice default
			optimizer->SetMinimumStepLength(
				optimizer->GetMinimumStepLength() / 10.0);
	}
}

template <typename TImage, typename TTransform>
MultiResolutionRegistration<TImage, TTransform>
::MultiResolutionRegistration()
{
	Logger::verbose << "MultiResolutionRegistration::Constructor" << std::endl;

	// Initialize registration components
	this->metric = MetricType::New();
	this->optimizer = OptimizerType::New();
	this->transform = TransformType::New();
	this->interpolator = InterpolatorType::New();
	this->fixedPyramid = ImagePyramidType::New();
	this->movingPyramid = ImagePyramidType::New();
	this->registration = RegistrationType::New();
	this->observer = ObserverType::New();
	
	// supplied by user, or calculated
	this->fixedImage = 0;
	this->movingImage = 0;
	this->m_InitialTransform = 0;
	this->m_LastTransform = 0;

	// Setup scaling of transform parameters *if* this transform
	// uses rotation.  (Rotation is more severe than translation, 
	// so this ensures a single rotation step is smaller than a single
	// translation step.)
	if (this->transform->GetNumberOfParameters() == 5)  // Centered rigid 2D
	{
		OptimizerType::ScalesType scales(this->transform->GetNumberOfParameters());
		const double scale = 1.0 / 1000.0;
		scales[0] = 1.0;
		for (int i = 1; i < this->transform->GetNumberOfParameters(); i++)
		{
			scales[i] = scales[0] * scale;
		}
		this->optimizer->SetScales(scales);
	}

	// Choose some good default optimization parameters
	this->SetOptimizerInitialMaximumStepLength(3.0);
	this->SetOptimizerInitialMinimumStepLength(1.0);
	this->SetOptimizerStepLengthScale(10.0);
	this->SetOptimizerNumberOfIterations(200);

	// Connect registration components
	this->registration->SetMetric(this->metric);
	this->registration->SetOptimizer(this->optimizer);
	this->registration->SetTransform(this->transform);
	this->registration->SetInterpolator(this->interpolator);
	this->registration->SetFixedImagePyramid(this->fixedPyramid);
	this->registration->SetMovingImagePyramid(this->movingPyramid);
	this->registration->AddObserver(itk::IterationEvent(), observer);

	// Choose some good default pyramid parameters
	this->SetNumberOfLevels(3);
	this->SetStartingShrinkFactor(4);
}

template <typename TImage, typename TTransform>
MultiResolutionRegistration<TImage, TTransform>
::~MultiResolutionRegistration()
{
}

template <typename TImage, typename TTransform>
void MultiResolutionRegistration<TImage, TTransform>
::SetNumberOfLevels(unsigned int levels)
{
	// Oddly, when the number of levels is changed independently, the
	// starting shrink factor changes.  So, we have to save it here
	// and reset it.  Go figure.
	unsigned int shrink = this->GetStartingShrinkFactor();

	Logger::verbose << "MultiResolutionRegistration::SetNumberOfLevels: Before: levels => " 
		<< this->GetNumberOfLevels() << " shrink => " << this->GetStartingShrinkFactor() << std::endl;

	this->fixedPyramid->SetNumberOfLevels(levels);
	this->movingPyramid->SetNumberOfLevels(levels);

	// Not sure why the registration method requires this to be set, too,
	// But maybe this is so the two pyramid filters supplied can have 
	// different filter schedules. (We're not doing that here, though.)
	this->registration->SetNumberOfLevels(levels);
	this->Modified();

	this->SetStartingShrinkFactor(shrink);
	Logger::verbose << "MultiResolutionRegistration::SetNumberOfLevels: After: levels => " 
		<< this->GetNumberOfLevels() << " shrink => " << this->GetStartingShrinkFactor() << std::endl;
}

template <typename TImage, typename TTransform>
unsigned int MultiResolutionRegistration<TImage, TTransform>
::GetNumberOfLevels() const
{
	return this->registration->GetNumberOfLevels();
}

template <typename TImage, typename TTransform>
void MultiResolutionRegistration<TImage, TTransform>
::SetStartingShrinkFactor(unsigned int factor)
{
	Logger::verbose << "MultiResolutionRegistration::SetStartingShrinkFactor: Before: levels => " 
		<< this->GetNumberOfLevels() << " shrink => " << this->GetStartingShrinkFactor() << std::endl;
	this->fixedPyramid->SetStartingShrinkFactors(factor);
	this->movingPyramid->SetStartingShrinkFactors(factor);
	this->Modified();
	Logger::verbose << "MultiResolutionRegistration::SetStartingShrinkFactor: After: levels => " 
		<< this->GetNumberOfLevels() << " shrink => " << this->GetStartingShrinkFactor() << std::endl;
}

template <typename TImage, typename TTransform>
unsigned int MultiResolutionRegistration<TImage, TTransform>
::GetStartingShrinkFactor() const
{
	return this->fixedPyramid->GetStartingShrinkFactors()[0];
}

template <typename TImage, typename TTransform>
void MultiResolutionRegistration<TImage, TTransform>
::SetOptimizerInitialMaximumStepLength(double step)
{
	// The observer takes care of initializing optimization parameters.
	Logger::verbose << "MultiResolutionRegistration::SetOptimizerInitialMaximumStepLength => " << step << std::endl;
	this->observer->SetInitialMaximumStepLength(step);
	this->Modified();
}

template <typename TImage, typename TTransform>
double MultiResolutionRegistration<TImage, TTransform>
::GetOptimizerInitialMaximumStepLength() const
{
	return this->observer->GetInitialMaximumStepLength();
}

template <typename TImage, typename TTransform>
void MultiResolutionRegistration<TImage, TTransform>
::SetOptimizerInitialMinimumStepLength(double step)
{
	// The observer takes care of initializing optimization parameters.
	Logger::verbose << "MultiResolutionRegistration::SetOptimizerInitialMinimumStepLength => " << step << std::endl;
	this->observer->SetInitialMinimumStepLength(step);
	this->Modified();
}

template <typename TImage, typename TTransform>
double MultiResolutionRegistration<TImage, TTransform>
::GetOptimizerInitialMinimumStepLength() const
{
	return this->observer->GetInitialMinimumStepLength();
}

template <typename TImage, typename TTransform>
void MultiResolutionRegistration<TImage, TTransform>
::SetOptimizerStepLengthScale(double scale)
{
	// The observer controls optimization parameters.
	Logger::verbose << "MultiResolutionRegistration::SetOptimizerStepLengthScale => " << scale << std::endl;
	this->observer->SetStepLengthScale(scale);
	this->Modified();
}

template <typename TImage, typename TTransform>
double MultiResolutionRegistration<TImage, TTransform>
::GetOptimizerStepLengthScale() const
{
	return this->observer->GetStepLengthScale();
}

template <typename TImage, typename TTransform>
void MultiResolutionRegistration<TImage, TTransform>
::SetOptimizerNumberOfIterations(unsigned long iters)
{
	// The observer controls optimization parameters.
	Logger::verbose << "MultiResolutionRegistration::SetOptimizerNumberOfIterations => " << iters << std::endl;
	this->observer->SetNumberOfIterations(iters);
	this->Modified();
}

template <typename TImage, typename TTransform>
unsigned long MultiResolutionRegistration<TImage, TTransform>
::GetOptimizerNumberOfIterations() const
{
	return this->observer->GetNumberOfIterations();
}

template <typename TImage, typename TTransform>
void MultiResolutionRegistration<TImage, TTransform>
::SetFixedImage(ImagePointer fixed)
{
	this->fixedImage = fixed;
	this->fixedPyramid->SetInput(fixed);
	this->registration->SetFixedImage(fixed);
	this->Modified();
}

template <typename TImage, typename TTransform>
void MultiResolutionRegistration<TImage, TTransform>
::SetMovingImage(ImagePointer moving)
{
	this->movingImage = moving;
	this->movingPyramid->SetInput(moving);
	this->registration->SetMovingImage(moving);
	this->Modified();
}

template <typename TImage, typename TTransform>
unsigned long MultiResolutionRegistration<TImage, TTransform>
::GetMTime() const
{
	unsigned long mtime = Superclass::GetMTime();
	unsigned long m;

	m = this->transform->GetMTime();
	mtime = (m > mtime ? m : mtime);
	m = this->observer->GetMTime();
	mtime = (m > mtime ? m : mtime);

	return mtime;
}

template <typename TImage, typename TTransform>
void MultiResolutionRegistration<TImage, TTransform>
::GenerateData()
{
	this->StartRegistration();
}

template <typename TImage, typename TTransform>
void MultiResolutionRegistration<TImage, TTransform>
::StartRegistration()
{
	Logger::verbose << "MultiResolutionRegistration::StartRegistration()" << std::endl;

	// Make sure we can do something.
	if (!this->fixedImage ||
		!this->movingImage)
	{
		Logger::error << "Fixed or moving image is not present!" << std::endl;
		return; // bail
	}

	this->LogSelf();

	// Check for an initial transform
	if (!this->m_InitialTransform)
	{
		// Use a default if none is provided.
		this->m_InitialTransform = this->DefaultInitialTransform();
	}

	// Setup registration region and initial transform
	this->fixedImage->Update();
	this->registration->SetFixedImageRegion(
		this->fixedImage->GetBufferedRegion());
	this->registration->SetInitialTransformParameters(
		this->m_InitialTransform->GetParameters());
	
	itk::TimeProbe timer;
	timer.Start();
	// Run registration
	try
	{
		this->registration->StartRegistration();
	}
	catch (itk::ExceptionObject &error)
	{
		Logger::error << "MultiResolutionRegistration::StartRegistration: Exception caught!" << std::endl;
		std::cerr << error << std::endl;
		typename TransformType::Pointer xform = TransformType::New();
		typename OptimizerType::ParametersType finalParams;
		finalParams.fill(0.0f);
		xform->SetParameters(finalParams);
		this->SetLastTransform(xform);
		return;
	}
	timer.Stop();
	Logger::info << "MultiResolutionRegistration: registration time: " << timer.GetMeanTime() << std::endl;
	Logger::info << "MutliResolutionRegistration: optimizer metric:  " << this->optimizer->GetValue() << std::endl;

	// Grab results
	typename TransformType::Pointer xform = TransformType::New();
	typename OptimizerType::ParametersType finalParams = this->registration->GetLastTransformParameters();
	xform->SetParameters(finalParams);
	this->SetLastTransform(xform);

	// Reset the initial transform
	this->m_InitialTransform = 0;
}

template <typename TImage, typename TTransform>
typename MultiResolutionRegistration<TImage, TTransform>::TransformType::InputPointType
MultiResolutionRegistration<TImage, TTransform>
::FindImageCenter(ImagePointer image)
{
	// Get image parameters
	const typename ImageType::SpacingType& spacing = image->GetSpacing();
	const typename ImageType::PointType& origin = image->GetOrigin();
	typename ImageType::SizeType size = image->GetLargestPossibleRegion().GetSize();

	// Compute center of image
	typename TransformType::InputPointType center;
	for (int d = 0; d < itk::GetImageDimension<ImageType>::ImageDimension; d++)
		center[d] = origin[d] + spacing[d] * (size[d] * 0.5);

	return center;
}

template <typename TImage, typename TTransform>
typename MultiResolutionRegistration<TImage, TTransform>::TransformPointer
MultiResolutionRegistration<TImage, TTransform>::DefaultInitialTransform()
{
	TransformPointer xform = TransformType::New();

	// Test for the type of translation we're doing
	if (this->transform->GetNumberOfParameters() == 5)
	{
		// Find an identity centered rigid 2D transform.
		this->fixedImage->Update();
		this->movingImage->Update();

		// Find centers of the images
		typename TransformType::InputPointType centerFixed =	
			this->FindImageCenter(this->fixedImage);
		typename TransformType::InputPointType centerMoving =
			this->FindImageCenter(this->movingImage);

		// Compute transform that aligns image centers
		typename TransformType::ParametersType params = xform->GetParameters();
		params[0] = 0.0;				// angle
		params[1] = centerFixed[0];		// center
		params[2] = centerFixed[1];
		params[3] = centerMoving[0] - centerFixed[0];	// initial transform
		params[4] = centerMoving[1] - centerFixed[1];
		xform->SetParameters(params);
	}
	else
	{
		// If we're not using a centered rigid transform, our initial parameters
		// can be all zero. (Regular translation.)
		typename TransformType::ParametersType params = xform->GetParameters();
		params.fill(0.0f);
		xform->SetParameters(params);
	}

	return xform;
}

template <typename TImage, typename TTransform>
void MultiResolutionRegistration<TImage, TTransform>
::PrintSelf(std::ostream& os, itk::Indent indent) const
{
	// Superclass::PrintSelf(os, indent); // this is not very interesting
	os << indent << "MultiResolutionRegistration::PrintSelf: " << std::endl;
	os << indent << "   Number of Levels:            " << this->GetNumberOfLevels() << std::endl;
	os << indent << "   Starting Shrink Factor:      " << this->GetStartingShrinkFactor() << std::endl;
	os << indent << "   Optimizer Initial Max Step:  " << this->GetOptimizerInitialMaximumStepLength() << std::endl;
	os << indent << "   Optimizer Initial Min Step:  " << this->GetOptimizerInitialMinimumStepLength() << std::endl;
	os << indent << "   Optimizer Step Length Scale: " << this->GetOptimizerStepLengthScale() << std::endl;
	os << indent << "   Optimizer Iterations:        " << this->GetOptimizerNumberOfIterations() << std::endl;
}

template <typename TImage, typename TTransform>
void MultiResolutionRegistration<TImage, TTransform>
::LogSelf() const
{
	Logger::debug << "MultiResolutionRegistration::LogSelf: " << std::endl;
	Logger::debug << "   Number of Levels:            " << this->GetNumberOfLevels() << std::endl;
	Logger::debug << "   Starting Shrink Factor:      " << this->GetStartingShrinkFactor() << std::endl;
	Logger::debug << "   Optimizer Initial Max Step:  " << this->GetOptimizerInitialMaximumStepLength() << std::endl;
	Logger::debug << "   Optimizer Initial Min Step:  " << this->GetOptimizerInitialMinimumStepLength() << std::endl;
	Logger::debug << "   Optimizer Step Length Scale: " << this->GetOptimizerStepLengthScale() << std::endl;
	Logger::debug << "   Optimizer Iterations:        " << this->GetOptimizerNumberOfIterations() << std::endl;
}
