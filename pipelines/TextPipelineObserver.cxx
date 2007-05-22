#include "TextPipelineObserver.h"

#include "Logger.h"

bool TextPipelineObserver::Update(double progress, const std::string& message)
{
    std::string function("TextPipelineObserver::Update");
    Logger::info << 
            function << ": " << 
            (100*progress) << "%, " << 
            message << std::endl;
    
    return false;
}
