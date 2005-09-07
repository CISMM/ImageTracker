#include "DialogLogger.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// LoggerDialog
//----------------------------------------------------------------------------

// WDR: event table for LoggerDialog

BEGIN_EVENT_TABLE(LoggerDialog,wxPanel)
END_EVENT_TABLE()

LoggerDialog::LoggerDialog( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxPanel( parent, id, position, size, style )
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

DialogLogger::DialogLogger(LoggerDialog* dialog, bool show)
{
    if (!dialog)
    {
        this->dialog = new LoggerDialog(NULL, -1);
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
