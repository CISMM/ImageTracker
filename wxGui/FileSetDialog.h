#pragma once
#include <vector>
#include <wx/wxprec.h>
#include "RegistrationDesign_wdr.h"
#include "../vtkView/VtkCanvas.h"
#include "../FileSet.h"


/* todo Consoldiate constants in one file? */
static const wxChar *FSD_FILETYPES = _T("All Files|*.*|"
                                        "TIFF Files|*.tif;*.tiff|"
                                        "PNG Files|*.png|"
                                        "JPG Files|*.jpg");

// WDR: class declarations

//----------------------------------------------------------------------------
// FileSetDialog
//----------------------------------------------------------------------------

class FileSetDialog: public wxPanel
{
public:
    // constructors and destructors
    FileSetDialog( wxWindow *parent, wxWindowID id=-1, 
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER );
    virtual ~FileSetDialog();
    
    void SetCanvas(VtkCanvas *canvas);
    VtkCanvas* GetCanvas();
    FileSet* GetFileSet();

    // WDR: method declarations for FileSetDialog
    wxListBox* GetListFiles()  { return (wxListBox*) FindWindow( ID_LIST_FILES ); }

    void OnAdd( wxCommandEvent &event );

private:
    // WDR: member variable declarations for FileSetDialog
    
private:
    // WDR: handler declarations for FileSetDialog
    void OnListFilesDoubleClick( wxCommandEvent &event );
    void OnRemove( wxCommandEvent &event );

    wxString directory;
    VtkCanvas *canvas;

private:
    DECLARE_EVENT_TABLE()
};
