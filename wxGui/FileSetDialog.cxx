#include ".\FileSetDialog.h"
#include "..\Logger.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// FileSetDialog
//----------------------------------------------------------------------------

// WDR: event table for FileSetDialog

BEGIN_EVENT_TABLE(FileSetDialog,wxPanel)
    EVT_BUTTON( FSD_REMOVE, FileSetDialog::OnRemove )
    EVT_BUTTON( FSD_ADD, FileSetDialog::OnAdd )
    EVT_LISTBOX( ID_LIST_FILES, FileSetDialog::OnListFilesDoubleClick )
END_EVENT_TABLE()

FileSetDialog::FileSetDialog( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxPanel( parent, id, position, size, style )
{
    this->directory = "";
    // WDR: dialog function CreateFileSetDialog for FileSetDialog
    CreateFileSetDialog( this, TRUE ); 
}

FileSetDialog::~FileSetDialog()
{
}

void FileSetDialog::SetCanvas(VtkCanvas *canvas)
{
    this->canvas = canvas;
}

VtkCanvas* FileSetDialog::GetCanvas()
{
    return this->canvas;
}

FileSet* FileSetDialog::GetFileSet()
{
    FileSet* files = new FileSet();
    files->SetDirectory(this->directory.c_str());

    FileSet::FileVector* vFiles = new FileSet::FileVector();
    for (int i = 0; i < this->GetListFiles()->GetCount(); i++)
    {
        wxString file(this->GetListFiles()->GetString(i));
        vFiles->push_back(file.c_str());
    }

    files->SetFileNames(vFiles);
    return files;
}

// WDR: handler implementations for FileSetDialog

void FileSetDialog::OnListFilesDoubleClick( wxCommandEvent &event )
{
    wxString filename = "";
    wxArrayInt sels;
    int count = this->GetListFiles()->GetSelections(sels);
    if (count > 0)
    {
        int sel = (int) sels.Last();
        filename.append(this->directory).append(this->GetListFiles()->GetString(sel));
        this->GetCanvas()->SetFileName(filename.c_str());
    }
}

void FileSetDialog::OnAdd( wxCommandEvent &event )
{
    wxFileDialog addFiles(this, "Open files", 
        this->directory, "", 
        FSD_FILETYPES, wxOPEN | wxMULTIPLE);

    if (addFiles.ShowModal() == wxID_OK)
    {
        wxArrayString files;
        wxArrayString paths;
        addFiles.GetFilenames(files);
        addFiles.GetPaths(paths);

        this->directory = wxString(addFiles.GetDirectory());
        this->directory.append("\\");

        for (unsigned int i = 0; i < files.GetCount(); i++)
        {
            const wxString file(files.Item(i));
            const wxString* path = new wxString(paths.Item(i));
            this->GetListFiles()->Append(file, (void *) path);
        }
    }
}

void FileSetDialog::OnRemove( wxCommandEvent &event )
{
    wxArrayInt selections;
    if (this->GetListFiles()->GetSelections(selections) > 0)
    {
        int count = selections.GetCount();
        int total = this->GetListFiles()->GetCount();
        wxString msg;
        msg.Printf("%d of %d files selected for deletion.", count, total);
        Logger::logDebug(msg.c_str());

        for (unsigned int i = 0; i < selections.GetCount(); i++)
        {
            // Have to subtract i off the selected item to account for 
            // items already deleted.
            this->GetListFiles()->Delete(selections.Item(i) - i);
        }
    }
}
