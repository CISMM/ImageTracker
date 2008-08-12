#include "MultiResolutionRegistrationPipeline.h"

#include <cmath>
#include <vector>

#include "itkArray2D.h"

#include "Logger.h"
#include "MathUtils.h"
#include "TransformGroup.h"
#include "ImageUtils.h"

MultiResolutionRegistrationPipeline::MultiResolutionRegistrationPipeline()
{
	// Create pipeline objects
	this->registration = RegistrationType::New();
	this->resample = ResampleType::New();
	this->smooth = SmoothType::New();

	// User must supply these
	this->SetTransformFile("");
}

void MultiResolutionRegistrationPipeline::SetInput(ImageFileSet* input)
{
	this->input = input;
	this->smooth->SetInput(this->input->GetOutput());
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

MultiResolutionRegistrationPipeline::ImageType::Pointer
MultiResolutionRegistrationPipeline::GetPreviewImage()
{
	return this->smooth->GetOutput();
}

void MultiResolutionRegistrationPipeline::Update()
{
    std::string function("MultiResolutionRegistrationPipeline::Update()");
    Logger::verbose << function << std::endl;
    
    typedef ImageTypeUS2 OutputImageType;
    
    int total = this->outputFiles.size();
    bool abort = this->NotifyProgress(0.0 / total, "Initializing...");
    
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

    Logger::info << function << ": " << 1 << "/" << total << std::endl;

    // Start at the beginning of the input images
    // Write the first image, un-changed
    WriteImage<ImageType, OutputImageType>(this->input->GetImage(0), this->outputFiles[0], false);

    abort = this->NotifyProgress(1.0 / total, "Registering...");
    
    // Register every image with the previous image
    ImageType::Pointer fixed = NULL;
    ImageType::Pointer moving = NULL;
    for (unsigned int i = 0; 
         i < this->input->GetImageCount()-1 && 
             i < this->outputFiles.size()-1 &&
             !abort; 
         i++)
    {
        // We need to create a copy of the fixed image so grabbing
        // the moving image doesn't clobber it
        fixed = CopyImage(this->input->GetImage(i));
        moving = this->input->GetImage(i+1);
        
        // Update pipeline inputs
        this->registration->SetFixedImage(fixed);
        this->registration->SetMovingImage(moving);
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
        WriteImage<ImageType, OutputImageType>(this->resample->GetOutput(), this->outputFiles[i+1], false);
        
        abort = this->NotifyProgress(((double) i+2) / total);
    }

    // Save transform data
    if (this->GetTransformFile() != "")
    {
        TransformGroup::SaveTransforms(&transforms, this->GetTransformFile());
    }

    // At this point, we have succeeded if we were not stopped.
    this->SetSuccess(!abort);
}
