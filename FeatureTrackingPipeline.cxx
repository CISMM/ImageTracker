#include ".\FeatureTrackingPipeline.h"
#include "Logger.h"

void FeatureTrackingPipeline::InitializePipeline()
{
    if (this->source && this->destination)
    {
        this->reader = new FileSetImageReader(this->source);
        this->pCurrentImg = this->reader->FirstImage();
        this->writer = WriterType::New();
        this->destinationIt = this->destination->GetFileNames()->begin();

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
        this->reader->HasNext() &&
        this->destinationIt != this->destination->GetFileNames()->end())
    {
        this->pCurrentImg = this->reader->CurrentImage();
        this->pNextImg = this->reader->NextImage();

        this->registrar->SetMovingImage(this->pCurrentImg);
        this->registrar->SetFixedImage(this->pNextImg);
        
        std::string filename = *(this->destinationIt);
        this->writer->SetFileName(filename.c_str());
        this->writer->SetInput(
            this->registrar->Update(this->detector->findFeatures(this->pCurrentImg)));
        Logger::logInfo("Writing vector image: " + filename);
        writer->Update();

        *(this->destinationIt)++;

        return true;
    }

    return false;
}

void FeatureTrackingPipeline::UpdateAll()
{
    while (this->UpdateOne());
}
