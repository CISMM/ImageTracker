#include "MultiImageRegistrationProcessor.h"

#include "FileSetImageReader.h"

MultiImageRegistrationProcessor::MultiImageRegistrationProcessor(void)
{
    this->transforms = new TransformVector();
    this->SetAlignmentMode(MultiImageRegistrationProcessor::ALIGN_WITH_PREVIOUS);
}

MultiImageRegistrationProcessor::~MultiImageRegistrationProcessor(void)
{
}

void MultiImageRegistrationProcessor::SetSource(FileSet* source)
{
    this->source = source;
    registrationComplete = false;
}

void MultiImageRegistrationProcessor::SetImageRegistration(ImageRegistration* registrar)
{
    this->registrar = registrar;
    registrationComplete = false;
}

MultiImageRegistrationProcessor::TransformVector* MultiImageRegistrationProcessor::GetTransforms(void)
{
    if (this->registrationComplete)
    {
        return this->transforms;
    }
    
    //else
    return NULL;
}

MultiImageRegistrationProcessor::TransformVector* MultiImageRegistrationProcessor::Update(void)
{
    registrationComplete = false;

    if (this->source == NULL || 
        this->registrar == NULL ||
        this->source->GetFileNames()->size() < 2)
    {
        //todo: probably want to throw an exception here.
        std::cerr << "MultiImageRegistrationProcessor::Update() Warning: source or registrar improperly configured." << std::endl;
        return NULL;
    }

    // There are at least two ways to process a set of images:
    // ALIGN_WITH_FIRST: Register each image with the first image, starting with the
    // transform of the previous image.
    // ALIGN_WITH_PREVIOUS: Register each image with the previous image in the set.  This 
    // method makes play back more difficult--each transform must be combined with all 
    // previous transforms.

    FileSetImageReader* pReader = new FileSetImageReader(this->source);

    //First transform
    std::cout << "Reading => " << pReader->CurrentFileName() << std::endl;
    ImageRegistration::ImageType::Pointer firstImage = pReader->CurrentImage();
    ImageRegistration::ImageType::Pointer fixedImage = firstImage;
    ImageRegistration::ImageType::Pointer movingImage;

    std::cout << "Reading => " << pReader->NextFileName() << std::endl;
    movingImage = pReader->CurrentImage();

    ImageRegistration::TransformPointer transform = 
        this->registrar->Register(fixedImage, movingImage);
    this->transforms->push_back(transform);

    //Register with all remaining images
    while (pReader->HasNext())
    {
        if (this->alignMode == MultiImageRegistrationProcessor::ALIGN_WITH_PREVIOUS)
        {
            fixedImage = movingImage; // previous moving image
            
            std::cout << "Reading => " << pReader->NextFileName() << std::endl;
            movingImage = pReader->CurrentImage();

            transform = this->registrar->Register(fixedImage, movingImage);
        }
        else // default: if (this->alignMode == MultiImageRegistrationProcessor::ALIGN_WITH_FIRST)
        {
            std::cout << "Reading => " << pReader->NextFileName() << std::endl;
            movingImage = pReader->CurrentImage();

            transform = this->registrar->Register(firstImage, movingImage, transform);
        }

        this->transforms->push_back(transform);
    }

    registrationComplete = true;
    delete(pReader);

    return this->GetTransforms();
}

void MultiImageRegistrationProcessor::SetAlignmentMode(int mode)
{
    this->alignMode = mode;
}