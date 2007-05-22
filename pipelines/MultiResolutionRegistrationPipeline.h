#pragma once

#include <string>

#include "itkCastImageFilter.h"
#include "itkCenteredRigid2DTransform.h"
#include "itkTranslationTransform.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkObject.h"
#include "itkResampleImageFilter.h"
#include "itkThresholdImageFilter.h"

#include "CommonTypes.h"
#include "FileSet.h"
#include "ImageSetReader.h"
#include "ItkPipeline.h"
#include "MultiResolutionRegistration.h"

/**
 * \class MultiResolutionRegistrationPipeline
 */
class MultiResolutionRegistrationPipeline : 
	public ItkPipeline
{
public:
	// Standard itk object definitions
	typedef MultiResolutionRegistrationPipeline Self;
	typedef ItkPipeline Superclass;
	typedef itk::SmartPointer<Self> Pointer;
	typedef itk::SmartPointer<const Self> ConstPointer;
	itkNewMacro(Self);
	itkTypeMacro(MultiResolutionRegistrationProcessor, ItkPipeline);

	// Pipeline typedefs
	typedef itk::CenteredRigid2DTransform<double> TransformType;
	// typedef itk::TranslationTransform<double, 2> TransformType;
	typedef CommonTypes::InputImageType InputImageType;
	typedef CommonTypes::InternalImageType ImageType;
	typedef ImageSetReaderBase* ReaderType;
	typedef itk::ThresholdImageFilter<ImageType> ThresholdType;
	typedef MultiResolutionRegistration<ImageType, TransformType> RegistrationType;
	typedef itk::ResampleImageFilter<ImageType, ImageType> ResampleType;
	typedef itk::CastImageFilter<ImageType, InputImageType> CasterType;

	// For image preview
	typedef itk::DiscreteGaussianImageFilter<ImageType, ImageType> SmoothType;

	/**
	 * Runs multiresolution registration on all images specified by
	 * InputFiles.  The resulting registration parameters and registered
	 * images are saved.
	 */
	virtual void Update();

	ImageType::PixelType GetUpperThreshold();
	ImageType::PixelType GetLowerThreshold();
        void SetUpperThreshold(ImageType::PixelType threshold);
        void SetLowerThreshold(ImageType::PixelType threshold);

        void SetPreviewShrinkFactor(unsigned int factor);
	void SetShrinkFactors(unsigned int min, unsigned int max);
        
        unsigned int GetStartingShrinkFactor();
        unsigned int GetNumberOfLevels();
        void SetStartingShrinkFactor(unsigned int factor);
	void SetNumberOfLevels(unsigned int levels);

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
	
	const FileSet& GetOutputFiles() { return this->outputFiles; }
	void SetOutputFiles(const FileSet& files) { this->outputFiles = files; }

        void SetInput(ImageSetReaderBase* input);
	ImageType::Pointer GetPreviewImage();

	itkGetMacro(TransformFile, std::string);
	itkSetMacro(TransformFile, std::string);

protected:
	MultiResolutionRegistrationPipeline();
	~MultiResolutionRegistrationPipeline(){}

private:
	FileSet outputFiles;
	std::string m_TransformFile;
        bool m_ThresholdBetween;

	ThresholdType::Pointer threshold[2];
	RegistrationType::Pointer registration;
	ResampleType::Pointer resample;
	CasterType::Pointer caster;
	SmoothType::Pointer smooth;

	ReaderType inputReader;
};
