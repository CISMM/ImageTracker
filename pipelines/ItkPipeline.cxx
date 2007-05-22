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

bool ItkPipeline::Notify(double progress, const std::string& message)
{
    bool abort = false, abortIt = false;
    
    // Notify all observers of the progress
    ObserverList::const_iterator it;
    for (it = this->observers.begin(); it != this->observers.end(); ++it)
    {
        (*it)->Update(progress, message, &abortIt);
        // determine if any observer has requested an abort
        abort = (abort || abortIt);
    }
    
    return abort;
}
