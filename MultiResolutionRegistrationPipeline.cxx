#include "MultiResolutionRegistrationPipeline.h"
#include <vector>
#include "TransformGroup.h"

MultiResolutionRegistrationPipeline::MultiResolutionRegistrationPipeline()
{
	// Create pipeline objects
	this->threshold[0] = ThresholdType::New();
	this->threshold[1] = ThresholdType::New();
	this->registration = RegistrationType::New();
	this->resample = ResampleType::New();
	this->caster = CasterType::New();
	this->smooth = SmoothType::New();

	// Connect pipeline
	this->registration->SetFixedImage(this->threshold[0]->GetOutput());
	this->registration->SetMovingImage(this->threshold[1]->GetOutput());
	this->smooth->SetInput(this->threshold[0]->GetOutput());

	// User must supply these
	this->SetTransformFile("");
}

void MultiResolutionRegistrationPipeline::SetInputFiles(const FileSet &files)
{
	this->inputFiles = files;
	this->inputReader = ReaderType(this->inputFiles);

	this->resample->SetInput(this->inputReader[0]);
	this->threshold[0]->SetInput(this->inputReader[0]);
	this->threshold[1]->SetInput(this->inputReader[1]);

	this->smooth->SetInput(this->threshold[0]->GetOutput());
}

unsigned int MultiResolutionRegistrationPipeline::GetStartingShrinkFactor()
{
	return this->registration->GetStartingShrinkFactor();
}

void MultiResolutionRegistrationPipeline::SetStartingShrinkFactor(unsigned int factor)
{
	this->registration->SetStartingShrinkFactor(factor);
	// For the pyramid image filter that's used in registration:
	// Gaussian smoothing is first performed using a 
	// DiscreteGaussianImageFilter with variance (shrink factor / 2)^2
	double var = factor * factor * 0.25;
	this->smooth->SetVariance(factor * factor * 0.25);
	// The smooth filter doesn't seem to update itself when its variance
	// changes.  This is odd...what else would change on this filter?
	this->smooth->Modified();
}

unsigned int MultiResolutionRegistrationPipeline::GetNumberOfLevels()
{
	return this->registration->GetNumberOfLevels();
}

void MultiResolutionRegistrationPipeline::SetNumberOfLevels(unsigned int levels)
{
	this->registration->SetNumberOfLevels(levels);
}

MultiResolutionRegistrationPipeline::ImageType::PixelType
MultiResolutionRegistrationPipeline::GetUpperThreshold()
{
	return this->threshold[0]->GetUpper();
}

void MultiResolutionRegistrationPipeline::SetUpperThreshold(ImageType::PixelType threshold)
{
	this->threshold[0]->SetUpper(threshold);
	this->threshold[1]->SetUpper(threshold);
}

MultiResolutionRegistrationPipeline::ImageType::PixelType
MultiResolutionRegistrationPipeline::GetLowerThreshold()
{
	return this->threshold[0]->GetLower();
}

void MultiResolutionRegistrationPipeline::SetLowerThreshold(ImageType::PixelType threshold)
{
	this->threshold[0]->SetLower(threshold);
	this->threshold[1]->SetLower(threshold);
}

MultiResolutionRegistrationPipeline::ImageType::Pointer
MultiResolutionRegistrationPipeline::GetPreviewImage()
{
	return this->smooth->GetOutput();
}

void MultiResolutionRegistrationPipeline::Update()
{
	Logger::verbose << "MultiResolutionRegistrationPipeline::Update()" << std::endl;

	// Create container for transforms
	typedef std::vector<RegistrationType::TransformPointer> TransformVector;
	TransformVector transforms;
	
	// Create a running transform--keeps transform from current image to initial image
	RegistrationType::TransformPointer transform = RegistrationType::TransformType::New();
	transform->SetIdentity();

	int total = this->outputFiles.size();
	Logger::info << "MultiResolutionRegistrationPipeline::Update: " << 1 << "/" << total << std::endl;

	// Start at the beginning of the input images
	// Write the first image, un-changed
	this->caster->SetInput(this->inputReader[0]);
	WriteImage(this->caster->GetOutput(), this->outputFiles[0]);

	// Connect caster to the resampling pipeline
	this->caster->SetInput(this->resample->GetOutput());

	// Register every image with the previous image
	for (unsigned int i = 0; 
	     i < this->inputReader.size()-1 && 
	     i < this->outputFiles.size(); 
	     i++)
	{
            ImageType::Pointer fixed = this->inputReader[i];
            ImageType::Pointer moving = this->inputReader[i+1];
            
            // Update pipeline inputs
            this->threshold[0]->SetInput(fixed);
            this->threshold[1]->SetInput(moving);
            this->smooth->SetInput(this->threshold[0]->GetOutput());
            this->resample->SetInput(moving);
            
            // Run registration
            Logger::info << "MultiResolutionRegistrationPipeline::Update: " << (i+2) << "/" << total << std::endl;
            this->registration->StartRegistration();
            // store transform
            transforms.push_back(this->registration->GetLastTransform());
            // pre-compose the current transform with the accumulated transform
            transform->Compose(this->registration->GetLastTransform(), true);
            
            // Save registered image
            this->resample->SetTransform(transform);
            this->resample->SetSize(fixed->GetLargestPossibleRegion().GetSize());
            this->resample->SetOutputOrigin(fixed->GetOrigin());
            this->resample->SetOutputSpacing(fixed->GetSpacing());
            this->resample->SetDefaultPixelValue(0);
            this->resample->Update();
            WriteImage(this->caster->GetOutput(), this->outputFiles[i]);
	}

	// Save transform data
	if (this->GetTransformFile() != "")
	{
		TransformGroup::SaveTransforms(&transforms, this->GetTransformFile());
	}

	// Reset image
	this->resample->SetInput(this->inputReader[0]);
	this->threshold[0]->SetInput(this->inputReader[0]);
	this->threshold[1]->SetInput(this->inputReader[1]);
	this->smooth->SetInput(this->threshold[0]->GetOutput());
}
