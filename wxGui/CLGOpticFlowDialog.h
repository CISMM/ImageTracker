/////////////////////////////////////////////////////////////////////////////
// Name:        CLGOpticFlowDialog.h
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#pragma once

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "DoubleSlider.h"
#include "RegistrationDesign_wdr.h"
#include "CLGOpticFlowPipeline.h"
#include "FileSet.h"
#include "wxUtils.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// CLGOpticFlowDialog
//----------------------------------------------------------------------------

class CLGOpticFlowDialog: public wxDialog
{
public:
    // constructors and destructors
    CLGOpticFlowDialog( wxWindow *parent, 
        wxWindowID id = -1, 
        const wxString &title = _("CLG Optic Flow"),
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    virtual ~CLGOpticFlowDialog();
    
    // WDR: method declarations for CLGOpticFlowDialog
    wxButton* GetButtonDirectory()  { return (wxButton*) FindWindow( ID_BUTTON_DIRECTORY ); }
    wxTextCtrl* GetTextPrefix()  { return (wxTextCtrl*) FindWindow( ID_TEXT_PREFIX ); }
    wxTextCtrl* GetTextImageDir()  { return (wxTextCtrl*) FindWindow( ID_TEXT_IMAGE_DIR ); }
    wxCheckBox* GetCheckImages()  { return (wxCheckBox*) FindWindow( ID_CHECK_IMAGES ); }
    wxSlider* GetSliderIterations()  { return (wxSlider*) FindWindow( ID_SLIDER_ITERATIONS ); }
    wxTextCtrl* GetTextIterations()  { return (wxTextCtrl*) FindWindow( ID_TEXT_ITERATIONS ); }
    DoubleSlider* GetSliderRelaxation()  { return (DoubleSlider*) FindWindow( ID_SLIDER_RELAXATION ); }
    wxTextCtrl* GetTextRelaxation()  { return (wxTextCtrl*) FindWindow( ID_TEXT_RELAXATION ); }
    DoubleSlider* GetSliderRegularization()  { return (DoubleSlider*) FindWindow( ID_SLIDER_REGULARIZATION ); }
    wxTextCtrl* GetTextRegularization()  { return (wxTextCtrl*) FindWindow( ID_TEXT_REGULARIZATION ); }
    DoubleSlider* GetSliderSigma()  { return (DoubleSlider*) FindWindow( ID_SLIDER_SIGMA ); }
    wxTextCtrl* GetTextSigma()  { return (wxTextCtrl*) FindWindow( ID_TEXT_SIGMA ); }

    void SetInput(const FileSet& files)
    {
        this->GetPipeline()->SetSource(files);
        if (this->GetPipeline()->GetDestination().size() == 0)
        {
            this->GetPipeline()->SetDestination(FileSet(files, wx2std(this->outFilePrefix), "mha"));
        }
        if (this->outFileDir.compare(_("")) == 0)
        {
            this->outFileDir = std2wx(this->GetPipeline()->GetSource().GetDirectory());
            this->GetTextImageDir()->SetValue(this->outFileDir);
        }
    }
    
private:
    // WDR: member variable declarations for CLGOpticFlowDialog
    
    CLGOpticFlowPipeline::Pointer flowPipeline;
    CLGOpticFlowPipeline::Pointer GetPipeline()
    {
        if (!this->flowPipeline)
            this->flowPipeline = CLGOpticFlowPipeline::New();
        return this->flowPipeline;
    }

    void InitializeDialog();
    void UpdateDialog();
    void UpdatePipeline();

    wxString outFilePrefix;
    wxString outFileDir;

private:
    // WDR: handler declarations for CLGOpticFlowDialog
    void OnHide( wxCommandEvent &event );
    void OnRunAll( wxCommandEvent &event );
    void OnImagesChecked( wxCommandEvent &event );
    void OnDirectory( wxCommandEvent &event );
    void OnIterationSlider( wxCommandEvent &event );
    void OnRelaxationSlider( wxCommandEvent &event );
    void OnRegularizationSlider( wxCommandEvent &event );
    void OnSigmaSlider( wxCommandEvent &event );

private:
    DECLARE_EVENT_TABLE()
};



