// -*- C++ -*- generated by wxGlade 0.4.1 on Fri Feb  2 17:46:58 2007

#include "MultiResolutionRegistrationDialog.h"

#include "itkStatisticsImageFilter.h"
#include "vtkRenderWindow.h"

#include "FileSet.h"
#include "Logger.h"
#include "PipelineExecutor.h"
#include "wxUtils.h"
// #include "WxPipelineObserver.h"

bool MultiResolutionRegistrationDialog::TransferDataToWindow()
{
    // Only setup the sliders the first time the dialog is displayed
    // After that, leave them how the user left them
    if (firstTime)
    {
        firstTime = false;
        StatsType::Pointer stats = StatsType::New();
        stats->SetInput(dynamic_cast<ImageTypeF2*>(this->input->GetImage(0)));
        stats->Update();
        if (stats->GetMaximum() > 255)
        {
            this->comboRange->SetValue(std2wx("16-bit"));
            // setting the combo box programmatically does not trigger the event, 
            // so we do it here
            wxCommandEvent event;
            this->OnRange(event);
            this->slideUpperBound->SetValue(65535);
        }
        else
        {
            this->comboRange->SetValue(std2wx("8-bit"));
            wxCommandEvent event;
            this->OnRange(event);
            this->slideUpperBound->SetValue(255);
        }
    }
        
    this->slideIterations->SetValue(this->pipeline->GetOptimizerNumberOfIterations());
    this->slideMaxStepLength->SetValue(this->pipeline->GetOptimizerInitialMaximumStepLength());
    this->slideMinStepLength->SetValue(this->pipeline->GetOptimizerInitialMinimumStepLength());
    this->slideStepScale->SetValue(this->pipeline->GetOptimizerStepLengthScale());
    
    this->textDirectory->SetValue(std2wx(this->input->GetFiles().GetDirectory()));
    
    this->ViewPreview(true);
    
    return true;
}

bool MultiResolutionRegistrationDialog::TransferDataFromWindow()
{
    std::string function("MultiResolutionRegistrationDialog::TransferDataFromWindow");
    this->pipeline->SetUpperThreshold(this->slideUpperBound->GetValue());
    this->pipeline->SetLowerThreshold(this->slideLowerBound->GetValue());
    
    this->pipeline->SetShrinkFactors(this->slideMinSmooth->GetValue(), this->slideMaxSmooth->GetValue());
    
    this->pipeline->SetOptimizerNumberOfIterations(this->slideIterations->GetValue());
    this->pipeline->SetOptimizerInitialMaximumStepLength(this->slideMaxStepLength->GetValue());
    this->pipeline->SetOptimizerInitialMinimumStepLength(this->slideMinStepLength->GetValue());
    this->pipeline->SetOptimizerStepLengthScale(this->slideStepScale->GetValue());
    
    FileSet outputFiles(this->input->GetFiles(), wx2std(this->textPrefix->GetValue()));
    outputFiles.SetDirectory(wx2std(this->textDirectory->GetValue()));
    this->pipeline->SetOutputFiles(outputFiles);
    this->pipeline->SetTransformFile(
            wx2std(this->textDirectory->GetValue() + this->textTransform->GetValue()));
    
    Logger::verbose << function << ": Creating executor thread" << std::endl;
    PipelineExecutor* exec = new PipelineExecutor(this->pipeline);
    exec->SetOpenFiles(this->checkOpenOutput->IsChecked(), this->controller);
    if (exec->Create() == wxTHREAD_NO_ERROR)
    {
        Logger::verbose << function << ": Running exectutor thread" << std::endl;
        exec->Run();
        Logger::verbose << function << ": Return from PipelineExecutor::Run()" << std::endl;
    }
    else
    {
        Logger::warning << function << ": Thread was not created; pipeline not executed." << std::endl;
    }
    
    this->ViewPreview(false);
    this->Show(false);
    return true;
}

void MultiResolutionRegistrationDialog::SetController(ImageTrackerController::Pointer controller)
{
    this->controller = controller;
}

void MultiResolutionRegistrationDialog::SetInput(DataSource::Pointer input)
{
    this->input = input;
    this->pipeline->SetInput(this->input->GetImages());
}

void MultiResolutionRegistrationDialog::ViewPreview(bool show)
{
    std::string function("MultiResolutionRegistrationDialog::ViewPreview()");
    if (this->controller && 
        this->input.IsNotNull() &&
        this->input->size() > 0)
    {
        if (show)
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
}

void MultiResolutionRegistrationDialog::UpdatePreview()
{
    if (this->preview)
    {
        this->visual->Update();
        this->controller->GetRenderWindow()->Render();
    }
}

MultiResolutionRegistrationDialog::MultiResolutionRegistrationDialog(wxWindow* parent, int id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
    wxDialog(parent, id, title, pos, size, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxTHICK_FRAME)
{
    // begin wxGlade: MultiResolutionRegistrationDialog::MultiResolutionRegistrationDialog
    sizer_20_staticbox = new wxStaticBox(this, -1, wxT("Smoothing"));
    sizer_23_staticbox = new wxStaticBox(this, -1, wxT("Optimization"));
    sizer_21_staticbox = new wxStaticBox(this, -1, wxT("Output"));
    sizer_22_staticbox = new wxStaticBox(this, -1, wxT("Thresholding"));
    checkEnabled = new wxCheckBox(this, CBX_ENABLE, wxT("Enabled"));
    label_17 = new wxStaticText(this, -1, wxT("Upper Bound"));
    slideUpperBound = new wxIntSlider(this, SLD_UPPER_BOUND);
    label_18 = new wxStaticText(this, -1, wxT("Lower Bound"));
    slideLowerBound = new wxIntSlider(this, SLD_LOWER_BOUND);
    label_20_copy = new wxStaticText(this, -1, wxT("Range"));
    const wxString comboRange_choices[] = {
        wxT("8-bit"),
        wxT("16-bit"),
        wxT("From Image")
    };
    comboRange = new wxComboBox(this, CMB_RANGE, wxT(""), wxDefaultPosition, wxDefaultSize, 3, comboRange_choices, wxCB_DROPDOWN|wxCB_READONLY);
    label_8 = new wxStaticText(this, -1, wxT("Maximum Smoothing"));
    slideMaxSmooth = new wxIntSlider(this, SLD_MAX_SMOOTH);
    label_9 = new wxStaticText(this, -1, wxT("Minimum Smoothing"));
    slideMinSmooth = new wxIntSlider(this, SLD_MIN_SMOOTH);
    label_21 = new wxStaticText(this, -1, wxT("Iterations"));
    slideIterations = new wxIntSlider(this, -1);
    label_22 = new wxStaticText(this, -1, wxT("Initial Maximum\nStep Length"));
    slideMaxStepLength = new wxDoubleSlider(this, -1);
    label_23 = new wxStaticText(this, -1, wxT("Initial Minimum\nStep Length"));
    slideMinStepLength = new wxDoubleSlider(this, -1);
    label_24 = new wxStaticText(this, -1, wxT("Inter-level\nStep Scale"));
    slideStepScale = new wxDoubleSlider(this, -1);
    label_10 = new wxStaticText(this, -1, wxT("Directory"));
    textDirectory = new wxTextCtrl(this, -1, wxT(""));
    btnBrowseDir = new wxButton(this, BTN_BROWSE_DIR, wxT("Browse..."));
    label_11 = new wxStaticText(this, -1, wxT("Transform File"));
    textTransform = new wxTextCtrl(this, -1, wxT("transforms.txt"));
    btnBrowseTransform = new wxButton(this, BTN_BROWSE_TRANSFORM, wxT("Browse..."));
    label_12 = new wxStaticText(this, -1, wxT("Image Prefix"));
    textPrefix = new wxTextCtrl(this, -1, wxT("reg-"));
    checkOpenOutput = new wxCheckBox(this, -1, wxT("Open output when finished"));
    btnRun = new wxButton(this, wxID_OK, wxT("&Run"));
    btnHide = new wxButton(this, wxID_CANCEL, wxT("&Hide"));

    set_properties();
    do_layout();
    // end wxGlade
    
    //-- Custom Code --//
    // Setup process objects
    this->pipeline = MultiResolutionRegistrationPipeline::New();
    this->visual = ScalarImageItkVtkPipeline::New();
    this->firstTime = true;
    this->preview = false;
    
    // Setup sliders
    this->slideUpperBound->SetValue(255);
    this->slideLowerBound->SetValue(0);
    
    this->slideMinSmooth->SetRange(1,32);
    this->slideMinSmooth->SetValue(2);
    this->slideMaxSmooth->SetRange(1,32);
    this->slideMaxSmooth->SetValue(8);
    
    this->slideIterations->SetRange(50, 1000);
    this->slideIterations->SetValue(200);
    this->slideMaxStepLength->SetRange(0.1, 25.0, 0.1);
    this->slideMaxStepLength->SetValue(5.0);
    this->slideMaxStepLength->SetFormat("%0.02f");
    this->slideMinStepLength->SetRange(0.05, 25, 0.05);
    this->slideMinStepLength->SetValue(0.5);
    this->slideMinStepLength->SetFormat("%0.02f");
    this->slideStepScale->SetRange(1.0, 20.0, 0.5);
    this->slideStepScale->SetValue(10.0);
    this->slideStepScale->SetFormat("%0.1f");
}


BEGIN_EVENT_TABLE(MultiResolutionRegistrationDialog, wxDialog)
    // begin wxGlade: MultiResolutionRegistrationDialog::event_table
    EVT_CHECKBOX(CBX_ENABLE, MultiResolutionRegistrationDialog::OnThresholdEnabled)
    EVT_COMBOBOX(CMB_RANGE, MultiResolutionRegistrationDialog::OnRange)
    EVT_BUTTON(BTN_BROWSE_DIR, MultiResolutionRegistrationDialog::OnBrowseDir)
    EVT_BUTTON(BTN_BROWSE_TRANSFORM, MultiResolutionRegistrationDialog::OnBrowseTransform)
    EVT_BUTTON(wxID_OK, MultiResolutionRegistrationDialog::OnRun)
    EVT_BUTTON(wxID_CANCEL, MultiResolutionRegistrationDialog::OnHide)
    // end wxGlade
    EVT_SLIDER(SLD_UPPER_BOUND, MultiResolutionRegistrationDialog::OnUpperBound)
    EVT_SLIDER(SLD_LOWER_BOUND, MultiResolutionRegistrationDialog::OnLowerBound)
    EVT_SLIDER(SLD_MAX_SMOOTH, MultiResolutionRegistrationDialog::OnMaxSmooth)
    EVT_SLIDER(SLD_MIN_SMOOTH, MultiResolutionRegistrationDialog::OnMinSmooth)
END_EVENT_TABLE();


void MultiResolutionRegistrationDialog::OnThresholdEnabled(wxCommandEvent &event)
{
    event.Skip();
}

void MultiResolutionRegistrationDialog::OnRange(wxCommandEvent &event)
{
    typedef unsigned char EightBit;
    typedef unsigned short SixteenBit;
    
    std::string function("MultiResolutionRegistrationDialog::OnRange");
    std::string val(wx2std(this->comboRange->GetValue()));
    Logger::verbose << function << ": value = " << val << std::endl;
    if (val == "8-bit")
    {
        Logger::verbose << function << ": setting 8-bit range" << std::endl;
        this->slideUpperBound->SetRange(std::numeric_limits<EightBit>::min(), 
                                        std::numeric_limits<EightBit>::max());
        this->slideLowerBound->SetRange(std::numeric_limits<EightBit>::min(), 
                                        std::numeric_limits<EightBit>::max());
    }
    else if (val == "16-bit")
    {
        Logger::verbose << function << ": setting 16-bit range" << std::endl;
        this->slideUpperBound->SetRange(std::numeric_limits<SixteenBit>::min(), 
                                        std::numeric_limits<SixteenBit>::max());
        this->slideLowerBound->SetRange(std::numeric_limits<SixteenBit>::min(), 
                                        std::numeric_limits<SixteenBit>::max());
    }
    else // Use image min/max
    {
        Logger::verbose << function << ": setting range from image" << std::endl;
        StatsType::Pointer stats = StatsType::New();
        stats->SetInput(dynamic_cast< ImageTypeF2* >(this->input->GetImage(0)));
        stats->Update();
        this->slideUpperBound->SetRange(stats->GetMinimum(), stats->GetMaximum());
        this->slideLowerBound->SetRange(stats->GetMinimum(), stats->GetMaximum());
    }
}

void MultiResolutionRegistrationDialog::OnUpperBound(wxCommandEvent &event)
{
    this->pipeline->SetUpperThreshold(this->slideUpperBound->GetValue());
    this->UpdatePreview();
}

void MultiResolutionRegistrationDialog::OnLowerBound(wxCommandEvent &event)
{
    this->pipeline->SetLowerThreshold(this->slideLowerBound->GetValue());
    this->UpdatePreview();
}

void MultiResolutionRegistrationDialog::OnMaxSmooth(wxCommandEvent &event)
{
    this->pipeline->SetPreviewShrinkFactor((unsigned int) this->slideMaxSmooth->GetValue());
    this->UpdatePreview();
}

void MultiResolutionRegistrationDialog::OnMinSmooth(wxCommandEvent &event)
{
    this->pipeline->SetPreviewShrinkFactor((unsigned int) this->slideMinSmooth->GetValue());
    this->UpdatePreview();
}

void MultiResolutionRegistrationDialog::OnBrowseDir(wxCommandEvent &event)
{
    wxDirDialog dlg(this, wxT("Choose a directory"), this->textDirectory->GetValue());
    if (dlg.ShowModal() == wxID_OK)
    {
		this->textDirectory->SetValue(dlg.GetPath().Append(std2wx(FileSet::PATH_DELIMITER)));
    }
}


void MultiResolutionRegistrationDialog::OnBrowseTransform(wxCommandEvent &event)
{
    wxFileDialog dlg(this, wxT("Choose a transform file"));
    if (dlg.ShowModal() == wxID_OK)
    {
        this->textTransform->SetValue(dlg.GetFilename());
    }
}


void MultiResolutionRegistrationDialog::OnRun(wxCommandEvent &event)
{	
    event.Skip();
}


void MultiResolutionRegistrationDialog::OnHide(wxCommandEvent &event)
{
    this->ViewPreview(false);
    event.Skip();
}


// wxGlade: add MultiResolutionRegistrationDialog event handlers


void MultiResolutionRegistrationDialog::set_properties()
{
    // begin wxGlade: MultiResolutionRegistrationDialog::set_properties
    SetTitle(wxT("Multi-Resolution Registration"));
    SetSize(wxSize(550, 800));
    checkEnabled->Hide();
    checkEnabled->SetValue(1);
    slideUpperBound->SetToolTip(wxT("The upper threshold of image intensities to consider"));
    slideLowerBound->SetToolTip(wxT("The lower threshold of image intensities to consider"));
    comboRange->SetToolTip(wxT("Select the range of intensities for thresholding"));
    comboRange->SetSelection(0);
    label_8->SetMinSize(wxSize(140, 17));
    slideMaxSmooth->SetToolTip(wxT("The coarsest resolution at which to align images"));
    slideMinSmooth->SetToolTip(wxT("The finest resolution at which to align images"));
    slideIterations->SetToolTip(wxT("The number of optimization iterations to apply at each resolution"));
    slideMaxStepLength->SetToolTip(wxT("The maximum step size at the coarsest resolution level"));
    slideMinStepLength->SetToolTip(wxT("The minimum step size at the coarsest resolution"));
    slideStepScale->SetToolTip(wxT("Scale factor by which to reduce the max and min step size at each resolution level"));
    textDirectory->SetToolTip(wxT("Directory in which to save registered images"));
    btnBrowseDir->SetToolTip(wxT("Find an output directory"));
    textTransform->SetToolTip(wxT("File in which to save the transform parameters"));
    btnBrowseTransform->SetToolTip(wxT("Find a transform file"));
    textPrefix->SetToolTip(wxT("Prefix to append to each output image"));
    checkOpenOutput->SetToolTip(wxT("Create a new data source and open it when complete"));
    checkOpenOutput->SetValue(1);
    btnRun->SetToolTip(wxT("Run this task"));
    btnHide->SetToolTip(wxT("Close this dialog"));
    // end wxGlade
}


void MultiResolutionRegistrationDialog::do_layout()
{
    // begin wxGlade: MultiResolutionRegistrationDialog::do_layout
    wxBoxSizer* sizer_16 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_19 = new wxBoxSizer(wxHORIZONTAL);
    wxStaticBoxSizer* sizer_21 = new wxStaticBoxSizer(sizer_21_staticbox, wxHORIZONTAL);
    wxFlexGridSizer* grid_sizer_4 = new wxFlexGridSizer(4, 3, 5, 0);
    wxStaticBoxSizer* sizer_23 = new wxStaticBoxSizer(sizer_23_staticbox, wxHORIZONTAL);
    wxFlexGridSizer* grid_sizer_6 = new wxFlexGridSizer(4, 2, 5, 0);
    wxStaticBoxSizer* sizer_20 = new wxStaticBoxSizer(sizer_20_staticbox, wxHORIZONTAL);
    wxFlexGridSizer* grid_sizer_1 = new wxFlexGridSizer(2, 2, 5, 0);
    wxStaticBoxSizer* sizer_22 = new wxStaticBoxSizer(sizer_22_staticbox, wxHORIZONTAL);
    wxFlexGridSizer* grid_sizer_5 = new wxFlexGridSizer(4, 2, 5, 0);
    grid_sizer_5->Add(20, 20, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_5->Add(checkEnabled, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_5->Add(label_17, 0, wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 0);
    grid_sizer_5->Add(slideUpperBound, 1, wxEXPAND, 0);
    grid_sizer_5->Add(label_18, 0, wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 0);
    grid_sizer_5->Add(slideLowerBound, 1, wxEXPAND, 0);
    grid_sizer_5->Add(label_20_copy, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_5->Add(comboRange, 0, wxEXPAND|wxADJUST_MINSIZE, 0);
    grid_sizer_5->AddGrowableCol(1);
    sizer_22->Add(grid_sizer_5, 1, wxEXPAND, 0);
    sizer_16->Add(sizer_22, 0, wxALL|wxEXPAND, 2);
    grid_sizer_1->Add(label_8, 0, wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 0);
    grid_sizer_1->Add(slideMaxSmooth, 1, wxEXPAND, 0);
    grid_sizer_1->Add(label_9, 0, wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 0);
    grid_sizer_1->Add(slideMinSmooth, 1, wxEXPAND, 0);
    grid_sizer_1->AddGrowableCol(1);
    sizer_20->Add(grid_sizer_1, 1, wxEXPAND, 0);
    sizer_16->Add(sizer_20, 0, wxALL|wxEXPAND, 2);
    grid_sizer_6->Add(label_21, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_6->Add(slideIterations, 1, wxEXPAND, 0);
    grid_sizer_6->Add(label_22, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_6->Add(slideMaxStepLength, 1, wxEXPAND, 0);
    grid_sizer_6->Add(label_23, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_6->Add(slideMinStepLength, 1, wxEXPAND, 0);
    grid_sizer_6->Add(label_24, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_6->Add(slideStepScale, 1, wxEXPAND, 0);
    grid_sizer_6->AddGrowableCol(1);
    sizer_23->Add(grid_sizer_6, 1, wxEXPAND, 0);
    sizer_16->Add(sizer_23, 0, wxALL|wxEXPAND, 2);
    grid_sizer_4->Add(label_10, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_4->Add(textDirectory, 0, wxEXPAND|wxADJUST_MINSIZE, 0);
    grid_sizer_4->Add(btnBrowseDir, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_4->Add(label_11, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_4->Add(textTransform, 0, wxEXPAND|wxADJUST_MINSIZE, 0);
    grid_sizer_4->Add(btnBrowseTransform, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_4->Add(label_12, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_4->Add(textPrefix, 0, wxEXPAND|wxADJUST_MINSIZE, 0);
    grid_sizer_4->Add(20, 20, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_4->Add(20, 20, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_4->Add(checkOpenOutput, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_4->Add(20, 20, 0, wxADJUST_MINSIZE, 0);
    grid_sizer_4->AddGrowableCol(1);
    sizer_21->Add(grid_sizer_4, 1, wxEXPAND, 0);
    sizer_16->Add(sizer_21, 0, wxALL|wxEXPAND, 2);
    sizer_19->Add(btnRun, 0, wxADJUST_MINSIZE, 0);
    sizer_19->Add(btnHide, 0, wxADJUST_MINSIZE, 0);
    sizer_16->Add(sizer_19, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxSHAPED, 0);
    SetAutoLayout(true);
    SetSizer(sizer_16);
    Layout();
    // end wxGlade
}

