
#include "Logger.h"
#include "HarrisFeatureDialog.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// HarrisFeatureDialog
//----------------------------------------------------------------------------

// WDR: event table for HarrisFeatureDialog

BEGIN_EVENT_TABLE(HarrisFeatureDialog,wxDialog)
    EVT_SLIDER( ID_SLIDER_SCALE, HarrisFeatureDialog::OnScaleSlider )
    EVT_SLIDER( ID_SLIDER_DISTANCE, HarrisFeatureDialog::OnDistanceSlider )
    EVT_BUTTON( ID_BUTTON_DIRECTORY, HarrisFeatureDialog::OnDirectory )
    EVT_BUTTON( ID_BUTTON_RUN, HarrisFeatureDialog::OnRun )
    EVT_BUTTON( ID_BUTTON_HIDE, HarrisFeatureDialog::OnHide )
    EVT_SLIDER( ID_SLIDER_COUNT, HarrisFeatureDialog::OnCountSlider )
    EVT_CHECKBOX( ID_CHECK_PREVIEW, HarrisFeatureDialog::OnPreview )
END_EVENT_TABLE()

HarrisFeatureDialog::HarrisFeatureDialog( wxWindow *parent, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style ) :
    wxDialog( parent, id, title, position, size, style ),
    outFileDir(_("")),
    outFilePrefix(_("Track-"))
{

    this->canvas = NULL;
    this->trackingPipe = NULL;

    // WDR: dialog function CreateHLTrackerDialog for HarrisFeatureDialog
    CreateHLTrackerDialog( this, TRUE ); 

    this->InitializeDialog();
    this->UpdateDialog();
}

HarrisFeatureDialog::~HarrisFeatureDialog()
{
}

void HarrisFeatureDialog::InitializeDialog()
{
    this->GetSliderScale()->SetRange(0.10, 20, 0.05);
    this->GetSliderScale()->SetValue(1.0);
}

void HarrisFeatureDialog::UpdateCanvas()
{
    if (this->GetCheckPreview()->IsChecked() && this->canvas)
    {
        this->canvas->SetItkImage(this->GetPipeline()->GetFeatureImage());
    }
}

void HarrisFeatureDialog::UpdateDialog()
{
    this->GetTextImageDir()->Enable(this->GetCheckImages()->IsChecked());
    this->GetButtonDirectory()->Enable(this->GetCheckImages()->IsChecked());
    this->GetTextPrefix()->Enable(this->GetCheckImages()->IsChecked());

    this->GetTextPrefix()->SetValue(this->outFilePrefix);
    this->GetTextImageDir()->SetValue(this->outFileDir);

    wxString text(_(""));
    text.Printf(_("%i"), this->GetPipeline()->GetMaxFeatures());
    this->GetTextCount()->SetValue(text);
    this->GetSliderCount()->SetValue(this->GetPipeline()->GetMaxFeatures());
    
    text.Printf(_("%5.2f"), this->GetPipeline()->GetScale());
    this->GetTextScale()->SetValue(text);
    this->GetSliderScale()->SetValue(this->GetPipeline()->GetScale());

    text.Printf(_("%i"), this->GetPipeline()->GetMaxDistance());
    this->GetTextDistance()->SetValue(text);
    this->GetSliderDistance()->SetValue(this->GetPipeline()->GetMaxDistance());
}

void HarrisFeatureDialog::UpdatePipeline()
{
    this->GetPipeline()->SetMaxFeatures(this->GetSliderCount()->GetValue());
    this->GetPipeline()->SetScale(this->GetSliderScale()->GetDoubleValue());
    this->GetPipeline()->SetMaxDistance(this->GetSliderDistance()->GetValue());
    this->GetPipeline()->SetDestination(
        FileSet(this->GetPipeline()->GetSource(), 
        wx2std(this->GetTextPrefix()->GetValue()), "mha"));
}

// WDR: handler implementations for HarrisFeatureDialog

void HarrisFeatureDialog::OnPreview( wxCommandEvent &event )
{
    this->UpdateCanvas();    
}

void HarrisFeatureDialog::OnHide( wxCommandEvent &event )
{
    this->Show(false);
}

void HarrisFeatureDialog::OnRun( wxCommandEvent &event )
{
    this->UpdatePipeline();
    this->GetPipeline()->UpdateAll();
}

void HarrisFeatureDialog::OnDirectory( wxCommandEvent &event )
{
    if (this->GetCheckImages()->IsChecked())
    {
        wxDirDialog dir(this, _("Choose a directory"), this->outFileDir, wxDD_NEW_DIR_BUTTON);
        if (dir.ShowModal() == wxID_OK)
        {
            this->outFileDir = wxString(dir.GetPath());
            this->outFileDir.append(_("\\"));
            this->GetTextImageDir()->SetValue(this->outFileDir);
        }
    }
}

void HarrisFeatureDialog::OnDistanceSlider( wxCommandEvent &event )
{
    this->GetPipeline()->SetMaxDistance(this->GetSliderDistance()->GetValue());
    wxString text(_(""));
    text.Printf(_("%i"), this->GetPipeline()->GetMaxDistance());
    this->GetTextDistance()->SetValue(text);
}

void HarrisFeatureDialog::OnCountSlider( wxCommandEvent &event )
{
    this->GetPipeline()->SetMaxFeatures(this->GetSliderCount()->GetValue());
    wxString text(_(""));
    text.Printf(_("%i"), this->GetPipeline()->GetMaxFeatures());
    this->GetTextCount()->SetValue(text);    
}

void HarrisFeatureDialog::OnScaleSlider( wxCommandEvent &event )
{
    this->GetPipeline()->SetScale(this->GetSliderScale()->GetDoubleValue());
    wxString text(_(""));
    text.Printf(_("%5.2f"), this->GetPipeline()->GetScale());
    this->GetTextScale()->SetValue(text);
    this->UpdateCanvas();
}
