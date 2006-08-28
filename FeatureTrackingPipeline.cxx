#include "FeatureTrackingPipeline.h"
#include "Logger.h"

void FeatureTrackingPipeline::InitializePipeline()
{
    if (!(this->source.size() == 0 || this->destination.size() == 0))
    {
        this->reader = ReaderType(this->source);
        this->index = 0;
        this->init = true;
    }
    else
    {
        Logger::logWarn("FeatureTrackingPipeline: Source and destination files have not been setup.");
        this->init = false;
    }
}

bool FeatureTrackingPipeline::UpdateOne()
{
    if (!this->init)
    {
        this->InitializePipeline();
    }

    if (this->init && 
        this->index < this->reader.size() - 1 &&
        this->index < this->destination.size())
    {
        this->registrar->SetMovingImage(this->reader[this->index]);
        this->registrar->SetFixedImage(this->reader[this->index+1]);
        
        Logger::logInfo("Writing vector image: " + this->destination[this->index]);
        WriteImage<MultiRegionRegistration::OutputImageType>(
            this->registrar->Update(
                this->detector->findFeatures(this->reader[this->index])),
            this->destination[this->index]);

        this->index++;

        return true;
    }

    return false;
}

void FeatureTrackingPipeline::UpdateAll()
{
    while (this->UpdateOne());
}
