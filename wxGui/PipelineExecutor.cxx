#include "PipelineExecutor.h"

#include "DataSource.h"
#include "FileSet.h"
#include "ImageTrackerController.h"
#include "Logger.h"

PipelineExecutor::PipelineExecutor(ItkPipeline* pipeline, wxThreadKind kind) :
    wxThread(kind),
	openResult(false)
{
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

void PipelineExecutor::SetOpenFiles(bool open)
{
	this->openResult = open;
}

PipelineExecutor::ExitCode PipelineExecutor::Entry()
{
    Logger::verbose << "PipelineExecutor::Entry" << std::endl;

    if (this->pipeline != NULL)
    {
        this->pipeline->Update();
    }

    // Open the result, if we're supposed to
    if (this->openResult &&
        this->pipeline->GetSuccess())
    {
        DataSource::Pointer source = DataSource::New();
        source->SetName("Result");
        source->SetFiles(this->pipeline->GetOutputFiles());
        
        // Add the new data source to the controller
        ImageTrackerController::Instance()->AddDataSource(source);
    }

    Logger::verbose << "PipelineExecutor::Entry: done" << std::endl;
    return (ExitCode) 0;
}
