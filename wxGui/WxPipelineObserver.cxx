#include "WxPipelineObserver.h"

#include "Logger.h"
#include "wxUtils.h"

WxPipelineObserver::WxPipelineObserver() :
    wasAborted(false),
    dlgProgress(_T("Pipeline Progress"), _T(""), 100, NULL, wxPD_CAN_ABORT | wxPD_REMAINING_TIME)
{
    std::string function("WxPipelineObserver::WxPipelineObserver");
    Logger::debug << function << ": constructed." << std::endl;
    this->dlgProgress.Show(true);
}
    
WxPipelineObserver::~WxPipelineObserver() 
{
    std::string function("WxPipelineObserver::~WxPipelineObserver");
    Logger::debug << function << ": destructing...." << std::endl;
//    this->dlgProgress.Destroy();
    // The wxProgressDialog seems to have some odd properties.
    // If the pipeline was allowed to complete without canceling, the
    // progress dialog can be Destroy()ed.  But, if the cancel button
    // was pressed during pipeline execution, the Destroy() method
    // results in a null pointer exception when the dialog's resources
    // are being cleaned up.
    // If the dialog box is simply hidden (Show(false)), it doesn't get 
    // destroyed when the application terminates, so the application stays
    // in memory.  Also, this would constitute a memory leak, as each pipeline
    // that is launched gets a new progress dialog.
    // But, it seems like if the C++ delete operation is called, the dialog
    // goes away, and the application can shut down properly.  Although wxWidgets
    // folks don't recommend this, that's what we do here, because nothing else
    // seems to work.
//     if (this->dlgProgress)
//     {
//         delete this->dlgProgress;
//         if (this->wasAborted)
//             this->dlgProgress->Show(false);
//         else
//             this->dlgProgress->Destroy();
        
//         this->dlgProgress = NULL;
//     }
}

bool WxPipelineObserver::Update(double progress, const std::string& message)
{
    bool doContinue = true;
    // We prevent the progress box from reaching 100% progress to avoid the 
    // events that get triggered when that happens.
    int prog = std::min(99, (int) (progress * 100));
    // int prog = (int) (progress * 100);
    
    // This may not be on the main thread, so wrap in a gui mutex
    wxMutexGuiEnter();
    doContinue = this->dlgProgress.Update(prog, nano::std2wx(message));
    wxMutexGuiLeave();
    
    Logger::verbose << "WxPipelineObserver::Update: " << prog << ", " << message << ", " << 
            (doContinue ? "continuing" : "aborting") << std::endl;
    
    this->wasAborted = !doContinue;
    return !doContinue; // convert to abort indicator
}
