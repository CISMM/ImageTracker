// -*- C++ -*- generated by wxGlade 0.4.1 on Fri Jun 29 18:44:39 2007

#include "HornOpticalFlowDialog.h"

#include "FileUtils.h"
#include "Logger.h"
#include "PipelineExecutor.h"
#include "wxUtils.h"

bool HornOpticalFlowDialog::TransferDataToWindow()
{
    this->sldSmoothing->SetValue(this->pipeline->GetSpatialSigma());
    this->sldRegularization->SetValue(this->pipeline->GetSmoothWeighting());
    this->sldIterations->SetValue(this->pipeline->GetIterations());
    
    // create an output file pattern from the input files
    std::string dir(this->input->GetFiles().GetDirectory());
    std::string format("horn-%04d.mha");
    unsigned int start = NumberPart(this->input->GetFiles()[0]);
    unsigned int end = start + this->input->size() - 2;
    this->panelFilePattern->SetFilePattern(FilePattern(dir, format, start, end));
    this->panelFilePattern->TransferDataToWindow();
    
    return true;
}

bool HornOpticalFlowDialog::TransferDataFromWindow()
{
    std::string function("HornOpticalFlowDialog::TransferDataFromWindow");
    this->pipeline->SetSpatialSigma(this->sldSmoothing->GetValue());
    this->pipeline->SetSmoothWeighting(this->sldRegularization->GetValue());
    this->pipeline->SetIterations(this->sldIterations->GetValue());
    
    FileSet outFiles(this->panelFilePattern->GetFilePattern());
    this->pipeline->SetOutputFiles(outFiles);
    
    // Create and launch a pipeline executor (uses another thread)
    PipelineExecutor* exec = new PipelineExecutor(this->pipeline);
    // TODO: When we support flow field visualization, uncomment next line.
    // exec->SetOpenFiles(this->checkOpenOutput->IsChecked(), this->controller);
    if (exec->Create() == wxTHREAD_NO_ERROR)
    {
        exec->Run();
    }
    else
    {
        Logger::warning << function << ": Unable to create threaded pipeline execution object." << std::endl;
        delete exec;
    }
    
    this->Show(false);
    return true;
}

void HornOpticalFlowDialog::SetInput(DataSource::Pointer input)
{
    this->input = input;
    this->pipeline->SetInput(this->input->GetImages());
}

HornOpticalFlowDialog::HornOpticalFlowDialog(wxWindow* parent, int id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
    wxDialog(parent, id, title, pos, size, style)
{
    // begin wxGlade: HornOpticalFlowDialog::HornOpticalFlowDialog
    sizer_36_staticbox = new wxStaticBox(this, -1, wxT("Output"));
    sizer_35_staticbox = new wxStaticBox(this, -1, wxT("Parameters"));
    label_28 = new wxStaticText(this, -1, wxT("Spatial Smoothing"));
    sldSmoothing = new wxDoubleSlider(this, -1);
    label_29 = new wxStaticText(this, -1, wxT("Flow Smoothness"));
    sldRegularization = new wxDoubleSlider(this, -1);
    label_30 = new wxStaticText(this, -1, wxT("Iterations"));
    sldIterations = new wxIntSlider(this, -1);
    panelFilePattern = new FilePatternPanel(this, -1);
    checkOpenOutput = new wxCheckBox(this, -1, wxT("Open output when done"));
    btnRun = new wxButton(this, wxID_OK, wxT("&Run"));
    btnHide = new wxButton(this, wxID_CANCEL, wxT("&Hide"));

    set_properties();
    do_layout();
    // end wxGlade
    
    //--- Custom Code ---//
    // Set up control components
    this->pipeline = HornOpticalFlowPipeline::New();
    
    // Set up sliders
    this->sldSmoothing->SetRange(0.5, 24.0, 0.1);
    this->sldSmoothing->SetFormat("%0.02f");
    this->sldRegularization->SetRange(1.0, 4000.0, 1.0);
    this->sldRegularization->SetFormat("%0.02f");
    this->sldIterations->SetRange(200, 4000);
    
    panelFilePattern->SetRangeEnabled(false);
    panelFilePattern->SetExtensionEnabled(false);
}


BEGIN_EVENT_TABLE(HornOpticalFlowDialog, wxDialog)
    // begin wxGlade: HornOpticalFlowDialog::event_table
    EVT_BUTTON(wxID_OK, HornOpticalFlowDialog::OnRun)
    EVT_BUTTON(wxID_CANCEL, HornOpticalFlowDialog::OnHide)
    // end wxGlade
END_EVENT_TABLE();

void HornOpticalFlowDialog::OnRun(wxCommandEvent &event)
{
    event.Skip();
}

void HornOpticalFlowDialog::OnHide(wxCommandEvent &event)
{
    event.Skip();
}

// wxGlade: add HornOpticalFlowDialog event handlers


void HornOpticalFlowDialog::set_properties()
{
    // begin wxGlade: HornOpticalFlowDialog::set_properties
    SetTitle(wxT("Horn & Schunck Optical Flow"));
    SetSize(wxSize(550, 550));
    sldSmoothing->SetToolTip(wxT("Amount to smooth each input image"));
    sldRegularization->SetToolTip(wxT("A higher value enforces a smoother optical flow field"));
    sldIterations->SetToolTip(wxT("How many iterations to perform on each image pair; more iterations is slower but may be more accurate"));
    checkOpenOutput->SetToolTip(wxT("Indicates whether ImageTracker should open the output files when finished"));
    checkOpenOutput->Enable(false);
    btnRun->SetToolTip(wxT("Execute the optical flow algorithm"));
    btnHide->SetToolTip(wxT("Hide this dialog without executing"));
    // end wxGlade
}


void HornOpticalFlowDialog::do_layout()
{
    // begin wxGlade: HornOpticalFlowDialog::do_layout
    wxBoxSizer* sizer_34 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_37 = new wxBoxSizer(wxHORIZONTAL);
    wxStaticBoxSizer* sizer_36 = new wxStaticBoxSizer(sizer_36_staticbox, wxVERTICAL);
    wxFlexGridSizer* grid_sizer_12 = new wxFlexGridSizer(1, 2, 5, 5);
    wxStaticBoxSizer* sizer_35 = new wxStaticBoxSizer(sizer_35_staticbox, wxVERTICAL);
    wxFlexGridSizer* grid_sizer_11 = new wxFlexGridSizer(3, 2, 5, 5);
    grid_sizer_11->Add(label_28, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_11->Add(sldSmoothing, 1, wxEXPAND, 0);
    grid_sizer_11->Add(label_29, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_11->Add(sldRegularization, 1, wxEXPAND, 0);
    grid_sizer_11->Add(label_30, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_11->Add(sldIterations, 1, wxEXPAND, 0);
    grid_sizer_11->AddGrowableCol(1);
    sizer_35->Add(grid_sizer_11, 0, wxEXPAND, 0);
    sizer_34->Add(sizer_35, 0, wxEXPAND, 0);
    sizer_36->Add(panelFilePattern, 0, wxEXPAND, 0);
    grid_sizer_12->Add(110, 20, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_12->Add(checkOpenOutput, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_12->AddGrowableCol(1);
    sizer_36->Add(grid_sizer_12, 1, wxEXPAND, 0);
    sizer_34->Add(sizer_36, 1, wxEXPAND, 0);
    sizer_37->Add(btnRun, 0, wxADJUST_MINSIZE, 0);
    sizer_37->Add(btnHide, 0, wxADJUST_MINSIZE, 0);
    sizer_34->Add(sizer_37, 0, wxALIGN_CENTER_HORIZONTAL, 0);
    SetAutoLayout(true);
    SetSizer(sizer_34);
    Layout();
    // end wxGlade
}

