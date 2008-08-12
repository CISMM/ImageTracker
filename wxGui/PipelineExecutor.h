#pragma once

#include <wx/wx.h>
#include <wx/thread.h>

#include "ItkPipeline.h"
#include "ItkPipelineObserver.h"

/**
 * \class PipelineExecutor
 * \brief Handles execution of an ItkPipeline on a separate thread.
 *
 * Manages an ItkPipeline including creating and attaching a progress dialog
 * observer, executing the pipeline on its own thread, and removing the progress
 * dialog observer.
 */
class PipelineExecutor : 
    public wxThread
{
public:
    /**
     * Create a PipelineExecutor for a given pipeline.
     */
    PipelineExecutor(ItkPipeline* pipeline, wxThreadKind kind = wxTHREAD_DETACHED);
    virtual ~PipelineExecutor();
    
    /**
     * Called by wxWidgets when this thread is started.  Starts the ItkPipeline
     * execution.
     */
    virtual ExitCode Entry();

    void SetOpenFiles(bool open);
    void SetImageResult(bool isImage);
    
protected:
    // The pipeline to execute on this thread.
    ItkPipeline* pipeline;
    
    // The progress observer attached to this thread's pipeline.
    ItkPipelineObserver::Pointer progress;
    
private:
    bool openResult;
    bool imageResult;
};
