#include "MultiRegionRegistrationPipeline.h"

#include "ImageSetReader.h"
#include "ImageUtils.h"
#include "HarrisFeatureDetector.h"
#include "Logger.h"

MultiRegionRegistrationPipeline::MultiRegionRegistrationPipeline(void)
{
}

MultiRegionRegistrationPipeline::~MultiRegionRegistrationPipeline(void)
{
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
    if (!(this->source.size() == 0 ||
          this->destination.size() == 0))
    {
        std::cerr << "MultiRegionRegistrationPipeline::Update() Warning: source or destination not configured." << std::endl;
        return;
    }

    ReaderType reader(this->source);
    HarrisFeatureDetector* detector = new HarrisFeatureDetector(2000, 1.2);
    MultiRegionRegistration::OutputImageType::Pointer outImage;
    
    for (unsigned int i = 0; 
        i < this->source.size() - 1 && i < this->destination.size(); 
        i++)
    {
        registrar->SetFixedImage(reader[i+1]);
        registrar->SetMovingImage(reader[i]);

        if (findFeatures) // Only compute motion where there is enough information
        {
            HarrisFeatureDetector::PointSetType::Pointer features = 
                detector->findFeatures(reader[i]);
            outImage = registrar->Update(features);
        }
        else
        {
            outImage = registrar->Update();
        }
        
        // Write image to file
        Logger::logInfo("Writing vector image: " + this->destination[i]);
        WriteImage<MultiRegionRegistration::OutputImageType>(outImage, this->destination[i]);
    }

    delete(detector);
}
