/////////////////////////////////////////////////////////////////////////////
// Name:        HarrisFeatureDialog.h
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

#include "FeatureTrackingPipeline.h"
#include "FileSet.h"
#include "VtkCanvas.h"
#include "wxUtils.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// HarrisFeatureDialog
//----------------------------------------------------------------------------

class HarrisFeatureDialog: public wxDialog
{
public:
    // constructors and destructors
    HarrisFeatureDialog( wxWindow *parent, 
        wxWindowID id=-1, 
        const wxString &title=_("Feature Tracker"),
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    virtual ~HarrisFeatureDialog();
    
    // WDR: method declarations for HarrisFeatureDialog
    wxButton* GetButtonDirectory()  { return (wxButton*) FindWindow( ID_BUTTON_DIRECTORY ); }
    wxCheckBox* GetCheckImages()  { return (wxCheckBox*) FindWindow( ID_CHECK_IMAGES ); }
    wxSlider* GetSliderCount()  { return (wxSlider*) FindWindow( ID_SLIDER_COUNT ); }
    wxTextCtrl* GetTextCount()  { return (wxTextCtrl*) FindWindow( ID_TEXT_COUNT ); }
    wxTextCtrl* GetTextPrefix()  { return (wxTextCtrl*) FindWindow( ID_TEXT_PREFIX ); }
    wxTextCtrl* GetTextImageDir()  { return (wxTextCtrl*) FindWindow( ID_TEXT_IMAGE_DIR ); }
    wxCheckBox* GetCheckPreview()  { return (wxCheckBox*) FindWindow( ID_CHECK_PREVIEW ); }
    wxSlider* GetSliderDistance()  { return (wxSlider*) FindWindow( ID_SLIDER_DISTANCE ); }
    wxTextCtrl* GetTextDistance()  { return (wxTextCtrl*) FindWindow( ID_TEXT_DISTANCE ); }
    DoubleSlider* GetSliderScale()  { return (DoubleSlider*) FindWindow( ID_SLIDER_SCALE ); }
    wxTextCtrl* GetTextScale()  { return (wxTextCtrl*) FindWindow( ID_TEXT_SCALE ); }

    void SetCanvas(VtkCanvas* canvas) { this->canvas = canvas; }
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
    // WDR: member variable declarations for HarrisFeatureDialog
    
private:
    // WDR: handler declarations for HarrisFeatureDialog
    void OnPreview( wxCommandEvent &event );
    void OnHide( wxCommandEvent &event );
    void OnRun( wxCommandEvent &event );
    void OnDirectory( wxCommandEvent &event );
    void OnDistanceSlider( wxCommandEvent &event );
    void OnCountSlider( wxCommandEvent &event );
    void OnScaleSlider( wxCommandEvent &event );

    FeatureTrackingPipeline* GetPipeline()
    {
        if (!this->trackingPipe)
            this->trackingPipe = new FeatureTrackingPipeline();
        return this->trackingPipe;
    }

    void InitializeDialog();
    void UpdateCanvas();
    void UpdateDialog();
    void UpdatePipeline();

private:
    wxString outFilePrefix;
    wxString outFileDir;
    FeatureTrackingPipeline* trackingPipe;
    VtkCanvas* canvas;

    DECLARE_EVENT_TABLE()
};
