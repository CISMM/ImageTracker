#include "MultiRegionRegistrationPipeline.h"

#include "itkImageFileWriter.h"

#include "FileSetImageReader.h"

MultiRegionRegistrationPipeline::MultiRegionRegistrationPipeline(void)
{
}

MultiRegionRegistrationPipeline::~MultiRegionRegistrationPipeline(void)
{
}

void MultiRegionRegistrationPipeline::SetSource(FileSet* source)
{
    this->source = source;
}

void MultiRegionRegistrationPipeline::SetDestination(FileSet* destination)
{
    this->destination = destination;
}

FileSet* MultiRegionRegistrationPipeline::GetSource()
{
    return this->source;
}

FileSet* MultiRegionRegistrationPipeline::GetDestination()
{
    return this->destination;
}

void MultiRegionRegistrationPipeline::SetRegistrar(MultiRegionRegistration* registrar)
{
    this->registrar = registrar;
}

MultiRegionRegistration* MultiRegionRegistrationPipeline::GetRegistrar()
{
    if (this->registrar == NULL)
    {
        this->registrar = new MultiRegionRegistration();
    }
    return this->registrar;
}

void MultiRegionRegistrationPipeline::Update()
{
    if (this->source == NULL ||
        this->destination == NULL)
    {
        std::cerr << "MultiRegionRegistrationPipeline::Update() Warning: source or destination not configured." << std::endl;
    }

    FileSetImageReader* pReader = new FileSetImageReader(this->source);
    FileSetImageReader::ImageType::Pointer pCurrent = pReader->CurrentImage();

    typedef itk::ImageFileWriter<MultiRegionRegistration::OutputImageType> WriterType;
    WriterType::Pointer writer = WriterType::New();
    FileSet::FileIterator outIt = this->destination->GetFileNames()->begin();

    FileSetImageReader::ImageType::Pointer pNext = NULL;
    MultiRegionRegistration::OutputImageType::Pointer outImage = NULL;

    for ( ; pReader->HasNext() &&
            outIt != this->destination->GetFileNames()->end(); 
            *outIt++)
    {
        pNext = pReader->NextImage();

        registrar->SetFixedImage(pNext);
        registrar->SetMovingImage(pCurrent);
        outImage = registrar->Update();

        // Write image to file
        writer->SetFileName(this->destination->FullFileName(outIt).c_str());
        writer->SetInput(outImage);
        writer->Update();
        
        pCurrent = pNext;
    }

    delete(pReader);
}
