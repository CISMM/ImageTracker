#pragma once

#include <string>
#include "itkCenteredRigid2DTransform.h"
#include "itkTranslationTransform.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkObject.h"
#include "itkResampleImageFilter.h"
#include "itkThresholdImageFilter.h"

#include "FileSet.h"
#include "FileSetImageReader.h"
#include "ItkMagickIO.h"
#include "MultiResolutionRegistration.h"

/**
 * \class MultiResolutionRegistrationPipeline
 */
class MultiResolutionRegistrationPipeline : 
	public itk::Object
{
public:
	// Standard itk object definitions
	typedef MultiResolutionRegistrationPipeline Self;
	typedef itk::LightObject Superclass;
	typedef itk::SmartPointer<Self> Pointer;
	typedef itk::SmartPointer<const Self> ConstPointer;
	itkNewMacro(Self);
	itkTypeMacro(MultiResolutionRegistrationProcessor, Object);

	// Pipeline typedefs
	typedef itk::CenteredRigid2DTransform<double> TransformType;
	// typedef itk::TranslationTransform<double, 2> TransformType;
	typedef CommonTypes::InternalImageType ImageType;
	typedef itk::ThresholdImageFilter<ImageType> ThresholdType;
	typedef MultiResolutionRegistration<ImageType, TransformType> RegistrationType;
	typedef itk::ResampleImageFilter<ImageType, ImageType> ResampleType;

	// For image preview
	typedef itk::DiscreteGaussianImageFilter<ImageType, ImageType> SmoothType;

	/**
	 * Runs multiresolution registration on all images specified by
	 * InputFiles.  The resulting registration parameters and registered
	 * images are saved.
	 */
	void Update();

	unsigned int GetStartingShrinkFactor();
	unsigned int GetNumberOfLevels();
	ImageType::PixelType GetUpperThreshold();
	ImageType::PixelType GetLowerThreshold();

	void SetStartingShrinkFactor(unsigned int factor);
	void SetNumberOfLevels(unsigned int levels);
	void SetUpperThreshold(ImageType::PixelType threshold);
	void SetLowerThreshold(ImageType::PixelType threshold);

	double GetOptimizerInitialMaximumStepLength() 
	{ return this->registration->GetOptimizerInitialMaximumStepLength(); }
	
	double GetOptimizerInitialMinimumStepLength() 
	{ return this->registration->GetOptimizerInitialMinimumStepLength(); }
	
	double GetOptimizerStepLengthScale() 
	{ return this->registration->GetOptimizerStepLengthScale(); }

	unsigned long GetOptimizerNumberOfIterations()
	{ return this->registration->GetOptimizerNumberOfIterations(); }

	void SetOptimizerInitialMaximumStepLength(double length)
	{ this->registration->SetOptimizerInitialMaximumStepLength(length); }

	void SetOptimizerInitialMinimumStepLength(double length)
	{ this->registration->SetOptimizerInitialMinimumStepLength(length); }

	void SetOptimizerStepLengthScale(double scale)
	{ this->registration->SetOptimizerStepLengthScale(scale); }

	void SetOptimizerNumberOfIterations(unsigned long iters)
	{ this->registration->SetOptimizerNumberOfIterations(iters); }
	
	FileSet* GetInputFiles();
	FileSet* GetOutputFiles();
	void SetInputFiles(FileSet& files);
	void SetOutputFiles(FileSet& files);

	ImageType::Pointer GetPreviewImage();

	itkGetMacro(TransformFile, std::string);
	itkSetMacro(TransformFile, std::string);

protected:
	MultiResolutionRegistrationPipeline();
	~MultiResolutionRegistrationPipeline(){}

private:
	FileSet inputFiles;
	FileSet outputFiles;
	std::string m_TransformFile;

	ThresholdType::Pointer threshold[2];
	RegistrationType::Pointer registration;
	ResampleType::Pointer resample;
	SmoothType::Pointer smooth;

	FileSetImageReader inputReader;
};