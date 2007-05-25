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
        int prog = (int) (progress * 100);
        
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
