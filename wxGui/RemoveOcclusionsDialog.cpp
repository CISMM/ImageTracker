// -*- C++ -*- generated by wxGlade 0.4.1 on Tue Jan 30 21:01:15 2007

#include "RemoveOcclusionsDialog.h"

#include "FileSet.h"
#include "wxUtils.h"

/**
 * Called by wxWidgets when this dialog is displayed. Sets up the
 * displayed components to reflect the data managed by this dialog.
 */
bool RemoveOcclusionsDialog::TransferDataToWindow()
{
    if (this->input.IsNotNull())
    {
        switch (this->pipeline->GetMetric())
        {
        case Mean: 
            this->comboMetric->SetValue(wxT("Mean"));
            break;
        case Median:
            this->comboMetric->SetValue(wxT("Median"));
            break;
        }
        this->slidePercentile->SetValue(this->pipeline->GetTransmitPercentile());
        this->slidePadding->SetValue(this->pipeline->GetFourierPadding());
        
        this->textDirectory->SetValue(std2wx(this->input->GetFiles().GetDirectory()));
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * Called by wxWidgets when the OK button is pressed. Transfers the
 * data in this dialog to the appropriate application objects.
 */
bool RemoveOcclusionsDialog::TransferDataFromWindow()
{
    this->pipeline->SetInputFiles(this->input->GetFiles());
    
    FileSet outFiles(this->input->GetFiles(), wx2std(this->textPrefix->GetValue()));
    outFiles.SetDirectory(wx2std(this->textDirectory->GetValue()));
    
    this->pipeline->SetOutputFiles(outFiles);
    this->pipeline->SetTransmissionFile(wx2std(
        this->textDirectory->GetValue() + wxT("/") + this->textTransmission->GetValue()));
    
    std::string metric = wx2std(this->comboMetric->GetValue());
    if (metric == "Mean")
        this->pipeline->SetMetric(Mean);
    else
        this->pipeline->SetMetric(Median);
    this->pipeline->SetTransmitPercentile(this->slidePercentile->GetValue());
    this->pipeline->SetFourierPadding(this->slidePadding->GetValue());
    
    this->pipeline->Update();
    
    if (this->checkOpenOutput->IsChecked() && this->controller.IsNotNull())
    {
        DataSource::Pointer data = DataSource::New();
        data->SetName("rpo " + this->input->GetName());
        data->SetFiles(this->pipeline->GetOutputFiles());
        this->controller->AddDataSource(data);
    }
    
    // Return false if we want to keep the window up.
    return true;
}

RemoveOcclusionsDialog::RemoveOcclusionsDialog(wxWindow* parent, int id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
    wxDialog(parent, id, title, pos, size, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxTHICK_FRAME)
{
    // begin wxGlade: RemoveOcclusionsDialog::RemoveOcclusionsDialog
    sizer_17_staticbox = new wxStaticBox(this, -1, wxT("Output"));
    sizer_18_staticbox = new wxStaticBox(this, -1, wxT("Parameters"));
    label_1 = new wxStaticText(this, -1, wxT("Metric"));
    const wxString comboMetric_choices[] = {
        wxT("Median"),
        wxT("Mean")
    };
    comboMetric = new wxComboBox(this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 2, comboMetric_choices, wxCB_DROPDOWN|wxCB_READONLY);
    label_3 = new wxStaticText(this, -1, wxT("Max Transmission"));
    slidePercentile = new wxSlider(this, -1, 100, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL|wxSL_LABELS);
    label_4 = new wxStaticText(this, -1, wxT("Fourier Padding"));
    slidePadding = new wxDoubleSlider(this, -1);
    label_5 = new wxStaticText(this, -1, wxT("Directory"));
    textDirectory = new wxTextCtrl(this, -1, wxT(""));
    btnBrowse = new wxButton(this, BTN_BROWSE, wxT("&Browse..."));
    label_6 = new wxStaticText(this, -1, wxT("Transmission Map"));
    textTransmission = new wxTextCtrl(this, -1, wxT("transmission.mha"));
    label_7 = new wxStaticText(this, -1, wxT("Output Prefix"));
    textPrefix = new wxTextCtrl(this, -1, wxT("rpo-"));
    checkOpenOutput = new wxCheckBox(this, -1, wxT("Open output when finished"));
    btnRun = new wxButton(this, wxID_OK, wxT("&Run"));
    btnHide = new wxButton(this, wxID_CANCEL, wxT("&Hide"));

    set_properties();
    do_layout();
    // end wxGlade
    
    // ----- Custom Code -----//
    this->pipeline = RemovePartialOcclusionsPipeline::New();
    // Set up the Fourier padding parameter
    this->slidePadding->SetRange(0.0, 1.0, 0.1);
    this->slidePadding->SetValue(0.5);
}


BEGIN_EVENT_TABLE(RemoveOcclusionsDialog, wxDialog)
    // begin wxGlade: RemoveOcclusionsDialog::event_table
    EVT_BUTTON(BTN_BROWSE, RemoveOcclusionsDialog::OnBrowse)
    EVT_BUTTON(wxID_OK, RemoveOcclusionsDialog::OnRun)
    EVT_BUTTON(wxID_CANCEL, RemoveOcclusionsDialog::OnHide)
    // end wxGlade
END_EVENT_TABLE();


void RemoveOcclusionsDialog::OnRun(wxCommandEvent &event)
{
    event.Skip();
}


void RemoveOcclusionsDialog::OnHide(wxCommandEvent &event)
{
    this->Show(false);
    event.Skip();
}

void RemoveOcclusionsDialog::OnBrowse(wxCommandEvent &event)
{
    wxDirDialog dir(this, wxT("Choose a directory"), this->textDirectory->GetValue());
    if (dir.ShowModal() == wxID_OK)
    {
        this->textDirectory->SetValue(dir.GetPath().Append(wxT("/")));
    }
}

// wxGlade: add RemoveOcclusionsDialog event handlers


void RemoveOcclusionsDialog::set_properties()
{
    // begin wxGlade: RemoveOcclusionsDialog::set_properties
    SetTitle(wxT("Remove Partial Occlusions"));
    comboMetric->SetToolTip(wxT("The metric to use to extimate the gradient of the fixed trasmission map"));
    comboMetric->SetSelection(0);
    label_3->SetMinSize(wxSize(120, 17));
    slidePercentile->SetToolTip(wxT("Percentile of the transmission map to set to 1 (total transmission)"));
    slidePadding->SetToolTip(wxT("Multiples of the image width by which to pad Fourier Transforms"));
    textDirectory->SetToolTip(wxT("The directory in which to store the output files"));
    label_6->SetMinSize(wxSize(120, 27));
    textTransmission->SetToolTip(wxT("A file to hold the computed transmission map (must support float pixel type)"));
    textPrefix->SetToolTip(wxT("A prefix to append to every output file"));
    checkOpenOutput->SetValue(1);
    btnRun->SetDefault();
    // end wxGlade
}


void RemoveOcclusionsDialog::do_layout()
{
    // begin wxGlade: RemoveOcclusionsDialog::do_layout
    wxBoxSizer* sizer_2 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_15 = new wxBoxSizer(wxHORIZONTAL);
    wxStaticBoxSizer* sizer_17 = new wxStaticBoxSizer(sizer_17_staticbox, wxHORIZONTAL);
    wxFlexGridSizer* grid_sizer_2 = new wxFlexGridSizer(4, 3, 5, 0);
    wxStaticBoxSizer* sizer_18 = new wxStaticBoxSizer(sizer_18_staticbox, wxHORIZONTAL);
    wxFlexGridSizer* grid_sizer_3 = new wxFlexGridSizer(3, 3, 5, 0);
    grid_sizer_3->Add(label_1, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_3->Add(comboMetric, 0, wxEXPAND|wxADJUST_MINSIZE, 0);
    grid_sizer_3->Add(85, 20, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_3->Add(label_3, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_3->Add(slidePercentile, 0, wxEXPAND|wxADJUST_MINSIZE, 0);
    grid_sizer_3->Add(20, 20, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_3->Add(label_4, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_3->Add(slidePadding, 1, wxEXPAND|wxADJUST_MINSIZE, 0);
    grid_sizer_3->Add(20, 20, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_3->AddGrowableCol(1);
    sizer_18->Add(grid_sizer_3, 1, wxEXPAND, 0);
    sizer_2->Add(sizer_18, 1, wxALL|wxEXPAND, 5);
    grid_sizer_2->Add(label_5, 0, wxEXPAND|wxADJUST_MINSIZE, 0);
    grid_sizer_2->Add(textDirectory, 0, wxEXPAND|wxADJUST_MINSIZE, 0);
    grid_sizer_2->Add(btnBrowse, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_2->Add(label_6, 0, wxEXPAND, 0);
    grid_sizer_2->Add(textTransmission, 0, wxEXPAND|wxADJUST_MINSIZE, 0);
    grid_sizer_2->Add(20, 20, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_2->Add(label_7, 0, wxEXPAND|wxADJUST_MINSIZE, 0);
    grid_sizer_2->Add(textPrefix, 0, wxEXPAND|wxADJUST_MINSIZE, 0);
    grid_sizer_2->Add(20, 20, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_2->Add(20, 20, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_2->Add(checkOpenOutput, 0, wxEXPAND|wxADJUST_MINSIZE, 0);
    grid_sizer_2->Add(20, 20, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_2->AddGrowableCol(1);
    sizer_17->Add(grid_sizer_2, 1, wxEXPAND, 0);
    sizer_2->Add(sizer_17, 1, wxALL|wxEXPAND, 5);
    sizer_15->Add(btnRun, 0, wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 0);
    sizer_15->Add(btnHide, 0, wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 0);
    sizer_2->Add(sizer_15, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 0);
    SetAutoLayout(true);
    SetSizer(sizer_2);
    sizer_2->Fit(this);
    sizer_2->SetSizeHints(this);
    Layout();
    // end wxGlade
}
