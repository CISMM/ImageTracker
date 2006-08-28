/////////////////////////////////////////////////////////////////////////////
// Name:        MultiResolutionRegistrationDialog.cxx
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "MultiResolutionRegistrationDialog.cxx"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wxUtils.h"
#include "MultiResolutionRegistrationDialog.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// MultiResolutionRegistrationDialog
//----------------------------------------------------------------------------

// WDR: event table for MultiResolutionRegistrationDialog

BEGIN_EVENT_TABLE(MultiResolutionRegistrationDialog,wxDialog)
    EVT_CHECKBOX( MR_CHECK_ABOVE, MultiResolutionRegistrationDialog::OnCheckAbove )
    EVT_CHECKBOX( MR_CHECK_BELOW, MultiResolutionRegistrationDialog::OnCheckBelow )
    EVT_SLIDER( MR_SLIDER_ABOVE, MultiResolutionRegistrationDialog::OnSliderAbove )
    EVT_SLIDER( MR_SLIDER_BELOW, MultiResolutionRegistrationDialog::OnSliderBelow )
    EVT_SLIDER( MR_SLIDER_SHRINK, MultiResolutionRegistrationDialog::OnSliderShrinkFactor )
    EVT_SLIDER( MR_SLIDER_LEVELS, MultiResolutionRegistrationDialog::OnSliderNumberOfLevels )
    EVT_BUTTON( MR_TRANSFORM_FILE, MultiResolutionRegistrationDialog::OnButtonTransformFile )
    EVT_BUTTON( MR_OUT_DIRECTORY, MultiResolutionRegistrationDialog::OnButtonOutDirectory )
    EVT_BUTTON( MR_BUTTON_RUN, MultiResolutionRegistrationDialog::OnButtonRun )
    EVT_BUTTON( MR_BUTTON_HIDE, MultiResolutionRegistrationDialog::OnButtonHide )
    EVT_SLIDER( MR_SLIDER_ITERATIONS, MultiResolutionRegistrationDialog::OnSliderIterations )
    EVT_SLIDER( MR_SLIDER_MAXSTEP, MultiResolutionRegistrationDialog::OnSliderMaxStep )
    EVT_SLIDER( MR_SLIDER_MINSTEP, MultiResolutionRegistrationDialog::OnSliderMinStep )
    EVT_SLIDER( MR_SLIDER_SCALE, MultiResolutionRegistrationDialog::OnSliderStepLengthScale )
END_EVENT_TABLE()

MultiResolutionRegistrationDialog::MultiResolutionRegistrationDialog( wxWindow *parent, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style ) :
    wxDialog( parent, id, title, position, size, style )
{
    // WDR: dialog function CreateMultiResolutionRegistrationDialog for MultiResolutionRegistrationDialog
    CreateMultiResolutionRegistrationDialog( this, TRUE );

	this->canvas = 0;
    this->pipeline = MultiResolutionRegistrationPipeline::New();
	
	// Setup double sliders
    this->GetSliderMaximumStep()->SetRange(0.1, 25.0, 0.1);
    this->GetSliderMaximumStep()->SetValue(5.0);
    this->GetSliderMaximumStep()->SetFormat("%.1f");
	// this->pipeline->SetOptimizerInitialMaximumStepLength(5.0);
    this->GetSliderMinimumStep()->SetRange(0.05, 25.0, 0.05);
    this->GetSliderMinimumStep()->SetValue(0.5);
    this->GetSliderMinimumStep()->SetFormat("%.2f");
	// this->pipeline->SetOptimizerInitialMinimumStepLength(0.5);
    this->GetSliderStepLengthScale()->SetRange(1.0, 20.0, 0.5);
    this->GetSliderStepLengthScale()->SetValue(10.0);
    this->GetSliderStepLengthScale()->SetFormat("%.1f");

	// Sync sliders with pipeline
	this->GetSliderAbove()->SetRange(0, 255);
	this->GetSliderAbove()->SetValue(255);
	this->GetSliderBelow()->SetRange(0, 255);
	this->GetSliderBelow()->SetValue(0);
	this->GetSliderNumberOfLevels()->SetRange(1, 16);
	this->GetSliderNumberOfLevels()->SetValue(3);
	this->GetSliderShrinkFactor()->SetRange(1, 24);
	this->GetSliderShrinkFactor()->SetValue(4);
	this->GetSliderIterations()->SetRange(1, 1000);
	this->GetSliderIterations()->SetValue(200);
}

MultiResolutionRegistrationDialog::~MultiResolutionRegistrationDialog()
{
}

void MultiResolutionRegistrationDialog::UpdateCanvas()
{
    if (this->canvas)
    {
        this->canvas->SetItkImage(this->pipeline->GetPreviewImage());
    }
}

void MultiResolutionRegistrationDialog::SetInputFiles(const FileSet& inputFiles)
{ 
    this->pipeline->SetInputFiles(inputFiles);
    if (this->GetTextOutDirectory()->GetValue().IsSameAs(_("")))
        this->GetTextOutDirectory()->SetValue(std2wx(this->pipeline->GetInputFiles().GetDirectory()));
    if (this->GetTextTransform()->GetValue().IsSameAs(_("")))
    {
        wxString file(std2wx(this->pipeline->GetInputFiles().GetDirectory()));
        file.append(_("MRTransforms.txt")); // wx treats back slashes ok
        this->GetTextTransform()->SetValue(file);
    }
    if (this->GetTextPrefix()->GetValue().IsSameAs(_("")))
        this->GetTextPrefix()->SetValue(_("MRReg-"));

    this->UpdateCanvas();
}

// WDR: handler implementations for MultiResolutionRegistrationDialog

void MultiResolutionRegistrationDialog::OnSliderStepLengthScale( wxCommandEvent &event )
{
    Logger::debug << "OnSliderStepLengthScale(): " << this->GetSliderStepLengthScale()->GetValue() << std::endl;
	this->pipeline->SetOptimizerStepLengthScale(this->GetSliderStepLengthScale()->GetValue());
}

void MultiResolutionRegistrationDialog::OnSliderMinStep( wxCommandEvent &event )
{
    Logger::debug << "OnSliderMinStep(): " << this->GetSliderMinimumStep()->GetValue() << std::endl;
    this->pipeline->SetOptimizerInitialMinimumStepLength(this->GetSliderMinimumStep()->GetValue());
}

void MultiResolutionRegistrationDialog::OnSliderMaxStep( wxCommandEvent &event )
{
    Logger::debug << "OnSliderMaxStep(): " << this->GetSliderMaximumStep()->GetValue() << std::endl;
    this->pipeline->SetOptimizerInitialMaximumStepLength(this->GetSliderMaximumStep()->GetValue());
}

void MultiResolutionRegistrationDialog::OnSliderIterations( wxCommandEvent &event )
{
    this->pipeline->SetOptimizerNumberOfIterations(this->GetSliderIterations()->GetValue());
}

void MultiResolutionRegistrationDialog::OnButtonHide( wxCommandEvent &event )
{
    // Hide dialog
    this->Show(false);
}

void MultiResolutionRegistrationDialog::OnButtonRun( wxCommandEvent &event )
{    
    if (this->GetTextOutDirectory()->GetValue().IsSameAs(_("")) ||
        this->GetTextTransform()->GetValue().IsSameAs(_("")))
    {
        wxMessageDialog alert(this, _("You must specify an output directory and transform file."), _("Output unspecified"), wxOK);
        alert.ShowModal();
        return;
    }

    // Determine the output info
    this->pipeline->SetTransformFile(wx2std(this->GetTextTransform()->GetValue()));
    FileSet files(this->pipeline->GetInputFiles(), wx2std(this->GetTextPrefix()->GetValue()));
    files.SetDirectory(wx2std(this->GetTextOutDirectory()->GetValue()));
    this->pipeline->SetOutputFiles(files);

    // Run pipeline
    Logger::info << "Running MultiResolutionRegistration...." << std::endl;
    this->pipeline->Update();
    Logger::info << "Finished." << std::endl;
}

void MultiResolutionRegistrationDialog::OnButtonOutDirectory( wxCommandEvent &event )
{
    wxDirDialog dir(this, _("Choose a directory"), std2wx(this->pipeline->GetInputFiles().GetDirectory()), wxDD_NEW_DIR_BUTTON);
    if (dir.ShowModal() == wxID_OK)
    {
        wxString outDir(dir.GetPath());
        outDir.append(_("\\"));
        this->GetTextOutDirectory()->SetValue(outDir);
    }
}

void MultiResolutionRegistrationDialog::OnButtonTransformFile( wxCommandEvent &event )
{
    wxFileDialog save(this, _("Choose a file"), this->GetTextOutDirectory()->GetValue());
    if (save.ShowModal() == wxID_OK)
    {
        this->GetTextTransform()->SetValue(save.GetPath());

        if (this->GetTextOutDirectory()->GetValue().IsSameAs(_("")))
            this->GetTextOutDirectory()->SetValue(save.GetDirectory() + _("\\"));
    }
}

void MultiResolutionRegistrationDialog::OnSliderNumberOfLevels( wxCommandEvent &event )
{
    this->pipeline->SetNumberOfLevels(this->GetSliderNumberOfLevels()->GetValue());
}

void MultiResolutionRegistrationDialog::OnSliderShrinkFactor( wxCommandEvent &event )
{
    this->pipeline->SetStartingShrinkFactor(this->GetSliderShrinkFactor()->GetValue());
    this->UpdateCanvas();
}

void MultiResolutionRegistrationDialog::OnSliderBelow( wxCommandEvent &event )
{
    if (this->GetCheckBelow()->IsChecked())
        this->pipeline->SetLowerThreshold(this->GetSliderBelow()->GetValue());
    this->UpdateCanvas();
}

void MultiResolutionRegistrationDialog::OnSliderAbove( wxCommandEvent &event )
{
    if (this->GetCheckAbove()->IsChecked())
        this->pipeline->SetUpperThreshold(this->GetSliderAbove()->GetValue());
    this->UpdateCanvas();
}

void MultiResolutionRegistrationDialog::OnCheckBelow( wxCommandEvent &event )
{
    this->GetSliderBelow()->Enable(this->GetCheckBelow()->IsChecked());
}

void MultiResolutionRegistrationDialog::OnCheckAbove( wxCommandEvent &event )
{
    this->GetSliderAbove()->Enable(this->GetCheckAbove()->IsChecked());
}
