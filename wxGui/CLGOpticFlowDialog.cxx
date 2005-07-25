/////////////////////////////////////////////////////////////////////////////
// Name:        CLGOpticFlowDialog.cxx
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#include "CLGOpticFlowDialog.h"
#include <math.h>
#include "../Logger.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// CLGOpticFlowDialog
//----------------------------------------------------------------------------

// WDR: event table for CLGOpticFlowDialog

BEGIN_EVENT_TABLE(CLGOpticFlowDialog,wxDialog)
    EVT_SLIDER( ID_SLIDER_SIGMA, CLGOpticFlowDialog::OnSigmaSlider )
    EVT_SLIDER( ID_SLIDER_REGULARIZATION, CLGOpticFlowDialog::OnRegularizationSlider )
    EVT_SLIDER( ID_SLIDER_RELAXATION, CLGOpticFlowDialog::OnRelaxationSlider )
    EVT_SLIDER( ID_SLIDER_ITERATIONS, CLGOpticFlowDialog::OnIterationSlider )
    EVT_BUTTON( ID_BUTTON_DIRECTORY, CLGOpticFlowDialog::OnDirectory )
    EVT_CHECKBOX( ID_CHECK_IMAGES, CLGOpticFlowDialog::OnImagesChecked )
    EVT_BUTTON( ID_BUTTON_RUN, CLGOpticFlowDialog::OnRunAll )
    EVT_BUTTON( ID_BUTTON_HIDE, CLGOpticFlowDialog::OnHide )
END_EVENT_TABLE()

CLGOpticFlowDialog::CLGOpticFlowDialog( wxWindow *parent, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style ) :
    wxDialog( parent, id, title, position, size, style )
{
    this->flowPipeline = NULL;
    this->outFileDir = "";
    this->outFilePrefix = "Flow-";

    // WDR: dialog function CreateCLGOpticFlowDialog for CLGOpticFlowDialog
    CreateCLGOpticFlowDialog( this, TRUE );

    this->InitializeDialog();
    this->UpdateDialog();
}

CLGOpticFlowDialog::~CLGOpticFlowDialog()
{
}

void CLGOpticFlowDialog::InitializeDialog()
{
    this->GetSliderSigma()->SetRange(0.10, 20, 0.05);
    this->GetSliderRegularization()->SetRange(1, 20, 0.5);
    this->GetSliderRelaxation()->SetRange(1, 2, 0.01);
}

void CLGOpticFlowDialog::UpdateDialog()
{
    this->GetTextImageDir()->Enable(this->GetCheckImages()->IsChecked());
    this->GetButtonDirectory()->Enable(this->GetCheckImages()->IsChecked());
    this->GetTextPrefix()->Enable(this->GetCheckImages()->IsChecked());

    this->GetTextPrefix()->SetValue(this->outFilePrefix);
    this->GetTextImageDir()->SetValue(this->outFileDir);

    wxString text;
    text = "";
    text.Printf("%5.2f", this->GetPipeline()->GetSpatialSigma());
    this->GetTextSigma()->SetValue(text);
    this->GetSliderSigma()->SetValue(this->GetPipeline()->GetSpatialSigma());

    text = "";
    double logReg = log10(this->GetPipeline()->GetRegularization());
    text.Printf("%5.2f", logReg);
    this->GetTextRegularization()->SetValue(text);
    this->GetSliderRegularization()->SetValue(logReg);

    text = "";
    text.Printf("%5.2f", this->GetPipeline()->GetRelaxation());
    this->GetTextRelaxation()->SetValue(text);
    this->GetSliderRelaxation()->SetValue(this->GetPipeline()->GetRelaxation());

    text = "";
    text.Printf("%i", this->GetPipeline()->GetIterations());
    this->GetTextIterations()->SetValue(text);
    this->GetSliderIterations()->SetValue(this->GetPipeline()->GetIterations());
}

void CLGOpticFlowDialog::UpdatePipeline()
{
    this->GetPipeline()->SetSpatialSigma(this->GetSliderSigma()->GetDoubleValue());
    double reg = pow(10.0, this->GetSliderRegularization()->GetDoubleValue());
    this->GetPipeline()->SetRegularization(reg);
    this->GetPipeline()->SetRelaxation(this->GetSliderRelaxation()->GetDoubleValue());
    this->GetPipeline()->SetIterations(this->GetSliderIterations()->GetValue());
    this->GetPipeline()->SetDestination(
        new FileSet(this->GetPipeline()->GetSource(),
        this->GetTextPrefix()->GetValue().c_str(), "mha"));
}

// WDR: handler implementations for CLGOpticFlowDialog

void CLGOpticFlowDialog::OnHide( wxCommandEvent &event )
{
    this->Show(false);
}

void CLGOpticFlowDialog::OnRunAll( wxCommandEvent &event )
{
    this->UpdatePipeline();
    this->GetPipeline()->UpdateAll();
}

void CLGOpticFlowDialog::OnImagesChecked( wxCommandEvent &event )
{
    
}

void CLGOpticFlowDialog::OnDirectory( wxCommandEvent &event )
{
    if (this->GetCheckImages()->IsChecked())
    {
        wxDirDialog dir(this, _("Choose a directory"), this->outFileDir, wxDD_NEW_DIR_BUTTON);
        if (dir.ShowModal() == wxID_OK)
        {
            this->outFileDir = wxString(dir.GetPath());
            this->outFileDir.append("\\");
            this->GetTextImageDir()->SetValue(this->outFileDir);
        }
    }
}

void CLGOpticFlowDialog::OnIterationSlider( wxCommandEvent &event )
{
    this->GetPipeline()->SetIterations(this->GetSliderIterations()->GetValue());
    wxString text = "";
    text.Printf("%i", this->GetPipeline()->GetIterations());
    this->GetTextIterations()->SetValue(text);
}

void CLGOpticFlowDialog::OnRelaxationSlider( wxCommandEvent &event )
{
    this->GetPipeline()->SetRelaxation(this->GetSliderRelaxation()->GetDoubleValue());
    wxString text = "";
    text.Printf("%5.2f", this->GetPipeline()->GetRelaxation());
    this->GetTextRelaxation()->SetValue(text);    
}

void CLGOpticFlowDialog::OnRegularizationSlider( wxCommandEvent &event )
{
    wxString text = "";
    double logReg = this->GetSliderRegularization()->GetDoubleValue();
    double reg = pow(10.0, logReg);
    text.Printf("%5.2f", logReg);
    this->GetTextRegularization()->SetValue(text);
    this->GetSliderRegularization()->SetValue(reg);
}

void CLGOpticFlowDialog::OnSigmaSlider( wxCommandEvent &event )
{
    this->GetPipeline()->SetSpatialSigma(this->GetSliderSigma()->GetDoubleValue());
    wxString text = "";
    text.Printf("%5.2f", this->GetPipeline()->GetSpatialSigma());
    this->GetTextSigma()->SetValue(text);
}




