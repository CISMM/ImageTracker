#include "TextPipelineObserver.h"

#include "Logger.h"

void TextPipelineObserver::Update(double progress, const std::string& message, bool* abort)
{
    std::string function("TextPipelineObserver::Update");
    Logger::info << 
            function << ": " << 
            (100*progress) << "%, " << 
            message << std::endl;
    *abort = false;
}
