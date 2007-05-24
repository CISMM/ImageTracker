#include "PipelineExecutor.h"
#include "Logger.h"

PipelineExecutor::PipelineExecutor(ItkPipeline* pipeline, wxThreadKind kind) :
    wxThread(kind)
{
    Logger::verbose << "PipelineExecutor::PipelineExecutor" << std::endl;
//     this->pipeline = pipeline;
//     if (this->pipeline != NULL)
//     {
//         this->progress = WxPipelineObserver::New();
//         this->pipeline->AddObserver(this->progress);
//     }
}

PipelineExecutor::~PipelineExecutor()
{
    Logger::verbose << "PipelineExecutor::~PipelineExecutor" << std::endl;
//     if (this->pipeline != NULL && this->progress.IsNotNull())
//     {
//         this->pipeline->RemoveObserver(progress);
//     }
}

PipelineExecutor::ExitCode PipelineExecutor::Entry()
{
    // Logger::verbose << "PipelineExecutor::Entry" << std::endl;
    int i = 0;
    while (i < 100000000)
    {
        i = i + 2;
        i = i - 1;
    }
    // Logger::verbose << "PipelineExecutor::Entry: i = " << i << std::endl;
    
//     if (this->pipeline != NULL)
//     {
//         this->pipeline->Update();
//     }
    return (ExitCode) i;
}
