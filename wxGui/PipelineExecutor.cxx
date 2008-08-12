#include "PipelineExecutor.h"

#include <string>

#include "FileSet.h"
#include "ImageTrackerController.h"
#include "Logger.h"
#include "TextPipelineObserver.h"
#include "WxPipelineObserver.h"

PipelineExecutor::PipelineExecutor(ItkPipeline* pipeline, wxThreadKind kind) :
    wxThread(kind),
	openResult(false),
    imageResult(true)
{
	std::string function("PipelineExecutor::PipelineExecutor");
	Logger::verbose << function << " (" << wxThread::GetCurrentId() << "): constructing..." << std::endl;
    this->pipeline = pipeline;
    if (this->pipeline != NULL)
    {
		Logger::verbose << function << ": Creating pipeline observer and attaching to pipeline" << std::endl;
        this->progress = WxPipelineObserver::New();
        this->pipeline->AddObserver(this->progress);
    }
}

PipelineExecutor::~PipelineExecutor()
{
	std::string function("PipelineExecutor::~PipelineExecutor");
	Logger::verbose << function << ": destructing..." << std::endl;
    if (this->pipeline != NULL && this->progress.IsNotNull())
    {
		Logger::verbose << function << ": removing observer from pipeline" << std::endl;
        this->pipeline->RemoveObserver(progress);
    }
	Logger::verbose << function << ": exiting" << std::endl;
}

void PipelineExecutor::SetOpenFiles(bool open)
{
	this->openResult = open;
}

void PipelineExecutor::SetImageResult(bool isImage)
{
    this->imageResult = isImage;
}

PipelineExecutor::ExitCode PipelineExecutor::Entry()
{
	std::string function("PipelineExecutor::Entry");
	Logger::debug << function << ": starting..." << std::endl;

    if (this->pipeline != NULL)
    {
		Logger::debug << function << ": updating pipeline..." << std::endl;
        this->pipeline->Update();
    }

    // Open the result, if we're supposed to
    if (this->pipeline->GetSuccess() &&
        this->openResult)
    {
		Logger::debug << function << ": loading output images through the backdoor" << std::endl;
		ImageTrackerController::Instance()->SetBackdoorFiles(this->pipeline->GetOutputFiles(), this->imageResult);
    }

	Logger::debug << function << ": finished." << std::endl;
    return (ExitCode) 0;
}
