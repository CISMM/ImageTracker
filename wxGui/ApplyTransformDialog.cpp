// -*- C++ -*- generated by wxGlade 0.4.1 on Fri Feb  9 12:45:04 2007

#include "ApplyTransformDialog.h"
        
#include "FileSet.h"
#include "Logger.h"
#include "PipelineExecutor.h"
#include "wxUtils.h"

bool ApplyTransformDialog::TransferDataToWindow()
{
    // Set up UI controls based on input information
    this->textTransform->SetValue(std2wx(this->input->GetFiles().GetDirectory() + "transforms.txt"));
    this->textDirectory->SetValue(std2wx(this->input->GetFiles().GetDirectory()));
    
    return true;
}

bool ApplyTransformDialog::TransferDataFromWindow()
{
    std::string function("ApplyTransformDialog::TransferDataFromWindow");
    Logger::verbose << function << ": entering" << std::endl;
    
    // Set up pipeline paramters
    this->pipeline->SetInput(this->input->GetImages());
    this->pipeline->SetTransformFile(wx2std(this->textTransform->GetValue()));
    
    // Create an output file set
    FileSet outputFiles(this->input->GetFiles(), wx2std(this->textPrefix->GetValue()));
    outputFiles.SetDirectory(wx2std(this->textDirectory->GetValue()));
    this->pipeline->SetOutputFiles(outputFiles);
    
    // Create and launch a pipeline executor (uses another thread)
    PipelineExecutor* exec = new PipelineExecutor(this->pipeline);
    exec->SetOpenFiles(this->checkOpenOutput->IsChecked());
    if (exec->Create() == wxTHREAD_NO_ERROR)
    {
        exec->Run();
    }
    else
    {
        Logger::warning << function << ": Unable able to create threaded pipeline execution object" << std::endl;
        delete exec;
    }
    
    this->Show(false);
    return true;
}

void ApplyTransformDialog::SetInput(DataSource::Pointer input)
{
    this->input = input;
}

ApplyTransformDialog::ApplyTransformDialog(wxWindow* parent, int id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
    wxDialog(parent, id, title, pos, size, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxTHICK_FRAME)
{
    // begin wxGlade: ApplyTransformDialog::ApplyTransformDialog
    sizer_31_staticbox = new wxStaticBox(this, -1, wxT("Output"));
    sizer_30_staticbox = new wxStaticBox(this, -1, wxT("Parameters"));
    label_25 = new wxStaticText(this, -1, wxT("Transform File"));
    textTransform = new wxTextCtrl(this, -1, wxT("transforms.txt"));
    btnBrowse = new wxButton(this, BTN_BROWSE_TRANSFORM, wxT("Browse..."));
    label_26 = new wxStaticText(this, -1, wxT("Directory"));
    textDirectory = new wxTextCtrl(this, -1, wxT(""));
    btnBrowseDirectory = new wxButton(this, BTN_BROWSE_DIRECTORY, wxT("Browse..."));
    label_27 = new wxStaticText(this, -1, wxT("Prefix"));
    textPrefix = new wxTextCtrl(this, -1, wxT("tran-"));
    checkOpenOutput = new wxCheckBox(this, -1, wxT("Open output when finished"));
    btnRun = new wxButton(this, wxID_OK, wxT("&Run"));
    btnHide = new wxButton(this, wxID_CANCEL, wxT("&Hide"));

    set_properties();
    do_layout();
    // end wxGlade
    
    //----- Custom code -----//
    this->pipeline = ApplyTransformsPipeline::New();
}

BEGIN_EVENT_TABLE(ApplyTransformDialog, wxDialog)
    // begin wxGlade: ApplyTransformDialog::event_table
    EVT_BUTTON(BTN_BROWSE_TRANSFORM, ApplyTransformDialog::OnBrowseTransform)
    EVT_BUTTON(BTN_BROWSE_DIRECTORY, ApplyTransformDialog::OnBrowseDirectory)
    EVT_BUTTON(wxID_OK, ApplyTransformDialog::OnRun)
    EVT_BUTTON(wxID_CANCEL, ApplyTransformDialog::OnHide)
    // end wxGlade
END_EVENT_TABLE();

void ApplyTransformDialog::OnBrowseTransform(wxCommandEvent &event)
{
    wxFileDialog open(this, wxT("Choose a transform file"), wxT(""), wxT(""), wxT("All Files|*.*|Text Files|*.txt"));
    if (open.ShowModal() == wxID_OK)
    {
        this->textTransform->SetValue(open.GetFilename());
    }
}

void ApplyTransformDialog::OnBrowseDirectory(wxCommandEvent &event)
{
    wxDirDialog dir(this, wxT("Choose an output directory"), this->textDirectory->GetValue());
    if (dir.ShowModal() == wxID_OK)
    {
		this->textDirectory->SetValue(dir.GetPath().Append(std2wx(FileSet::PATH_DELIMITER)));
    }
}

void ApplyTransformDialog::OnRun(wxCommandEvent &event)
{
	event.Skip();
}

void ApplyTransformDialog::OnHide(wxCommandEvent &event)
{
	event.Skip();
}

// wxGlade: add ApplyTransformDialog event handlers

void ApplyTransformDialog::set_properties()
{
    // begin wxGlade: ApplyTransformDialog::set_properties
    SetTitle(wxT("Apply Transform"));
    SetSize(wxSize(550, 250));
    textTransform->SetToolTip(wxT("A transform file from an image stabilization operation"));
    btnBrowse->SetToolTip(wxT("Find a transform file"));
    textDirectory->SetToolTip(wxT("The directory in which to save transformed images"));
    btnBrowseDirectory->SetToolTip(wxT("Find an output directory"));
    textPrefix->SetToolTip(wxT("Prefix to apply to all output images"));
    checkOpenOutput->SetToolTip(wxT("Create a new data source and open it when complete"));
    checkOpenOutput->SetValue(1);
    btnRun->SetToolTip(wxT("Run this task"));
    btnRun->SetDefault();
    btnHide->SetToolTip(wxT("Close this dialog"));
    // end wxGlade
}

void ApplyTransformDialog::do_layout()
{
    // begin wxGlade: ApplyTransformDialog::do_layout
    wxBoxSizer* sizer_28 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_29 = new wxBoxSizer(wxHORIZONTAL);
    wxStaticBoxSizer* sizer_31 = new wxStaticBoxSizer(sizer_31_staticbox, wxHORIZONTAL);
    wxFlexGridSizer* grid_sizer_10 = new wxFlexGridSizer(3, 3, 5, 0);
    wxStaticBoxSizer* sizer_30 = new wxStaticBoxSizer(sizer_30_staticbox, wxHORIZONTAL);
    wxFlexGridSizer* grid_sizer_9 = new wxFlexGridSizer(1, 3, 5, 0);
    grid_sizer_9->Add(label_25, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_9->Add(textTransform, 0, wxEXPAND|wxADJUST_MINSIZE, 0);
    grid_sizer_9->Add(btnBrowse, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_9->AddGrowableCol(1);
    sizer_30->Add(grid_sizer_9, 1, wxEXPAND, 0);
    sizer_28->Add(sizer_30, 0, wxEXPAND, 0);
    grid_sizer_10->Add(label_26, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_10->Add(textDirectory, 0, wxEXPAND|wxADJUST_MINSIZE, 0);
    grid_sizer_10->Add(btnBrowseDirectory, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_10->Add(label_27, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_10->Add(textPrefix, 0, wxEXPAND|wxADJUST_MINSIZE, 0);
    grid_sizer_10->Add(20, 20, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_10->Add(20, 20, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_10->Add(checkOpenOutput, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_10->Add(20, 20, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_10->AddGrowableCol(1);
    sizer_31->Add(grid_sizer_10, 1, wxEXPAND, 0);
    sizer_28->Add(sizer_31, 1, wxEXPAND, 0);
    sizer_29->Add(btnRun, 0, wxADJUST_MINSIZE, 0);
    sizer_29->Add(btnHide, 0, wxADJUST_MINSIZE, 0);
    sizer_28->Add(sizer_29, 0, wxALIGN_CENTER_HORIZONTAL, 0);
    SetAutoLayout(true);
    SetSizer(sizer_28);
    Layout();
    // end wxGlade
}

