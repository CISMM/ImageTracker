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
	this->smooth = SmoothType::New();

	// Connect pipeline
	this->registration->SetFixedImage(this->threshold[0]->GetOutput());
	this->registration->SetMovingImage(this->threshold[1]->GetOutput());
	this->smooth->SetInput(this->threshold[0]->GetOutput());

	// User must supply these
	// this->inputFiles = 0;
	// this->outputFiles = 0;
	this->SetTransformFile("");
}

FileSet* MultiResolutionRegistrationPipeline::GetInputFiles()
{
	return &(this->inputFiles);
}

void MultiResolutionRegistrationPipeline::SetInputFiles(FileSet &files)
{
	this->inputFiles = files;
	this->inputReader.SetFileSet(&(this->inputFiles));

	this->resample->SetInput(this->inputReader.FirstImage());
	this->threshold[0]->SetInput(this->inputReader.FirstImage());
	this->threshold[1]->SetInput(this->inputReader.NextImage());

	this->smooth->SetInput(this->threshold[0]->GetOutput());
}

FileSet* MultiResolutionRegistrationPipeline::GetOutputFiles()
{
	return &(this->outputFiles);
}

void MultiResolutionRegistrationPipeline::SetOutputFiles(FileSet& files)
{
	this->outputFiles = files;
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
	typedef std::vector<typename RegistrationType::TransformPointer> TransformVector;
	TransformVector transforms;
	
	// Create a running transform--keeps transform from current image to initial image
	RegistrationType::TransformPointer transform = RegistrationType::TransformType::New();
	transform->SetIdentity();

	int total = this->outputFiles.GetFileNames()->size();
	int idx = 1;
	Logger::info << "MultiResolutionRegistrationPipeline::Update: " << idx << "/" << total << std::endl;

	// Retrieve output filenames
	FileSet::FileIterator outFileIt = this->outputFiles.GetFileNames()->begin();
	ItkMagickIO::Pointer writer = ItkMagickIO::New();

	// Start at the beginning of the input images
	ImageType::Pointer fixed = this->inputReader.FirstImage();
	ImageType::Pointer moving;

	// Write the first image, un-changed	
	writer->Write(*outFileIt, fixed);
	if (outFileIt != this->outputFiles.GetFileNames()->end())
		++outFileIt;

	// Register every image with the previous image
	while (this->inputReader.HasNext())
	{
		moving = this->inputReader.NextImage();

		// Update pipeline inputs
		this->threshold[0]->SetInput(fixed);
		this->threshold[1]->SetInput(moving);
		this->smooth->SetInput(fixed);
		this->resample->SetInput(moving);

		// Run registration
		Logger::info << "MultiResolutionRegistrationPipeline::Update: " << (++idx) << "/" << total << std::endl;
		this->registration->StartRegistration();
		// store transform
		transforms.push_back(this->registration->GetLastTransform());
		// pre-compose the current transform with the accumulated transform
		transform->Compose(this->registration->GetLastTransform(), true);
		
		// Save registered image
		if (outFileIt != this->outputFiles.GetFileNames()->end())
		{
			this->resample->SetTransform(transform);
			this->resample->SetSize(fixed->GetLargestPossibleRegion().GetSize());
			this->resample->SetOutputOrigin(fixed->GetOrigin());
			this->resample->SetOutputSpacing(fixed->GetSpacing());
			this->resample->SetDefaultPixelValue(0);
			this->resample->Update();
			writer->Write(*outFileIt, this->resample->GetOutput());
			++outFileIt;
		}

		// The current moving image is the next fixed image
		fixed = moving;
	}

	// Save transform data
	if (this->GetTransformFile() != "")
	{
		TransformGroup::SaveTransforms(&transforms, this->GetTransformFile());
	}

	// Reset image
	this->resample->SetInput(this->inputReader.FirstImage());
	this->threshold[0]->SetInput(this->inputReader.FirstImage());
	this->threshold[1]->SetInput(this->inputReader.NextImage());
	this->smooth->SetInput(this->threshold[0]->GetOutput());
}