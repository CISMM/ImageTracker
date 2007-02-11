#include "MultiResolutionRegistrationPipeline.h"

#include <math.h>
#include <vector>

#include "itkArray2D.h"

#include "CommonTypes.h"
#include "Logger.h"
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

void MultiResolutionRegistrationPipeline::SetInput(ImageSetReaderBase* input)
{
	this->inputReader = input;

	this->resample->SetInput(dynamic_cast<ImageTypeF2*>(this->inputReader->GetImage(0)));
        this->threshold[0]->SetInput(dynamic_cast<ImageTypeF2*>(this->inputReader->GetImage(0)));
        this->threshold[1]->SetInput(dynamic_cast<ImageTypeF2*>(this->inputReader->GetImage(1)));

	this->smooth->SetInput(this->threshold[0]->GetOutput());
}

void MultiResolutionRegistrationPipeline::SetPreviewShrinkFactor(unsigned int factor)
{
    double var = factor * factor * 0.25;
    this->smooth->SetVariance(var);
    this->smooth->Modified();
}

void MultiResolutionRegistrationPipeline::SetShrinkFactors(unsigned int min, unsigned int max)
{
    std::string function("MultiResolutionRegistrationPipeline::SetShrinkFactors");
    Logger::debug << function << ":\tmin = " << min << "\tmax = " << max << std::endl;
    
    // We can use logs and exponents to compute the useful shrink levels--there is no reason to
    // use more levels than what you get from halving the shrink level at each step.
    int dims = ImageType::ImageDimension;
    int levels = (int) (std::ceil(log2(max)) - std::floor(log2(min)) + 1);
    Logger::debug << function << ":\tlevels = " << levels << std::endl;
    
    RegistrationType::ImagePyramidType::ScheduleType sched(levels, dims);
    unsigned int shrink, next, level;
    
    Logger::debug << function << ":\tschedule =\n\t";
    for (shrink = max, level = 0; shrink >= 1 && shrink >= min; level++)
    {
        for (unsigned int d = 0; d < dims; d++)
        {
            sched[level][d] = shrink;
            Logger::debug << sched[level][d] << "\t";
        }
        Logger::debug << "\n\t";
        
        next = (unsigned int) exp2(std::floor(log2(shrink-1)));
        if (next < min && shrink > min) // the minimum occurs before the next smaller power of 2
        {
            for (unsigned int d = 0; d < dims; d++)
            {
                sched[level+1][d] = min;
                Logger::debug << sched[level+1][d] << "\t";
            }
        }
        
        shrink = next;
    }
    
    Logger::debug << std::endl;
    
    this->registration->SetShrinkSchedule(sched);
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
    std::string function("MultiResolutionRegistrationPipeline::Update()");
    Logger::verbose << function << std::endl;

    // Create container for transforms
    typedef std::vector<RegistrationType::TransformPointer> TransformVector;
    TransformVector transforms;
    
    // Store an initial identity transformation for the first image--it registers with itself perfectly
    RegistrationType::TransformPointer transform = RegistrationType::TransformType::New();
    transform->SetIdentity();
    transforms.push_back(transform);
    // Create a running transform--keeps transform from current image to initial image
    transform = RegistrationType::TransformType::New();
    transform->SetIdentity();

    int total = this->outputFiles.size();
    Logger::info << function << ": " << 1 << "/" << total << std::endl;

    // Start at the beginning of the input images
    // Write the first image, un-changed
    this->caster->SetInput(dynamic_cast<ImageTypeF2*>(this->inputReader->GetImage(0)));
    WriteImage(this->caster->GetOutput(), this->outputFiles[0]);

    // Connect caster to the resampling pipeline
    this->caster->SetInput(this->resample->GetOutput());

    // Register every image with the previous image
    for (unsigned int i = 0; 
            i < this->inputReader->size()-1 && 
            i < this->outputFiles.size()-1; 
            i++)
    {
        ImageType::Pointer fixed = dynamic_cast<ImageTypeF2*>(this->inputReader->GetImage(i));
        ImageType::Pointer moving = dynamic_cast<ImageTypeF2*>(this->inputReader->GetImage(i+1));
        
        // Update pipeline inputs
        this->threshold[0]->SetInput(fixed);
        this->threshold[1]->SetInput(moving);
        this->smooth->SetInput(this->threshold[0]->GetOutput());
        this->resample->SetInput(moving);
        
        // Run registration
        Logger::info << function << ": " << (i+2) << "/" << total << std::endl;
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
        WriteImage(this->caster->GetOutput(), this->outputFiles[i+1]);
    }

    // Save transform data
    if (this->GetTransformFile() != "")
    {
            TransformGroup::SaveTransforms(&transforms, this->GetTransformFile());
    }

    // Reset image
    this->resample->SetInput(dynamic_cast<ImageTypeF2*>(this->inputReader->GetImage(0)));
    this->threshold[0]->SetInput(dynamic_cast<ImageTypeF2*>(this->inputReader->GetImage(0)));
    this->threshold[1]->SetInput(dynamic_cast<ImageTypeF2*>(this->inputReader->GetImage(1)));
    this->smooth->SetInput(this->threshold[0]->GetOutput());
}
