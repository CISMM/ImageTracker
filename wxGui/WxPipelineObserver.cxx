#include "WxPipelineObserver.h"

#include "Logger.h"
#include "wxUtils.h"

bool WxPipelineObserver::Update(double progress, const std::string& message)
{
    bool notAbort = true;
    if (this->dlgProgress)
    {
        int prog = (int) (progress * 90);
        notAbort = this->dlgProgress->Update(prog, std2wx(message));
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
