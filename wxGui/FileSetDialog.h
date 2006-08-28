#pragma once

#include <vector>

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "RegistrationDesign_wdr.h"
#include "VtkCanvas.h"
#include "FileSet.h"

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
    const FileSet& GetFileSet();

    // WDR: method declarations for FileSetDialog
    wxListBox* GetListFiles()  { return (wxListBox*) FindWindow( ID_LIST_FILES ); }

    void OnAdd( wxCommandEvent &event );

private:
    // WDR: member variable declarations for FileSetDialog
    
private:
    // WDR: handler declarations for FileSetDialog
    void OnListFilesDoubleClick( wxCommandEvent &event );
    void OnRemove( wxCommandEvent &event );

    VtkCanvas *canvas;
    FileSet files;

private:
    DECLARE_EVENT_TABLE()
};
