#include <iostream>

#include "RegistrationOutput.h"

RegistrationOutput::RegistrationOutput(FileSet* source /* = NULL */, 
        TransformGroup::TransformVector* transforms /* = NULL */, 
        FileSet* destination /* = NULL */,
        int composeMode /* = COMPOSE_NONE */)
{
    this->source = source;
    this->transforms = transforms;
    this->destination = destination;
    this->SetComposeMode(composeMode);
}

RegistrationOutput::~RegistrationOutput(void)
{
}

void RegistrationOutput::SetSource(FileSet* source)
{
    this->source = source;
}

void RegistrationOutput::SetTransforms(
    TransformGroup::TransformVector* newTransforms)
{
    this->transforms = newTransforms;
}

void RegistrationOutput::SetDestination(FileSet* destination)
{
    this->destination = destination;
}

void RegistrationOutput::Update(void)
{
    //Ensure everything is set up
    if (this->source == NULL ||
        this->transforms == NULL ||
        this->destination == NULL)
    {
        std::cerr << "RegistrationOutput: invalid state.  Image source, transforms, or destination not set." << std::endl;
        return;
    }

    //There ought to be one fewer transformation than image--we start transforming on the second image.
    if ((this->source->GetFileNames()->size() - 1 != this->transforms->size()) ||
        (this->destination->GetFileNames()->size() < (signed) this->transforms->size()))
    {
        std::cout << "RegistrationOutput: source, destination, and transform types are not lining up exactly: " <<
            "\n\tSource image count => " << this->source->GetFileNames()->size() <<
            "\n\tTransforms count   => " << this->transforms->size() <<
            "\n\tDest image count   => " << this->destination->GetFileNames()->size() << std::endl;
    }

    TransformGroup::TransformVector::iterator tranIt = transforms->begin();
    TransformGroup::TransformPointer transform = TransformGroup::TransformType::New();
    transform->SetIdentity();

    FileSet::FileIterator sourceIt = source->GetFileNames()->begin();
    FileSet::FileIterator destIt = destination->GetFileNames()->begin();

    ReaderType::Pointer reader = ReaderType::New();
    ResampleFilterType::Pointer resampler = ResampleFilterType::New();
    CastFilterType::Pointer caster = CastFilterType::New();
    WriterType::Pointer writer = WriterType::New();

    resampler->SetInput(reader->GetOutput());
    caster->SetInput(resampler->GetOutput());
    writer->SetInput(caster->GetOutput());

    // Write the first image with no transformation
    std::string inFile = source->GetDirectory() + *sourceIt;
    std::string outFile = destination->GetDirectory() + *destIt;
    reader->SetFileName(inFile.c_str());
    reader->Update();
    resampler->SetTransform(transform);
    resampler->SetSize(reader->GetOutput()->GetLargestPossibleRegion().GetSize());
    resampler->SetOutputOrigin(reader->GetOutput()->GetOrigin());
    resampler->SetOutputSpacing(reader->GetOutput()->GetSpacing());
    resampler->SetDefaultPixelValue(0);
    writer->SetFileName(outFile.c_str());
    writer->Update();

    *sourceIt++; *destIt++;

    for ( ; sourceIt != source->GetFileNames()->end() &&
            destIt != destination->GetFileNames()->end() &&
            tranIt != transforms->end() ; 
            *sourceIt++, *destIt++, *tranIt++)
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

        // Write the first image with no transformation
        inFile = source->GetDirectory() + *sourceIt;
        outFile = destination->GetDirectory() + *destIt;
        reader->SetFileName(inFile.c_str());
        reader->Update();
        resampler->SetTransform(transform);
        resampler->SetSize(reader->GetOutput()->GetLargestPossibleRegion().GetSize());
        resampler->SetOutputOrigin(reader->GetOutput()->GetOrigin());
        resampler->SetOutputSpacing(reader->GetOutput()->GetSpacing());
        resampler->SetDefaultPixelValue(0);
        writer->SetFileName(outFile.c_str());
        writer->Update();
    }
}

void RegistrationOutput::SetComposeMode(int mode)
{
    this->composeMode = mode;
}
