#include "TextPipelineObserver.h"

#include "Logger.h"

TextPipelineObserver::TextPipelineObserver()
{
	std::string function("TextPipelineObserver::TextPipelineObserver");
	Logger::debug << function << ": constructed." << std::endl;
}

TextPipelineObserver::~TextPipelineObserver()
{
	std::string function("TextPipelineObserver::~TextPipelineObserver");
	Logger::debug << function << ": destructed." << std::endl;
}

bool TextPipelineObserver::Update(double progress, const std::string& message)
{
    std::string function("TextPipelineObserver::Update");
    Logger::info << 
            function << ": " << 
            (100*progress) << "%, " << 
            message << std::endl;
    
    return false;
}
