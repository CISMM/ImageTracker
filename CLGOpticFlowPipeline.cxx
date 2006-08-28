#include "CLGOpticFlowPipeline.h"
// #include "ImageWindow.h"

#include "Logger.h"

void CLGOpticFlowPipeline::InitializePipeline()
{
    if ((!this->source.size() == 0 || this->destination.size() == 0))
    {
        this->reader = ReaderType(this->source);
        this->index = 0;

        this->init = true;
    }
    else
    {
        Logger::logWarn("CLGOpticFlowPipeline: Source and/or destination files have not been setup.");
        this->init = false;
    }
}

bool CLGOpticFlowPipeline::UpdateOne()
{
    if (!this->init)
    {
        this->InitializePipeline();
    }

    if (this->init &&
        this->index < (this->source.size() - 1) &&
        this->index < this->destination.size())
    {
        this->flowFilter->SetInput1(this->reader[this->index]);
        this->flowFilter->SetInput2(this->reader[this->index+1]);

        Logger::logInfo("Writing vector image: " + this->destination[this->index]);
        WriteImage(this->flowFilter->GetOutput(), this->destination[this->index]);

        this->index++;
        return true;
    }

    this->init = false;
    return false; // No frames to process.
}

void CLGOpticFlowPipeline::UpdateAll()
{
    while (this->UpdateOne());
}
