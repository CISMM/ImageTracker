// -*- C++ -*- generated by wxGlade 0.6.3 on Fri Aug  1 14:33:30 2008

#include "IntegrateFlowDialog.h"

// begin wxGlade: ::extracode

// end wxGlade

#include <string>

#include "FileUtils.h"
#include "ImageTrackerController.h"
#include "PipelineExecutor.h"
#include "wxUtils.h"

IntegrateFlowDialog::IntegrateFlowDialog(wxWindow* parent, int id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
    wxDialog(parent, id, title, pos, size, wxDEFAULT_DIALOG_STYLE)
{
    // begin wxGlade: IntegrateFlowDialog::IntegrateFlowDialog
    sizer_53_staticbox = new wxStaticBox(this, -1, wxT("Output"));
    sizer_52_staticbox = new wxStaticBox(this, -1, wxT("Parameters"));
    label_39 = new wxStaticText(this, wxID_ANY, wxT("Step size"));
    slideStepSize = new wxDoubleSlider(this, wxID_ANY);
    panelFilePattern = new FilePatternPanel(this, wxID_ANY);
    checkOpenOutput = new wxCheckBox(this, wxID_ANY, wxT("Open output when finished"));
    buttonRun = new wxButton(this, wxID_OK, wxT("&Run"));
    buttonHide = new wxButton(this, wxID_CANCEL, wxT("&Hide"));

    set_properties();
    do_layout();
    // end wxGlade
    
    //-- Custom code --//
    // initialize pipeline & input
    this->input = NULL;
    this->pipeline = IntegrateFlowFieldPipeline::New();
    
    // initialize controls
    this->slideStepSize->SetRange(0.1, 1.0, 0.1);
    this->slideStepSize->SetValue(0.5);
    this->panelFilePattern->SetRangeEnabled(false);
    this->panelFilePattern->SetExtensionEnabled(false);
}


BEGIN_EVENT_TABLE(IntegrateFlowDialog, wxDialog)
    // begin wxGlade: IntegrateFlowDialog::event_table
    EVT_BUTTON(wxID_OK, IntegrateFlowDialog::OnRun)
    // end wxGlade
END_EVENT_TABLE();


void IntegrateFlowDialog::OnRun(wxCommandEvent &event)
{
    event.Skip();
}


// wxGlade: add IntegrateFlowDialog event handlers


void IntegrateFlowDialog::set_properties()
{
    // begin wxGlade: IntegrateFlowDialog::set_properties
    SetTitle(wxT("Integrate Flow"));
    SetSize(wxSize(550, 400));
    slideStepSize->SetToolTip(wxT("Size of an integration step"));
    checkOpenOutput->SetToolTip(wxT("Create a new data source and open it when finished"));
    checkOpenOutput->SetValue(1);
    buttonRun->SetDefault();
    // end wxGlade
}


void IntegrateFlowDialog::do_layout()
{
    // begin wxGlade: IntegrateFlowDialog::do_layout
    wxBoxSizer* sizer_51 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_54 = new wxBoxSizer(wxHORIZONTAL);
    wxStaticBoxSizer* sizer_53 = new wxStaticBoxSizer(sizer_53_staticbox, wxVERTICAL);
    wxFlexGridSizer* grid_sizer_19 = new wxFlexGridSizer(1, 2, 5, 5);
    wxStaticBoxSizer* sizer_52 = new wxStaticBoxSizer(sizer_52_staticbox, wxHORIZONTAL);
    sizer_52->Add(label_39, 0, 0, 0);
    sizer_52->Add(slideStepSize, 1, wxEXPAND, 0);
    sizer_51->Add(sizer_52, 0, wxEXPAND, 0);
    sizer_53->Add(panelFilePattern, 0, wxEXPAND, 0);
    grid_sizer_19->Add(110, 20, 0, 0, 0);
    grid_sizer_19->Add(checkOpenOutput, 0, 0, 0);
    grid_sizer_19->AddGrowableCol(1);
    sizer_53->Add(grid_sizer_19, 1, wxEXPAND, 0);
    sizer_51->Add(sizer_53, 1, wxEXPAND, 0);
    sizer_54->Add(buttonRun, 0, 0, 0);
    sizer_54->Add(buttonHide, 0, 0, 0);
    sizer_51->Add(sizer_54, 0, wxALIGN_CENTER_HORIZONTAL, 0);
    SetSizer(sizer_51);
    Layout();
    // end wxGlade
}


void IntegrateFlowDialog::SetInput(VectorFileSet* input)
{
    this->input = input;
}

bool IntegrateFlowDialog::TransferDataToWindow()
{
    if (this->pipeline && 
        this->pipeline.IsNotNull() && 
        this->input)
    {
        this->slideStepSize->SetValue(this->pipeline->GetStepSize());
        
        // create a default output file pattern
        std::string dir(nano::wx2std(wxGetCwd()));
        CapDirectory(dir);
        std::string format("disp-%04d.mha");
        std::string example = ImageTrackerController::Instance()->GetImageFiles()[0];    
        unsigned int start = NumberPart(example);
        unsigned int end = start + this->input->GetImageCount() - 1;
        this->panelFilePattern->SetFilePattern(FilePattern(dir, format, start, end));
        this->panelFilePattern->TransferDataToWindow();
        
        return true;
    }
    else
    {
        return false;
    }
}

bool IntegrateFlowDialog::TransferDataFromWindow()
{
    std::string function("IntegrateFlowDialog::TransferDataFromWindow");
    this->pipeline->SetInput(this->input);
    this->pipeline->SetStepSize(this->slideStepSize->GetValue());
    
    // Create an output file set
    FileSet outFiles(this->panelFilePattern->GetFilePattern());
    this->pipeline->SetOutputFiles(outFiles);
    
    // Create and launch a pipeline executor (uses another thread)
    PipelineExecutor* exec = new PipelineExecutor(this->pipeline);
    exec->SetOpenFiles(this->checkOpenOutput->IsChecked());
    exec->SetImageResult(false);    // our output is a set of vectors
    if (exec->Create() == wxTHREAD_NO_ERROR)
    {
        exec->Run();
    }
    else
    {
        Logger::warning << function << ": Unable able to create threaded pipeline execution object" << std::endl;
        delete exec;
    }
    
    return true;
}
