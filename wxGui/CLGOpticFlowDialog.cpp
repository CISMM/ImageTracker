// -*- C++ -*- generated by wxGlade 0.4.1 on Tue Feb  6 01:59:37 2007

#include "CLGOpticFlowDialog.h"
#include "FileSet.h"
#include "Logger.h"
#include "MathUtils.h"
#include "PipelineExecutor.h"
#include "wxUtils.h"

bool CLGOpticFlowDialog::TransferDataToWindow()
{
    this->slideSmoothing->SetValue(this->pipeline->GetSpatialSigma());
    this->slideRegularization->SetValue(log10(this->pipeline->GetRegularization()));
    this->slideRelaxation->SetValue(this->pipeline->GetRelaxation());
    this->slideIterations->SetValue(this->pipeline->GetIterations());
    
    this->textDirectory->SetValue(std2wx(this->input->GetFiles().GetDirectory()));
    
    this->ViewPreview(true);
    
    return true;
}

bool CLGOpticFlowDialog::TransferDataFromWindow()
{
    std::string function("CLGOpticFlowDialog::TransferDataFromWindow");
    this->pipeline->SetSpatialSigma(this->slideSmoothing->GetValue());
    this->pipeline->SetRegularization(exp10(this->slideRegularization->GetValue()));
    this->pipeline->SetRelaxation(this->slideRelaxation->GetValue());
    this->pipeline->SetIterations(this->slideIterations->GetValue());
    
    FileSet outputFiles(this->input->GetFiles(), wx2std(this->textPrefix->GetValue()), "mha");
    outputFiles.SetDirectory(wx2std(this->textDirectory->GetValue()));
    this->pipeline->SetOutputFiles(outputFiles);
    
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
    
    this->ViewPreview(false);
    this->Show(false);
    return true;
}

void CLGOpticFlowDialog::SetController(ImageTrackerController::Pointer controller)
{
    this->controller = controller;
}

void CLGOpticFlowDialog::SetInput(DataSource::Pointer input)
{
    this->input = input;
    this->pipeline->SetInput(this->input->GetImages());
}

void CLGOpticFlowDialog::ViewPreview(bool show)
{
    std::string function("CLGOpticFlowDialog::ViewPreview");
    if (show &&
       this->controller.IsNotNull() &&
       this->input.IsNotNull())
    {
        Logger::debug << function << ": Setting up previewing." << std::endl;
        this->visual->SetInput(this->pipeline->GetPreviewImage());
        this->controller->GetRenderer()->AddActor(this->visual->GetOutput());
        this->preview = true;
        this->UpdatePreview();
    }
    else
    {
        this->controller->GetRenderer()->RemoveActor(this->visual->GetOutput());
        this->UpdatePreview();
        this->preview = false;
        
    }
}

void CLGOpticFlowDialog::UpdatePreview()
{
    if (this->preview)
    {
        this->visual->Update();
        this->controller->GetRenderWindow()->Render();
    }
}
        
CLGOpticFlowDialog::CLGOpticFlowDialog(wxWindow* parent, int id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
    wxDialog(parent, id, title, pos, size, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxTHICK_FRAME)
{
    // begin wxGlade: CLGOpticFlowDialog::CLGOpticFlowDialog
    sizer_26_staticbox = new wxStaticBox(this, -1, wxT("Output"));
    sizer_27_staticbox = new wxStaticBox(this, -1, wxT("Parameters"));
    label_15 = new wxStaticText(this, -1, wxT("Spatial Smoothing"));
    slideSmoothing = new wxDoubleSlider(this, SLD_SMOOTHING);
    label_16 = new wxStaticText(this, -1, wxT("Regularization (10^x)"));
    slideRegularization = new wxDoubleSlider(this, -1);
    label_19 = new wxStaticText(this, -1, wxT("Relaxation"));
    slideRelaxation = new wxDoubleSlider(this, -1);
    label_20 = new wxStaticText(this, -1, wxT("Iterations"));
    slideIterations = new wxIntSlider(this, -1);
    label_13 = new wxStaticText(this, -1, wxT("Directory"));
    textDirectory = new wxTextCtrl(this, -1, wxT(""));
    btnBrowse = new wxButton(this, BTN_BROWSE, wxT("&Browse..."));
    label_14 = new wxStaticText(this, -1, wxT("Prefix"));
    textPrefix = new wxTextCtrl(this, -1, wxT("flow-"));
    checkOpenOutput = new wxCheckBox(this, -1, wxT("Open output when done"));
    btnRun = new wxButton(this, wxID_OK, wxT("&Run"));
    btnHide = new wxButton(this, wxID_CANCEL, wxT("&Hide"));

    set_properties();
    do_layout();
    // end wxGlade
    
    //--- Custom Code ---//
    // Set up control components
    this->pipeline = CLGOpticFlowPipeline::New();
    this->visual = ScalarImageItkVtkPipeline::New();
    this->preview = false;
    
    // Set up sliders
    this->slideSmoothing->SetRange(0.5, 25.0, 0.1);
    this->slideSmoothing->SetFormat("%0.02f");
    this->slideRegularization->SetRange(1.0, 20.0, 0.5);
    this->slideRegularization->SetFormat("%0.02f");
    this->slideRelaxation->SetRange(1, 2, 0.01);
    this->slideRelaxation->SetFormat("%0.02f");
    this->slideIterations->SetRange(200, 4000);
}


BEGIN_EVENT_TABLE(CLGOpticFlowDialog, wxDialog)
    EVT_SLIDER(SLD_SMOOTHING, CLGOpticFlowDialog::OnSlideSmoothing)
    // begin wxGlade: CLGOpticFlowDialog::event_table
    EVT_BUTTON(BTN_BROWSE, CLGOpticFlowDialog::OnBrowse)
    EVT_BUTTON(wxID_OK, CLGOpticFlowDialog::OnRun)
    EVT_BUTTON(wxID_CANCEL, CLGOpticFlowDialog::OnHide)
    // end wxGlade
END_EVENT_TABLE();

void CLGOpticFlowDialog::OnSlideSmoothing(wxCommandEvent &event)
{
    this->pipeline->SetSpatialSigma(this->slideSmoothing->GetValue());
    this->UpdatePreview();
}

void CLGOpticFlowDialog::OnBrowse(wxCommandEvent &event)
{
    wxDirDialog dlg(this, wxT("Choose a directory"), this->textDirectory->GetValue());
    if (dlg.ShowModal() == wxID_OK)
    {
		this->textDirectory->SetValue(dlg.GetPath().Append(std2wx(FileSet::PATH_DELIMITER)));
    }
}


void CLGOpticFlowDialog::OnRun(wxCommandEvent &event)
{
    event.Skip();
}


void CLGOpticFlowDialog::OnHide(wxCommandEvent &event)
{
    this->ViewPreview(false);
    event.Skip();
}


// wxGlade: add CLGOpticFlowDialog event handlers


void CLGOpticFlowDialog::set_properties()
{
    // begin wxGlade: CLGOpticFlowDialog::set_properties
    SetTitle(wxT("CLG Optic Flow"));
    SetSize(wxSize(550, 400));
    slideSmoothing->SetToolTip(wxT("Smoothing to apply to each input image"));
    label_16->SetMinSize(wxSize(145, 17));
    slideRegularization->SetToolTip(wxT("Weighting to apply to the global component of flow computation (higher value creates smoother flow)"));
    slideRelaxation->SetToolTip(wxT("Optimization speed parameter--1.9 is a good value"));
    slideIterations->SetToolTip(wxT("Number of flow refinement iterations for each image pair"));
    textDirectory->SetToolTip(wxT("Directory in which to store computed flow fields"));
    btnBrowse->SetToolTip(wxT("Find an output directory"));
    textPrefix->SetToolTip(wxT("Prefix to apply to each output flow field"));
    checkOpenOutput->SetToolTip(wxT("Create a new data source and open it when completed (not yet available)"));
    checkOpenOutput->Enable(false);
    btnRun->SetToolTip(wxT("Run this task"));
    btnRun->SetDefault();
    btnHide->SetToolTip(wxT("Close this dialog"));
    // end wxGlade
}


void CLGOpticFlowDialog::do_layout()
{
    // begin wxGlade: CLGOpticFlowDialog::do_layout
    wxBoxSizer* sizer_24 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_25 = new wxBoxSizer(wxHORIZONTAL);
    wxStaticBoxSizer* sizer_26 = new wxStaticBoxSizer(sizer_26_staticbox, wxHORIZONTAL);
    wxFlexGridSizer* grid_sizer_7 = new wxFlexGridSizer(3, 3, 5, 0);
    wxStaticBoxSizer* sizer_27 = new wxStaticBoxSizer(sizer_27_staticbox, wxHORIZONTAL);
    wxFlexGridSizer* grid_sizer_8 = new wxFlexGridSizer(4, 2, 5, 0);
    grid_sizer_8->Add(label_15, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_8->Add(slideSmoothing, 1, wxEXPAND, 0);
    grid_sizer_8->Add(label_16, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_8->Add(slideRegularization, 1, wxEXPAND, 0);
    grid_sizer_8->Add(label_19, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_8->Add(slideRelaxation, 1, wxEXPAND, 0);
    grid_sizer_8->Add(label_20, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_8->Add(slideIterations, 1, wxEXPAND, 0);
    grid_sizer_8->AddGrowableCol(1);
    sizer_27->Add(grid_sizer_8, 1, wxEXPAND, 0);
    sizer_24->Add(sizer_27, 1, wxEXPAND, 0);
    grid_sizer_7->Add(label_13, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_7->Add(textDirectory, 0, wxEXPAND|wxADJUST_MINSIZE, 0);
    grid_sizer_7->Add(btnBrowse, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_7->Add(label_14, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_7->Add(textPrefix, 0, wxEXPAND|wxADJUST_MINSIZE, 0);
    grid_sizer_7->Add(20, 20, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_7->Add(20, 20, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_7->Add(checkOpenOutput, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_7->Add(20, 20, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_7->AddGrowableCol(1);
    sizer_26->Add(grid_sizer_7, 1, wxEXPAND, 0);
    sizer_24->Add(sizer_26, 0, wxEXPAND, 0);
    sizer_25->Add(btnRun, 0, wxADJUST_MINSIZE, 0);
    sizer_25->Add(btnHide, 0, wxADJUST_MINSIZE, 0);
    sizer_24->Add(sizer_25, 0, wxALIGN_CENTER_HORIZONTAL, 0);
    SetAutoLayout(true);
    SetSizer(sizer_24);
    Layout();
    // end wxGlade
}

