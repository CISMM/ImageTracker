#include "WxPipelineObserver.h"

#include "Logger.h"
#include "wxUtils.h"

WxPipelineObserver::WxPipelineObserver()
    : dlgProgress(0)
{
    this->CreateProgressDialog();
}
    
WxPipelineObserver::~WxPipelineObserver() 
{
    if (this->dlgProgress)
    {
        this->dlgProgress->Destroy();
        this->dlgProgress = NULL;
    }
}

bool WxPipelineObserver::Update(double progress, const std::string& message)
{
    bool notAbort = true;
    if (this->dlgProgress)
    {
        // We prevent the progress box from reaching 100% progress to avoid the 
        // events that get triggered when that happens.
        int prog = std::min(99, (int) (progress * 100));
        
        // This may not be on the main thread, so wrap in a gui mutex
        wxMutexGuiEnter();
        notAbort = this->dlgProgress->Update(prog, std2wx(message));
        wxMutexGuiLeave();
        
        Logger::verbose << "WxPipelineObserver::Update: " << prog << ", " << message << ", " << 
                (notAbort ? "continuing" : "aborting") << std::endl;
    }
    return !notAbort;
}

void WxPipelineObserver::CreateProgressDialog(const std::string& title, const std::string& message, int maximum, 
                                              wxWindow* parent, int style)
{
    if (this->dlgProgress)
    {
        this->dlgProgress->Destroy();
        this->dlgProgress = NULL;
    }
    
    this->dlgProgress = new wxProgressDialog(std2wx(title), std2wx(message), maximum, parent, style);
    this->dlgProgress->Show(true);
}
