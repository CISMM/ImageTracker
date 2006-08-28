/////////////////////////////////////////////////////////////////////////////
// Name:        MultiResolutionRegistrationDialog.h
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#ifndef __MultiResolutionRegistrationDialog_H__
#define __MultiResolutionRegistrationDialog_H__

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "RegistrationDesign_wdr.h"
#include "DoubleSlider.h"
#include "FileSet.h"
#include "Logger.h"
#include "MultiResolutionRegistrationPipeline.h"
#include "VtkCanvas.h"
#include "wxDoubleSlider.h"
#include "wxIntSlider.h"

// WDR: class declarations

//----------------------------------------------------------------------------
// MultiResolutionRegistrationDialog
//----------------------------------------------------------------------------

class MultiResolutionRegistrationDialog: public wxDialog
{
public:
    // constructors and destructors
    MultiResolutionRegistrationDialog( wxWindow *parent, 
        wxWindowID id = -1, const wxString &title = _("Multi Resolution Registration"),
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER );
    virtual ~MultiResolutionRegistrationDialog();
    
    // WDR: method declarations for MultiResolutionRegistrationDialog
    wxDoubleSlider* GetSliderStepLengthScale()  { return (wxDoubleSlider*) FindWindow( MR_SLIDER_SCALE ); }
    wxDoubleSlider* GetSliderMinimumStep()  { return (wxDoubleSlider*) FindWindow( MR_SLIDER_MINSTEP ); }
    wxDoubleSlider* GetSliderMaximumStep()  { return (wxDoubleSlider*) FindWindow( MR_SLIDER_MAXSTEP ); }
    wxIntSlider* GetSliderIterations()  { return (wxIntSlider*) FindWindow( MR_SLIDER_ITERATIONS ); }
    wxTextCtrl* GetTextPrefix()  { return (wxTextCtrl*) FindWindow( MR_TEXT_PREFIX ); }
    wxTextCtrl* GetTextOutDirectory()  { return (wxTextCtrl*) FindWindow( MR_TEXT_OUTDIR ); }
    wxTextCtrl* GetTextTransform()  { return (wxTextCtrl*) FindWindow( MR_TEXT_TRANSFORM ); }
    wxIntSlider* GetSliderNumberOfLevels()  { return (wxIntSlider*) FindWindow( MR_SLIDER_LEVELS ); }
    wxIntSlider* GetSliderShrinkFactor()  { return (wxIntSlider*) FindWindow( MR_SLIDER_SHRINK ); }
    wxIntSlider* GetSliderBelow()  { return (wxIntSlider*) FindWindow( MR_SLIDER_BELOW ); }
    wxIntSlider* GetSliderAbove()  { return (wxIntSlider*) FindWindow( MR_SLIDER_ABOVE ); }
    wxCheckBox* GetCheckBelow()  { return (wxCheckBox*) FindWindow( MR_CHECK_BELOW ); }
    wxCheckBox* GetCheckAbove()  { return (wxCheckBox*) FindWindow( MR_CHECK_ABOVE ); }
    
    void SetCanvas(VtkCanvas* canvas) { this->canvas = canvas; }
    void SetInputFiles(const FileSet& inputFiles);

protected:
    void UpdateCanvas();
	void Initialize();

private:
    // WDR: member variable declarations for MultiResolutionRegistrationDialog
    MultiResolutionRegistrationPipeline::Pointer pipeline;
    VtkCanvas* canvas;

private:
    // WDR: handler declarations for MultiResolutionRegistrationDialog
    void OnSliderStepLengthScale( wxCommandEvent &event );
    void OnSliderMinStep( wxCommandEvent &event );
    void OnSliderMaxStep( wxCommandEvent &event );
    void OnSliderIterations( wxCommandEvent &event );
    void OnButtonHide( wxCommandEvent &event );
    void OnButtonRun( wxCommandEvent &event );
    void OnButtonOutDirectory( wxCommandEvent &event );
    void OnButtonTransformFile( wxCommandEvent &event );
    void OnSliderNumberOfLevels( wxCommandEvent &event );
    void OnSliderShrinkFactor( wxCommandEvent &event );
    void OnSliderBelow( wxCommandEvent &event );
    void OnSliderAbove( wxCommandEvent &event );
    void OnCheckBelow( wxCommandEvent &event );
    void OnCheckAbove( wxCommandEvent &event );

private:
    DECLARE_EVENT_TABLE()
};

#endif
