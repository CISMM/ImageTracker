/////////////////////////////////////////////////////////////////////////////
// Name:        DialogLogger.h
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include <wx/wxprec.h>
#include "../Logger.h"
#include "RegistrationDesign_wdr.h"

class DialogLogger;

// WDR: class declarations

//----------------------------------------------------------------------------
// LoggerDialog
//----------------------------------------------------------------------------

/*
 * Dialog that contains a text control for logging messages.
 */
class LoggerDialog : public wxDialog
{
public:
    // constructors and destructors
    LoggerDialog( wxWindow *parent, wxWindowID id, const wxString &title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER );
    ~LoggerDialog();
    // WDR: method declarations for LoggerDialog
    wxTextCtrl* GetTextMessages()  { return (wxTextCtrl*) FindWindow( LOG_MESSAGES ); }
    
    /*
     * Set the ILog implementation that logs.
     */
    void SetLogger(DialogLogger* logger);
    
private:
    // WDR: member variable declarations for LoggerDialog

    DialogLogger* logger;
private:
    // WDR: handler declarations for LoggerDialog
    void OnCopy( wxCommandEvent &event );
    void OnHide( wxCommandEvent &event );
    void OnClear( wxCommandEvent &event );

private:
    DECLARE_EVENT_TABLE()
};

/*
 * ILog implementation that logs messages to a wxDialog (LoggerDialog).
 */
class DialogLogger : public ILog
{
public:
    /*
     * Default constructor.  If a dialog is not supplied, one will be created.
     */
    DialogLogger(LoggerDialog* dialog = (LoggerDialog *) NULL, bool show=true);

    /*
     * Destructor.
     */
    ~DialogLogger();

protected:
    void doLog(const std::string &msg);

private:
    LoggerDialog* dialog;
};