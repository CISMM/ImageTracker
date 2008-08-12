#pragma once

#include <string>

#include "itkCastImageFilter.h"
#include "itkCenteredRigid2DTransform.h"
#include "itkTranslationTransform.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkObject.h"
#include "itkResampleImageFilter.h"

#include "ImageFileSet.h"
#include "ItkImagePipeline.h"
#include "MultiResolutionRegistration.h"

/**
 * \class MultiResolutionRegistrationPipeline
 * \brief Performs a multiresolution registration of images.
 * 
 * Registers the images in a series using a multiresolution framework.
 * The multiresolution framework uses a multiscale image pyramid with
 * coarse and fine bounds.  The registration performs rough alignmnent
 * at the coarse level and refines the registration estimate at the
 * lower levels.  
 */
class MultiResolutionRegistrationPipeline : 
	public ItkImagePipeline
{
public:
	// Standard itk object definitions
	typedef MultiResolutionRegistrationPipeline Self;
	typedef ItkImagePipeline Superclass;
	typedef itk::SmartPointer<Self> Pointer;
	typedef itk::SmartPointer<const Self> ConstPointer;
	itkNewMacro(Self);
	itkTypeMacro(MultiResolutionRegistrationProcessor, ItkImagePipeline);

	// Pipeline typedefs
	typedef itk::CenteredRigid2DTransform<double> TransformType;
	typedef ImageFileSet::ImageType ImageType;
	typedef MultiResolutionRegistration<ImageType, TransformType> RegistrationType;
	typedef itk::ResampleImageFilter<ImageType, ImageType> ResampleType;

	// For image preview
	typedef itk::DiscreteGaussianImageFilter<ImageType, ImageType> SmoothType;

	/**
	 * Runs multiresolution registration on all images specified by
	 * InputFiles.  The resulting registration parameters and registered
	 * images are saved.
	 */
	virtual void Update();

    virtual void SetInput(ImageFileSet* input);
    
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

	ImageType::Pointer GetPreviewImage();

	itkGetMacro(TransformFile, std::string);
	itkSetMacro(TransformFile, std::string);

protected:
	MultiResolutionRegistrationPipeline();
	~MultiResolutionRegistrationPipeline(){}

private:
	//FileSet outputFiles;
	std::string m_TransformFile;
    bool m_ThresholdBetween;

	RegistrationType::Pointer registration;
	ResampleType::Pointer resample;
	SmoothType::Pointer smooth;
};
