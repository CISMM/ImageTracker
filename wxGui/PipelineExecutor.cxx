#include "PipelineExecutor.h"

#include "DataSource.h"
#include "FileSet.h"
#include "Logger.h"

PipelineExecutor::PipelineExecutor(ItkPipeline* pipeline, wxThreadKind kind) :
    wxThread(kind),
	openResult(false)
{
    this->controller = NULL;
    Logger::verbose << "PipelineExecutor::PipelineExecutor" << std::endl;
     this->pipeline = pipeline;
     if (this->pipeline != NULL)
     {
         this->progress = WxPipelineObserver::New();
         this->pipeline->AddObserver(this->progress);
     }
}

PipelineExecutor::~PipelineExecutor()
{
    Logger::verbose << "PipelineExecutor::~PipelineExecutor" << std::endl;
     if (this->pipeline != NULL && this->progress.IsNotNull())
     {
         this->pipeline->RemoveObserver(progress);
     }
}

void PipelineExecutor::SetOpenFiles(bool open, ImageTrackerController* controller)
{
	this->openResult = open;
	this->controller = controller;
}

PipelineExecutor::ExitCode PipelineExecutor::Entry()
{
    Logger::verbose << "PipelineExecutor::Entry" << std::endl;

    if (this->pipeline != NULL)
    {
        this->pipeline->Update();
    }

    // Open the result, if we're supposed to
    if (this->openResult && this->controller != NULL)
    {
        DataSource::Pointer source = DataSource::New();
        source->SetName("Result");
        source->SetFiles(this->pipeline->GetOutputFiles());
        this->controller->AddDataSource(source);
    }

    Logger::verbose << "PipelineExecutor::Entry: done" << std::endl;
    return (ExitCode) 0;
}
