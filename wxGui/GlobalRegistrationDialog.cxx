#include "./GlobalRegistrationDialog.h"

#include <limits>
#include "../CommonTypes.h"
#include "../Logger.h"
#include "../TransformGroup.h"
#include "itkExceptionObject.h"

GlobalRegistrationDialog::GlobalRegistrationDialog(wxWindow *parent, wxWindowID id,
                                                   const wxString &title,
                                                   const wxPoint &position,
                                                   const wxSize &size,
                                                   long style) 
                                                   : wxDialog(parent, id, title, position, size, style)
{
    this->canvas = NULL;
    this->regPipe = NULL;
    this->transformFile = "";
    this->outFileDir = "";
    this->outFilePrefix = "Reg-";

    // WDR: dialog function CreateRegistrationDialog for GlobalRegistrationDialog
    CreateRegistrationDialog(this, TRUE);

    // Set default values
    this->InitializeDialog();
    this->UpdateDialog();
}

GlobalRegistrationDialog::~GlobalRegistrationDialog(void)
{
}

void GlobalRegistrationDialog::InitializeDialog()
{
    // TODO: how can we get min and max values for pixels?
    // int low = (int) std::numeric_limits<CommonTypes::InputPixelType>::min();
    // int high = (int) std::numeric_limits<CommonTypes::InputPixelType>::max();
    GlobalRegistrationPipeline::ImageType::PixelType min=0, max=255;
    this->GetSliderAbove()->SetRange(min, max);
    this->GetSliderBelow()->SetRange(min, max);
    this->GetSliderSmooth()->SetRange(0, 20, 0.05);

    this->GetSliderAbove()->SetValue(0);
    this->GetSliderBelow()->SetValue(0);
    this->GetSliderSmooth()->SetValue(1.0);
}

void GlobalRegistrationDialog::UpdateDialog()
{
    this->GetSliderAbove()->Enable(this->GetCheckAbove()->IsChecked());
    this->GetSliderBelow()->Enable(this->GetCheckBelow()->IsChecked());
    this->GetSliderSmooth()->Enable(this->GetCheckSmooth()->IsChecked());

    this->UpdateThresholdRange();

    wxString text;
    if (this->GetCheckAbove()->IsChecked())
    {
        text = "";
        text.Printf("%i", this->GetSliderAbove()->GetValue());
        this->GetTextAbove()->SetValue(text);
    }
    else
    {
        this->GetTextAbove()->SetValue("");
    }

    if (this->GetCheckBelow()->IsChecked())
    {
        text = "";
        text.Printf("%i", this->GetSliderBelow()->GetValue());
        this->GetTextBelow()->SetValue(text);
    }
    else
    {
        this->GetTextBelow()->SetValue("");
    }

    text = "";
    text.Printf("%3.2f", this->GetSliderSmooth()->GetDoubleValue());
    this->GetTextSmooth()->SetValue(text);

    this->GetTextTransform()->Enable(this->GetCheckTransform()->IsChecked());
    this->GetTextTransform()->SetValue(this->transformFile);

    this->GetTextOutdir()->Enable(this->GetCheckImages()->IsChecked());
    this->GetTextOutdir()->SetValue(this->outFileDir);
    this->GetTextPrefix()->Enable(this->GetCheckImages()->IsChecked());
    this->GetTextPrefix()->SetValue(this->outFilePrefix);
}

void GlobalRegistrationDialog::SetCanvas(VtkCanvas *canvas)
{
    this->canvas = canvas;
}

GlobalRegistrationPipeline::ImageType::PixelType* GlobalRegistrationDialog::GetAbove()
{
    GlobalRegistrationPipeline::ImageType::PixelType* above = NULL;

    if (this->GetCheckAbove()->IsChecked())
    {
        above = new GlobalRegistrationPipeline::ImageType::PixelType();
        int val = this->GetSliderAbove()->GetValue();
        *above = (GlobalRegistrationPipeline::ImageType::PixelType) abs(val);
    }

    return above;
}

GlobalRegistrationPipeline::ImageType::PixelType* GlobalRegistrationDialog::GetBelow()
{
    GlobalRegistrationPipeline::ImageType::PixelType* below = NULL;

    if (this->GetCheckBelow()->IsChecked())
    {
        below = new GlobalRegistrationPipeline::ImageType::PixelType();
        int val = this->GetSliderBelow()->GetValue();
        *below = (GlobalRegistrationPipeline::ImageType::PixelType) abs(val);
    }

    return below;
}

double GlobalRegistrationDialog::GetSigma()
{
    double sigma = 0.7;
    if (this->GetCheckSmooth()->IsChecked())
    {
        sigma = this->GetSliderSmooth()->GetDoubleValue();
    }

    return sigma;
}

GlobalRegistrationPipeline* GlobalRegistrationDialog::GetPipeline()
{
    if (!this->regPipe)
    {
        this->regPipe = new GlobalRegistrationPipeline();
    }

    return this->regPipe;
}

void GlobalRegistrationDialog::UpdatePipelineValues()
{
    // Logger::logDebug("Updating registration pipeline values.");
    this->GetPipeline()->SetThresholdAbove(this->GetAbove());
    this->GetPipeline()->SetThresholdBelow(this->GetBelow());
    this->GetPipeline()->SetSmoothSigma(this->GetSigma());
}

void GlobalRegistrationDialog::UpdateCanvas()
{
    if (this->GetCheckPreview()->IsChecked() && this->canvas)
    {
        try
        {
            // Logger::logVerbose("Updating registration preview canvas...");
            this->UpdatePipelineValues();

            // Logger::logVerbose("...Updating current fixed image");
            this->GetPipeline()->GetCurrentFixedImage()->Update();
            // Logger::logVerbose("...Loading image data from ITK");
            this->canvas->SetItkImage(this->GetPipeline()->GetCurrentFixedImage());
            // Logger::logVerbose("Canvas updated.");
        }
        catch (itk::ExceptionObject &err)
        {
            Logger::logError("ITK Exception occurred while trying to update canvas.");
            Logger::logError(err.GetDescription());
        }
    }
}

void GlobalRegistrationDialog::UpdateThresholdRange()
{
    if (this->regPipe)
    {
        GlobalRegistrationPipeline::ImageType::PixelType min=0, max=255;
        this->GetPipeline()->GetMinMax(&min, &max);
        this->GetSliderAbove()->SetRange(min, max);
        this->GetSliderBelow()->SetRange(min, max);
    }
}

void GlobalRegistrationDialog::SaveOutput()
{
    if (this->GetPipeline()->IsRegistrationFinished())
    {
        wxString text;

        if (this->GetCheckTransform()->IsChecked() && 
            !this->transformFile.IsEmpty())
        {
            text = "";
            text.Printf("Saving transforms to: %s", this->transformFile.c_str());
            Logger::logInfo(text.c_str());
            TransformGroup::LogTransforms(this->GetPipeline()->GetTransforms());
            this->GetPipeline()->SetTransformFile(this->transformFile.c_str());
            this->GetPipeline()->SaveTransforms();
        }

        if (this->GetCheckImages()->IsChecked() && 
            !(this->outFileDir.IsEmpty() || this->outFilePrefix.IsEmpty()))
        {
            text = "";
            text.Printf("Saving image files to: %s", this->outFileDir.c_str());
            Logger::logInfo(text.c_str());
            FileSet *outFiles = new FileSet(
                this->GetPipeline()->GetInput(), 
                this->outFilePrefix.c_str());
            outFiles->SetDirectory(this->outFileDir.c_str());
            
            this->GetPipeline()->SetOutput(outFiles);
            this->GetPipeline()->SaveImages();
        }
    }
    else
    {
        Logger::logInfo("Registration has not finished...output will not be saved.");
    }
}

// WDR: event table for GlobalRegistrationDialog
BEGIN_EVENT_TABLE(GlobalRegistrationDialog, wxDialog)
    EVT_CHECKBOX( ID_CHECK_ABOVE, GlobalRegistrationDialog::OnCheckAbove )
    EVT_CHECKBOX( ID_CHECK_BELOW, GlobalRegistrationDialog::OnCheckBelow )
    EVT_CHECKBOX( ID_CHECK_SMOOTH, GlobalRegistrationDialog::OnCheckSmooth )
    EVT_CHECKBOX( ID_CHECK_PREVIEW, GlobalRegistrationDialog::OnCheckPreview )
    EVT_BUTTON( ID_NEXT, GlobalRegistrationDialog::OnNext )
    EVT_BUTTON( ID_RUN, GlobalRegistrationDialog::OnRun )
    EVT_BUTTON( ID_HIDE, GlobalRegistrationDialog::OnHide )
    EVT_SLIDER( ID_SLIDER_ABOVE, GlobalRegistrationDialog::OnSliderAbove )
    EVT_SLIDER( ID_SLIDER_BELOW, GlobalRegistrationDialog::OnSliderBelow )
    EVT_SLIDER( ID_SLIDER_SMOOTH, GlobalRegistrationDialog::OnSliderSmooth )
    EVT_CHECKBOX( ID_CHECK_TRANSFORM, GlobalRegistrationDialog::OnCheckTransforms )
    EVT_CHECKBOX( ID_CHECK_IMAGES, GlobalRegistrationDialog::OnCheckImages )
    EVT_BUTTON( ID_TRANSFORM_FILE, GlobalRegistrationDialog::OnTransformFile )
    EVT_BUTTON( ID_OUT_DIRECTORY, GlobalRegistrationDialog::OnOutDirectory )
END_EVENT_TABLE()

// WDR: handler implementations for GlobalRegistrationDialog

void GlobalRegistrationDialog::OnOutDirectory( wxCommandEvent &event )
{
    if (this->GetCheckImages()->IsChecked())
    {
        wxDirDialog dir(this, _("Choose a directory"), this->outFileDir, wxDD_NEW_DIR_BUTTON);
        if (dir.ShowModal() == wxID_OK)
        {
            this->outFileDir = wxString(dir.GetPath());
            this->outFileDir.append("\\");
            
            if (this->transformFile.IsSameAs(""))
            {
                this->transformFile = wxString(dir.GetPath() + "\\Transforms.txt");
            }
        }
    }
    this->UpdateDialog();
}

void GlobalRegistrationDialog::OnTransformFile( wxCommandEvent &event )
{
    if (this->GetCheckTransform()->IsChecked())
    {
        wxFileDialog save(this, _("Choose a file"), this->outFileDir);
        if (save.ShowModal() == wxID_OK)
        {
            this->transformFile = wxString(save.GetPath());

            if (this->outFileDir.IsSameAs(""))
            {
                this->outFileDir = wxString(save.GetDirectory());
                this->outFileDir.append("\\");
            }
        }
    }
    this->UpdateDialog();
}

void GlobalRegistrationDialog::OnCheckImages( wxCommandEvent &event )
{
    this->UpdateDialog();
}

void GlobalRegistrationDialog::OnCheckTransforms( wxCommandEvent &event )
{
    this->UpdateDialog();
}

void GlobalRegistrationDialog::OnNext( wxCommandEvent &event )
{
    this->GetPipeline()->UpdateOne();
    this->UpdateCanvas();
    this->SaveOutput();
}

void GlobalRegistrationDialog::OnSliderSmooth( wxCommandEvent &event )
{
    this->UpdateDialog();
    this->UpdateCanvas();
}

void GlobalRegistrationDialog::OnSliderBelow( wxCommandEvent &event )
{
    this->UpdateDialog();
    this->UpdateCanvas();
}

void GlobalRegistrationDialog::OnSliderAbove( wxCommandEvent &event )
{
    this->UpdateDialog();
    this->UpdateCanvas();
}

void GlobalRegistrationDialog::OnCheckAbove( wxCommandEvent &event )
{
    this->UpdateDialog();
    this->UpdateCanvas();
}

void GlobalRegistrationDialog::OnCheckBelow(wxCommandEvent &event)
{
    this->UpdateDialog();
    this->UpdateCanvas();
}

void GlobalRegistrationDialog::OnCheckSmooth(wxCommandEvent &event)
{
    this->UpdateDialog();
    this->UpdateCanvas();
}

void GlobalRegistrationDialog::OnCheckPreview(wxCommandEvent &event)
{
    this->UpdateCanvas();
}

void GlobalRegistrationDialog::OnRun(wxCommandEvent &event)
{
    Logger::logWarn("Starting registration...this may take a while.");
    this->GetPipeline()->UpdateAll();
    this->SaveOutput();
    Logger::logWarn("Finished.");
}

void GlobalRegistrationDialog::OnHide(wxCommandEvent &event)
{
    this->Show(FALSE);
}
