#include "ItkPipeline.h"

#include <algorithm>

void ItkPipeline::AddObserver(ItkPipelineObserver* observer)
{
    this->observers.push_back(observer);
}

void ItkPipeline::RemoveObserver(ItkPipelineObserver* observer)
{
    this->observers.remove(observer);
}

bool ItkPipeline::NotifyProgress(double progress, const std::string& message)
{
    bool abort = false, abortIt = false;
    
    // Notify all observers of the progress
    ObserverList::const_iterator it;
    for (it = this->observers.begin(); it != this->observers.end(); ++it)
    {
        if (*it) // check if the object is still there
            abortIt = (*it)->Update(progress, message);
        // determine if any observer has requested an abort
        abort = (abort || abortIt);
    }
    
    return abort;
}

ImageSetReaderBase* ItkPipeline::GetInput()
{
    return this->input;
}

void ItkPipeline::SetInput(ImageSetReaderBase *input)
{
    this->input = input;
}

const FileSet& ItkPipeline::GetOutputFiles()
{
    return this->outputFiles;
}

void ItkPipeline::SetOutputFiles(const FileSet &files)
{
    this->outputFiles = files;
}