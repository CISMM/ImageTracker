#include "DialogLogger.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// LoggerDialog
//----------------------------------------------------------------------------

// WDR: event table for LoggerDialog

BEGIN_EVENT_TABLE(LoggerDialog,wxDialog)
    EVT_BUTTON( LOG_CLEAR, LoggerDialog::OnClear )
    EVT_BUTTON( LOG_HIDE, LoggerDialog::OnHide )
    EVT_BUTTON( LOG_COPY, LoggerDialog::OnCopy )
END_EVENT_TABLE()

LoggerDialog::LoggerDialog( wxWindow *parent, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style ) :
    wxDialog( parent, id, title, position, size, style )
{
    // WDR: dialog function CreateLoggerDialog for LoggerDialog
    CreateLoggerDialog( this, TRUE ); 
}

LoggerDialog::~LoggerDialog()
{
    this->logger = (DialogLogger *) NULL;
}

void LoggerDialog::SetLogger(DialogLogger* logger)
{
    this->logger = logger;
}

// WDR: handler implementations for LoggerDialog

void LoggerDialog::OnCopy( wxCommandEvent &event )
{
    this->GetTextMessages()->SetSelection(-1, -1);
    this->GetTextMessages()->Copy();
    Logger::logInfo("Text copied to clipboard.");
}

void LoggerDialog::OnHide( wxCommandEvent &event )
{
    if (this->logger)
    {
        this->logger->setVisible(false);
    }

    this->Show(false);
}

void LoggerDialog::OnClear( wxCommandEvent &event )
{
    this->GetTextMessages()->Clear();
}

DialogLogger::DialogLogger(LoggerDialog* dialog, bool show)
{
    if (!dialog)
    {
        this->dialog = new LoggerDialog(NULL, -1, _("Logger"));
    }
    else
    {
        this->dialog = dialog;
    }
    this->dialog->SetLogger(this);
    this->setVisible(show);
}

DialogLogger::~DialogLogger()
{
    delete this->dialog;
}

void DialogLogger::doLog(const std::string &msg)
{
    wxString text(msg.c_str());
    text.append("\n");
    this->dialog->GetTextMessages()->AppendText(text);

    if (this->isVisible())
    {
        this->dialog->Show();
    }
}