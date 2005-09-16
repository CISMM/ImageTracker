#include "MultiRegionRegistrationPipeline.h"

#include "itkImageFileWriter.h"

#include "FileSetImageReader.h"
#include "HarrisFeatureDetector.h"
#include "Logger.h"

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

void MultiRegionRegistrationPipeline::Update(bool findFeatures)
{
    if (this->source == NULL ||
        this->destination == NULL)
    {
        std::cerr << "MultiRegionRegistrationPipeline::Update() Warning: source or destination not configured." << std::endl;
    }

    FileSetImageReader* pReader = new FileSetImageReader(this->source);
    FileSetImageReader::InternalImageType::Pointer pCurrent = pReader->CurrentImage();

    typedef itk::ImageFileWriter<MultiRegionRegistration::OutputImageType> WriterType;
    WriterType::Pointer writer = WriterType::New();
    FileSet::FileIterator outIt = this->destination->GetFileNames()->begin();

    FileSetImageReader::InternalImageType::Pointer pNext = NULL;
    MultiRegionRegistration::OutputImageType::Pointer outImage = NULL;
    HarrisFeatureDetector* detector = new HarrisFeatureDetector(2000, 1.2);

    for ( ; pReader->HasNext() &&
            outIt != this->destination->GetFileNames()->end(); 
            *outIt++)
    {
        pNext = pReader->NextImage();

        registrar->SetFixedImage(pNext);
        registrar->SetMovingImage(pCurrent);

        if (findFeatures) // Only compute motion where there is enough information
        {
            HarrisFeatureDetector::PointSetType::Pointer features = 
                detector->findFeatures(pCurrent);
            outImage = registrar->Update(features);
        }
        else
        {
            outImage = registrar->Update();
        }
        
        // Write image to file
        std::string filename = *outIt;
        Logger::logInfo("Writing vector image: " + filename);
        writer->SetFileName(filename.c_str());
        writer->SetInput(outImage);
        writer->Update();
        
        pCurrent = pNext;
    }

    delete(pReader);
    delete(detector);
}
