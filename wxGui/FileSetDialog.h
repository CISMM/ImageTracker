#pragma once
#include <vector>
#include <wx/wxprec.h>
#include "ImageCanvas.h"
#include "RegistrationDesign_wdr.h"
#include "../FileSet.h"


/* todo Consoldiate constants in one file? */
static const wxChar *FSD_FILETYPES = _T("PNG Files|*.png|"
                                        "TIFF Files|*.tif;*.tiff|"
                                        "JPG Files|*.jpg|"
                                        "All Files|*.*");

// WDR: class declarations

//----------------------------------------------------------------------------
// FileSetDialog
//----------------------------------------------------------------------------

class FileSetDialog: public wxDialog
{
public:
    // constructors and destructors
    FileSetDialog( wxWindow *parent, wxWindowID id=-1, 
        const wxString &title = _T("Open files"),
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER );
    virtual ~FileSetDialog();
    
    void SetCanvas(ImageCanvas *canvas);
    ImageCanvas* GetCanvas();
    FileSet* GetFileSet();

    // WDR: method declarations for FileSetDialog
    wxTextCtrl* GetTextDir()  { return (wxTextCtrl*) FindWindow( ID_TEXT_DIR ); }
    wxListBox* GetListFiles()  { return (wxListBox*) FindWindow( ID_LIST_FILES ); }
    
private:
    // WDR: member variable declarations for FileSetDialog

    void UpdateDialog();
    
private:
    // WDR: handler declarations for FileSetDialog
    void OnListFilesDoubleClick( wxCommandEvent &event );
    void OnAdd( wxCommandEvent &event );
    void OnRemove( wxCommandEvent &event );
    void OnOk( wxCommandEvent &event );

    wxString directory;
    ImageCanvas *canvas;

private:
    DECLARE_EVENT_TABLE()
};
