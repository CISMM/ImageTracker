/////////////////////////////////////////////////////////////////////////////
// Name:        HarrisFeatureDialog.h
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include <wx/wxprec.h>
#include "DoubleSlider.h"
#include "ImageCanvas.h"
#include "RegistrationDesign_wdr.h"

#include "../FeatureTrackingPipeline.h"
#include "../FileSet.h"

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

    void SetCanvas(ImageCanvas* canvas) { this->canvas = canvas; }
    void SetInput(FileSet* files) 
    { 
        this->GetPipeline()->SetSource(files);
        if (!this->GetPipeline()->GetDestination())
        {
            this->GetPipeline()->SetDestination(new FileSet(files, this->outFilePrefix.c_str(), "mha"));
        }
        if (this->outFileDir.compare("") == 0)
        {
            this->outFileDir = wxString(this->GetPipeline()->GetSource()->GetDirectory().c_str());
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
    ImageCanvas* canvas;

    DECLARE_EVENT_TABLE()
};
