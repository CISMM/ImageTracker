#include "CLGOpticFlowPipeline.h"
#include "./vtkView/ImageWindow.h"

#include "Logger.h"

void CLGOpticFlowPipeline::InitializePipeline()
{
    if (this->source && this->destination)
    {
        this->reader = new FileSetImageReader(this->source);
        this->writer = WriterType::New();
        this->writer->SetInput(this->flowFilter->GetOutput());

        this->destinationIt = this->destination->GetFileNames()->begin();
        
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
        this->reader->HasNext() &&
        this->destinationIt != this->destination->GetFileNames()->end())
    {
        // ImageWindow::ImShow(this->reader->CurrentImage(), "Current");
        this->flowFilter->SetInput1((ImageType::ConstPointer) (this->reader->CurrentImage()));
        this->flowFilter->SetInput2((ImageType::ConstPointer) (this->reader->NextImage()));
        // ImageWindow::ImShow(this->reader->CurrentImage(), "Next");

        std::string filename = *(this->destinationIt);
        this->writer->SetFileName(filename.c_str());
        Logger::logInfo("Writing vector image: " + filename);
        writer->Update();

        *(this->destinationIt)++;

        return true;
    }

    this->init = false;
    return false; // No frames to process.
}

void CLGOpticFlowPipeline::UpdateAll()
{
    while (this->UpdateOne());
}
