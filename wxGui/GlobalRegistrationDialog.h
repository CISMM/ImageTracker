#pragma once
#include <wx/wxprec.h>
#include "DoubleSlider.h"
#include "FileSetDialog.h"
#include "ImageCanvas.h"
#include "RegistrationDesign_wdr.h"

#include "../GlobalRegistrationPipeline.h"

/*
 * A dialog control that allows for configuration of registration.
 * Included is optional thresholding and smoothing of input images.
 * This dialog controls a GlobalRegistrationPipeline object.
 */
class GlobalRegistrationDialog : public wxDialog
{
public:
    // constructors and destructors
    GlobalRegistrationDialog(wxWindow *parent, wxWindowID id=-1, 
        const wxString &title=_T("Global Registration"), 
        const wxPoint &position=wxDefaultPosition, 
        const wxSize &size=wxDefaultSize, 
        long style=wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    ~GlobalRegistrationDialog(void);

    // WDR: method declarations for GlobalRegistrationDialog
    wxTextCtrl* GetTextOutdir()  { return (wxTextCtrl*) FindWindow( ID_TEXT_OUTDIR ); }
    wxTextCtrl* GetTextPrefix()  { return (wxTextCtrl*) FindWindow( ID_TEXT_PREFIX ); }
    wxTextCtrl* GetTextTransform()  { return (wxTextCtrl*) FindWindow( ID_TEXT_TRANSFORM ); }
    wxCheckBox* GetCheckImages()  { return (wxCheckBox*) FindWindow( ID_CHECK_IMAGES ); }
    wxCheckBox* GetCheckTransform()  { return (wxCheckBox*) FindWindow( ID_CHECK_TRANSFORM ); }
    DoubleSlider* GetSliderSmooth()  { return (DoubleSlider*) FindWindow( ID_SLIDER_SMOOTH ); }
    wxCheckBox* GetCheckPreview()  { return (wxCheckBox*) FindWindow( ID_CHECK_PREVIEW ); }
    wxTextCtrl* GetTextSmooth()  { return (wxTextCtrl*) FindWindow( ID_TEXT_SMOOTH ); }
    wxCheckBox* GetCheckSmooth()  { return (wxCheckBox*) FindWindow( ID_CHECK_SMOOTH ); }
    wxSlider* GetSliderBelow()  { return (wxSlider*) FindWindow( ID_SLIDER_BELOW ); }
    wxTextCtrl* GetTextBelow()  { return (wxTextCtrl*) FindWindow( ID_TEXT_BELOW ); }
    wxCheckBox* GetCheckBelow()  { return (wxCheckBox*) FindWindow( ID_CHECK_BELOW ); }
    wxSlider* GetSliderAbove()  { return (wxSlider*) FindWindow( ID_SLIDER_ABOVE ); }
    wxTextCtrl* GetTextAbove()  { return (wxTextCtrl*) FindWindow( ID_TEXT_ABOVE ); }
    wxCheckBox* GetCheckAbove()  { return (wxCheckBox*) FindWindow( ID_CHECK_ABOVE ); }

    /*
     * Returns a handle to the GlobalRegistrationPipeline that is associated with 
     * this Dialog.
     */
    GlobalRegistrationPipeline* GetPipeline();

    /*
     * Sets an ImageCanvas to be associated with this dialog.  The ImageCanvas will be
     * updated in preview mode.
     */
    void SetCanvas(ImageCanvas *cavas);

    /*
     * Get a value for the upper limit of the threshold controlled by this Dialog.  Returns
     * NULL if the upper limit is not enabled.
     */
    GlobalRegistrationPipeline::ImageType::PixelType* GetAbove();

    /*
     * Get a value for the lower limit of the threshold controlled by this Dialog. Returns
     * NULL if the lower limit is not enabled.
     */
    GlobalRegistrationPipeline::ImageType::PixelType* GetBelow();
    double GetSigma();

private:
    // WDR: member variable declarations for GlobalRegistrationDialog
    
private:
    // WDR: handler declarations for GlobalRegistrationDialog
    void OnOutDirectory( wxCommandEvent &event );
    void OnTransformFile( wxCommandEvent &event );
    void OnCheckImages( wxCommandEvent &event );
    void OnCheckTransforms( wxCommandEvent &event );
    void OnNext( wxCommandEvent &event );
    void OnSliderSmooth( wxCommandEvent &event );
    void OnSliderBelow( wxCommandEvent &event );
    void OnSliderAbove( wxCommandEvent &event );
    void OnHide( wxCommandEvent &event );
    void OnRun( wxCommandEvent &event );
    void OnCheckPreview( wxCommandEvent &event );
    void OnCheckSmooth( wxCommandEvent &event );
    void OnCheckBelow( wxCommandEvent &event );
    void OnCheckAbove( wxCommandEvent &event );

    /*
     * Initialize the dialog controls for the first time the dialog is displayed.
     * Sets initial slider ranges and values, for example.
     */
    void InitializeDialog();

    /*
     * Updates the dialog controls based on current control state.  For example,
     * disables or enables text boxes, updates value fields, etc.
     */
    void UpdateDialog();

    /*
     * Update the ImageCanvas that has been associated with this Dialog.  This
     * will show the output of the current image from the pipeline associated
     * with this dialog.
     */
    void UpdateCanvas();

    /*
     * Update the registration pipeline variables based on the state of the
     * dialog controls.
     */
    void UpdatePipelineValues();

    /*
     * Update the upper and lower threshold filter ranges.
     */
    void UpdateThresholdRange();

    /*
     * Instruct the registration pipeline to save its output given parameters
     * controlled by the dialog.
     */
    void SaveOutput();

    wxString transformFile;
    wxString outFileDir;
    wxString outFilePrefix;
    ImageCanvas *canvas;
    GlobalRegistrationPipeline *regPipe;
    

DECLARE_EVENT_TABLE();
};
