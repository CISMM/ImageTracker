// -*- C++ -*- generated by wxGlade 0.4.1 on Thu Oct 19 12:42:02 2006

#include "DataSourceDialog.h"

#include "FileSet.h"
#include "wxUtils.h"

static const std::string IMAGE_FILE_FILTER(
    "All Files|*.*|TIFF|*.tif;*.tiff|PNG|*.png|JPG|*.jpg|mha|*.mha|vtk|*.vtk");

bool DataSourceDialog::TransferDataToWindow()
{
    this->txtName->SetValue(std2wx(this->dataSource->GetName()));
    switch (this->dataSource->GetPixelDataType())
    {
        case ScalarChar:
            rbxPixelType->SetSelection(0);
            break;
        case ScalarShort:
            rbxPixelType->SetSelection(1);
            break;
        case ScalarFloat:
            rbxPixelType->SetSelection(2);
            break;
        case VectorFloat:
            rbxPixelType->SetSelection(3);
            break;
    }
    
    this->lbxFiles->Clear();
    FileSet files = this->dataSource->GetFiles();
    for (unsigned int i = 0; i < files.size(); i++)
    {
        this->lbxFiles->Append(std2wx(files[i]));
    }
    
    if (this->IsNewSource())
    {
        wxCommandEvent evt;
        this->OnAddFiles(evt);
    }
    
    return true;
}

bool DataSourceDialog::TransferDataFromWindow()
{
    bool success = false;
    
    // Update data source values
    this->dataSource->SetName(wx2std(this->txtName->GetValue()));
    switch (rbxPixelType->GetSelection())
    {
        case 0:
            this->dataSource->SetPixelDataType(ScalarChar);
            break;
        case 1:
            this->dataSource->SetPixelDataType(ScalarShort);
            break;
        case 2:
            this->dataSource->SetPixelDataType(ScalarFloat);
            break;
        case 3:
            this->dataSource->SetPixelDataType(VectorFloat);
            break;
    }
    
    if (this->lbxFiles->GetCount() > 0)
    {    
        FileSet files;
        for (int i = 0; i < this->lbxFiles->GetCount(); i++)
        {
            files.Add(wx2std(this->lbxFiles->GetString(i)));
        }
        
        this->dataSource->SetFiles(files);
        success = true;
    }
    else
    {
        wxMessageDialog dlg(this, wxT("Please select at least one image file for this data source."), wxT("No files"), wxOK);
        dlg.ShowModal();
        success = false;
    }
    
    return success;
}

void DataSourceDialog::SetDataSource(DataSource::Pointer source)
{
    this->dataSource = source;
}

DataSource::Pointer DataSourceDialog::GetDataSource()
{
    return this->dataSource;
}

void DataSourceDialog::ClearDataSource()
{
    this->dataSource = DataSource::New();
}

DataSourceDialog::DataSourceDialog(wxWindow* parent, int id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
    wxDialog(parent, id, title, pos, size, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxTHICK_FRAME)
{
    // begin wxGlade: DataSourceDialog::DataSourceDialog
    sizer_11_staticbox = new wxStaticBox(this, -1, wxT("Files"));
    label_2 = new wxStaticText(this, -1, wxT("Name"));
    txtName = new wxTextCtrl(this, -1, wxT(""));
    const wxString rbxPixelType_choices[] = {
        wxT("scalar 8-bit"),
        wxT("scalar 16-bit"),
        wxT("scalar float"),
        wxT("vector float")
    };
    rbxPixelType = new wxRadioBox(this, -1, wxT("Pixel Type"), wxDefaultPosition, wxDefaultSize, 4, rbxPixelType_choices, 0, wxRA_SPECIFY_ROWS);
    const wxString lbxFiles_choices[] = {
        wxT("")
    };
    lbxFiles = new wxListBox(this, -1, wxDefaultPosition, wxDefaultSize, 1, lbxFiles_choices, wxLB_EXTENDED);
    btnAddFiles = new wxButton(this, BTN_ADD_FILES, wxT("+"));
    btnRemoveFiles = new wxButton(this, BTN_REMOVE_FILES, wxT("-"));
    btnOk = new wxButton(this, wxID_OK, wxT("&OK"));
    btnCancel = new wxButton(this, wxID_CANCEL, wxT("&Cancel"));

    set_properties();
    do_layout();
    // end wxGlade
    
    // TODO: implement vector float support
    // Disable vector floats, for now
    // rbxPixelType->Enable(3, false);
    
    // This is pretty messed up right here.  Windows complains if the choices array for a
    // list box is empty.  If you pass an empty wxString as the only element, everything
    // is ok.  But, then you have a list item by default, which is bad.  So, we have to 
    // clear it here.  Wow.
    this->lbxFiles->Clear();
}


BEGIN_EVENT_TABLE(DataSourceDialog, wxDialog)
    // begin wxGlade: DataSourceDialog::event_table
    EVT_BUTTON(BTN_ADD_FILES, DataSourceDialog::OnAddFiles)
    EVT_BUTTON(BTN_REMOVE_FILES, DataSourceDialog::OnRemoveFiles)
    // end wxGlade
END_EVENT_TABLE();


void DataSourceDialog::OnAddFiles(wxCommandEvent &event)
{
    wxFileDialog open(this, wxT("Choose image files"), wxT(""), wxT(""), 
                      std2wx(IMAGE_FILE_FILTER), 
                      wxOPEN | wxFILE_MUST_EXIST | wxMULTIPLE | wxCHANGE_DIR);
    if (open.ShowModal() == wxID_OK)
    {
        wxArrayString paths;
        open.GetPaths(paths);
        paths.Sort();
        this->lbxFiles->Append(paths);
    }
}


void DataSourceDialog::OnRemoveFiles(wxCommandEvent &event)
{
    wxArrayInt idxs;
    int count = this->lbxFiles->GetSelections(idxs);
    if (count > 0)
    {
        // Remove all elements in reverse order to preserve indices
        for (int i = count-1; i >= 0; i--)
        {
            this->lbxFiles->Delete(idxs[i]);
        }
    }
}


// wxGlade: add DataSourceDialog event handlers


void DataSourceDialog::set_properties()
{
    // begin wxGlade: DataSourceDialog::set_properties
    SetTitle(wxT("Edit Data Source"));
    SetSize(wxSize(380, 480));
    txtName->SetToolTip(wxT("A name to describe this data source"));
    rbxPixelType->SetToolTip(wxT("The pixel type of the images in this data source"));
    rbxPixelType->SetSelection(1);
    lbxFiles->SetToolTip(wxT("The image files in this data source"));
    lbxFiles->SetSelection(0);
    btnAddFiles->SetToolTip(wxT("Add images to this data source"));
    btnRemoveFiles->SetToolTip(wxT("Remove selected images from this data source"));
    btnOk->SetToolTip(wxT("Apply changes"));
    btnOk->SetDefault();
    btnCancel->SetToolTip(wxT("Reject changes"));
    // end wxGlade
}


void DataSourceDialog::do_layout()
{
    // begin wxGlade: DataSourceDialog::do_layout
    wxBoxSizer* sizer_5 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_13 = new wxBoxSizer(wxHORIZONTAL);
    wxStaticBoxSizer* sizer_11 = new wxStaticBoxSizer(sizer_11_staticbox, wxVERTICAL);
    wxBoxSizer* sizer_12 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_7 = new wxBoxSizer(wxHORIZONTAL);
    sizer_7->Add(label_2, 0, wxRIGHT|wxADJUST_MINSIZE, 5);
    sizer_7->Add(txtName, 1, wxADJUST_MINSIZE, 0);
    sizer_5->Add(sizer_7, 0, wxTOP|wxEXPAND|wxALIGN_CENTER_HORIZONTAL, 10);
    sizer_5->Add(rbxPixelType, 0, wxTOP|wxBOTTOM|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxADJUST_MINSIZE, 10);
    sizer_11->Add(lbxFiles, 1, wxEXPAND|wxADJUST_MINSIZE, 0);
    sizer_12->Add(btnAddFiles, 0, wxEXPAND|wxADJUST_MINSIZE, 0);
    sizer_12->Add(btnRemoveFiles, 0, wxEXPAND|wxADJUST_MINSIZE, 0);
    sizer_11->Add(sizer_12, 0, wxALIGN_CENTER_HORIZONTAL, 0);
    sizer_5->Add(sizer_11, 2, wxEXPAND|wxALIGN_CENTER_HORIZONTAL, 0);
    sizer_13->Add(btnOk, 0, wxADJUST_MINSIZE, 0);
    sizer_13->Add(btnCancel, 0, 0, 0);
    sizer_5->Add(sizer_13, 0, wxTOP|wxALIGN_CENTER_HORIZONTAL, 20);
    SetAutoLayout(true);
    SetSizer(sizer_5);
    Layout();
    // end wxGlade
}

