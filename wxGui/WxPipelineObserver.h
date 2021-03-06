#pragma once

#include <string>

#include <wx/wx.h>
#include <wx/progdlg.h>

#include "itkObjectFactory.h"
#include "ItkPipelineObserver.h"

/**
 * \class WxPipelineObserver
 * \brief wxWidgets-based itk pipeline progress observer.
 *
 * WxPipelineObserver controls a wxProgressDialog that updates based on ItkPipeline
 * progress events.
 */
class WxPipelineObserver : 
    public ItkPipelineObserver
{
public:
    typedef WxPipelineObserver Self;
    typedef ItkPipelineObserver Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    itkNewMacro(Self);
    itkTypeMacro(WxPipelineObserver, ItkPipelineObserver);
    
    virtual bool Update(double progress, const std::string& message);
    
protected:
    WxPipelineObserver();
    virtual ~WxPipelineObserver();
    
    
private:
    // not implemented
    WxPipelineObserver(const Self& other);
    void operator=(const Self& other);
    
    wxProgressDialog dlgProgress;
    bool wasAborted;
};
