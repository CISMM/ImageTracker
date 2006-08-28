#include <iostream>

#include "RegistrationOutput.h"
#include "ImageUtils.h"

void RegistrationOutput::Update(void)
{
    //Ensure everything is set up
    if (this->source.size() == 0 ||
        this->transforms.size() == 0 ||
        this->destination.size() == 0)
    {
        std::cerr << "RegistrationOutput: invalid state.  Image source, transforms, or destination not set." << std::endl;
        return;
    }

    //There ought to be one fewer transformation than image--we start transforming on the second image.
    if ((this->source.size() - 1 != this->transforms.size()) ||
        (this->destination.size() < (signed) this->transforms.size()))
    {
        std::cout << "RegistrationOutput: source, destination, and transform types are not lining up exactly: " <<
            "\n\tSource image count => " << this->source.size() <<
            "\n\tTransforms count   => " << this->transforms.size() <<
            "\n\tDest image count   => " << this->destination.size() << std::endl;
    }

    TransformGroup::TransformVector::iterator tranIt = transforms.begin();
    TransformGroup::TransformPointer transform = TransformGroup::TransformType::New();
    transform->SetIdentity();

    ReaderType reader(this->source);

    InputImageType::Pointer image = reader[0];
    ResampleFilterType::Pointer resampler = ResampleFilterType::New();
    CastFilterType::Pointer caster = CastFilterType::New();

    resampler->SetInput(image);
    caster->SetInput(resampler->GetOutput());

    // Write the first image with no transformation
    resampler->SetTransform(transform);
    resampler->SetSize(image->GetLargestPossibleRegion().GetSize());
    resampler->SetOutputOrigin(image->GetOrigin());
    resampler->SetOutputSpacing(image->GetSpacing());
    resampler->SetDefaultPixelValue(0);
    WriteImage(caster->GetOutput(), this->destination[0]);

    for (unsigned int i = 1;
         i < reader.size() &&
         i < destination.size() &&
         tranIt != transforms.end();
         i++, ++tranIt)
    {
        if (this->composeMode == RegistrationOutput::COMPOSE_POST) 
        {
            transform->Compose(*tranIt, false);
        }
        if (this->composeMode == RegistrationOutput::COMPOSE_PRE)
        {
            transform->Compose(*tranIt, true);
        }
        else // if (this->composeMode == RegistrationOutput::COMPOSE_NONE)
        {
            transform = *tranIt;
        }
        
        image = reader[i];
        resampler->SetInput(image);
        resampler->SetTransform(transform);
        resampler->SetSize(image->GetLargestPossibleRegion().GetSize());
        resampler->SetOutputOrigin(image->GetOrigin());
        resampler->SetOutputSpacing(image->GetSpacing());
        resampler->SetDefaultPixelValue(0);
        WriteImage(caster->GetOutput(), this->destination[i]);
    }
}
