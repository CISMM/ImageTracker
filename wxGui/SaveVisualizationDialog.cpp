// -*- C++ -*- generated by wxGlade 0.4.1 on Sun Sep 30 23:43:00 2007

#include "SaveVisualizationDialog.h"

#include <cmath>

#include "FileSet.h"
#include "FileUtils.h"
#include "ImageTrackerController.h"
#include "Logger.h"
#include "wxUtils.h"

bool SaveVisualizationDialog::TransferDataToWindow()
{
    char text[80];
    
    int curIdx = ImageTrackerController::Instance()->GetImageIndex();
    int maxIdx = ImageTrackerController::Instance()->GetImageCount() - 1;
    
    if (this->firstTime)
    {
        this->firstTime = false;

        sprintf(text, "%d", curIdx);
        this->textRangeFrom->SetValue(nano::std2wx(std::string(text)));
        sprintf(text, "%d", maxIdx);
        this->textRangeTo->SetValue(nano::std2wx(std::string(text)));
    }

    sprintf(text, "(Max: %d)", maxIdx);
    this->labelRangeMax->SetLabel(nano::std2wx(std::string(text)));
    sprintf(text, "(%d)", curIdx);
    this->labelFrameCurrent->SetLabel(nano::std2wx(std::string(text)));
    
    return true;
}

bool SaveVisualizationDialog::TransferDataFromWindow()
{
    return true;
}

SaveVisualizationDialog::SaveVisualizationDialog(wxWindow* parent, int id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
    wxDialog(parent, id, title, pos, size, style),
    firstTime(true)
{
    // begin wxGlade: SaveVisualizationDialog::SaveVisualizationDialog
    label_42 = new wxStaticText(this, wxID_ANY, wxT("Directory"));
    textDirectory = new wxTextCtrl(this, wxID_ANY, wxT("."));
    buttonBrowse = new wxButton(this, BUTTON_BROWSE, wxT("&Browse..."));
    label_43 = new wxStaticText(this, wxID_ANY, wxT("File Pattern"));
    textFilePattern = new wxTextCtrl(this, wxID_ANY, wxT("visualization"));
    label_44 = new wxStaticText(this, wxID_ANY, wxT("-%04d.tif"));
    label_45 = new wxStaticText(this, wxID_ANY, wxT("e.g. cell-flowvis"));
    label_49 = new wxStaticText(this, wxID_ANY, wxT("Frames"));
    radFramesAll = new wxRadioButton(this, wxID_ANY, wxT("All"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
    radFramesRange = new wxRadioButton(this, wxID_ANY, wxT("Range"));
    radFramesCurrent = new wxRadioButton(this, wxID_ANY, wxT("Current"));
    label_50 = new wxStaticText(this, wxID_ANY, wxT("From"));
    textRangeFrom = new wxTextCtrl(this, TEXT_RANGE_FROM, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    label_51 = new wxStaticText(this, wxID_ANY, wxT("to"));
    textRangeTo = new wxTextCtrl(this, TEXT_RANGE_TO, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    labelRangeMax = new wxStaticText(this, wxID_ANY, wxT("(max)"));
    labelFrameCurrent = new wxStaticText(this, wxID_ANY, wxT("()"));
    text_ctrl_1 = new wxTextCtrl(this, wxID_ANY, wxT("Note: Do not let any other window obscure the ImageTracker render window and do not minimize ImageTracker when saving visualizations. Doing so will probably corrupt the saved images. The frame index slider will provide progress information, and a message should be printed to the Logger window when finished if the proper logging level is enabled."), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_WORDWRAP);
    buttonRun = new wxButton(this, wxID_OK, wxT("&Run"));
    buttonHide = new wxButton(this, wxID_CANCEL, wxT("&Hide"));

    set_properties();
    do_layout();
    // end wxGlade
}


BEGIN_EVENT_TABLE(SaveVisualizationDialog, wxDialog)
    // begin wxGlade: SaveVisualizationDialog::event_table
    EVT_BUTTON(BUTTON_BROWSE, SaveVisualizationDialog::OnBrowse)
    EVT_TEXT_ENTER(TEXT_RANGE_FROM, SaveVisualizationDialog::OnRangeFrom)
    EVT_TEXT_ENTER(TEXT_RANGE_TO, SaveVisualizationDialog::OnRangeTo)
    EVT_BUTTON(wxID_OK, SaveVisualizationDialog::OnRun)
    EVT_BUTTON(wxID_CANCEL, SaveVisualizationDialog::OnHide)
    // end wxGlade
END_EVENT_TABLE();


void SaveVisualizationDialog::OnBrowse(wxCommandEvent &event)
{
    wxDirDialog dir(this, wxT("Choose a directory"), this->textDirectory->GetValue());
    if (dir.ShowModal() == wxID_OK)
    {
        std::string directory = nano::wx2std(dir.GetPath());
        CapDirectory(directory);
        this->textDirectory->SetValue(nano::std2wx(directory));
    }
}

void SaveVisualizationDialog::OnRangeFrom(wxCommandEvent &event)
{
    std::string function("SaveVisualizationDialog::OnRangeFrom");
    Logger::verbose << function << ": entering" << std::endl;
    int value = atoi(nano::wx2std(this->textRangeFrom->GetValue()).c_str());
    value = std::max(0, value);
    value = std::min(value, (int) (ImageTrackerController::Instance()->GetImageCount()-1));
    char text[80];
    sprintf(text, "%d", value);
    this->textRangeFrom->SetValue(nano::std2wx(std::string(text)));
}


void SaveVisualizationDialog::OnRangeTo(wxCommandEvent &event)
{
    std::string function("SaveVisualizationDialog::OnRangeTo");
    Logger::verbose << function << ": entering" << std::endl;
    int value = atoi(nano::wx2std(this->textRangeTo->GetValue()).c_str());
    value = std::max(0, value);
    value = std::min(value, (int) (ImageTrackerController::Instance()->GetImageCount()-1));
    char text[80];
    sprintf(text, "%d", value);
    this->textRangeTo->SetValue(nano::std2wx(std::string(text)));
}


void SaveVisualizationDialog::OnRun(wxCommandEvent &event)
{
    event.Skip();
}


void SaveVisualizationDialog::OnHide(wxCommandEvent &event)
{
    event.Skip();
}

std::string SaveVisualizationDialog::GetFileFormat()
{
    std::string directory(nano::wx2std(this->textDirectory->GetValue()));
    CapDirectory(directory);
    wxString wxdir(nano::std2wx(directory));
    return nano::wx2std(wxdir.append(this->textFilePattern->GetValue()).append(_("-%04d.tif")));
}

int SaveVisualizationDialog::GetIndexStart()
{
    int from = 0;
    
    if (this->radFramesRange->GetValue())
    {
        from = atoi(nano::wx2std(this->textRangeFrom->GetValue()).c_str());
    }
    else if (this->radFramesCurrent->GetValue())
    {
        from = ImageTrackerController::Instance()->GetImageIndex();
    }
    
    return from;
}

int SaveVisualizationDialog::GetIndexEnd()
{
    int to = ImageTrackerController::Instance()->GetImageCount() - 1;
    
    if (this->radFramesRange->GetValue())
    {
        to = atoi(nano::wx2std(this->textRangeTo->GetValue()).c_str());
    }
    else if (this->radFramesCurrent->GetValue())
    {
        to = ImageTrackerController::Instance()->GetImageIndex();
    }
    
    return to;
}

// wxGlade: add SaveVisualizationDialog event handlers


void SaveVisualizationDialog::set_properties()
{
    // begin wxGlade: SaveVisualizationDialog::set_properties
    SetTitle(wxT("Save Visualizations"));
    SetSize(wxSize(466, 380));
    label_42->SetMinSize(wxSize(80, 17));
    label_43->SetMinSize(wxSize(80, 17));
    label_49->SetMinSize(wxSize(75, 25));
    radFramesAll->SetMinSize(wxSize(75, 25));
    radFramesAll->SetValue(1);
    radFramesRange->SetMinSize(wxSize(75, 25));
    radFramesCurrent->SetMinSize(wxSize(75, 25));
    label_50->SetMinSize(wxSize(32, 25));
    textRangeFrom->SetMinSize(wxSize(80, 27));
    label_51->SetMinSize(wxSize(13, 25));
    textRangeTo->SetMinSize(wxSize(80, 27));
    labelRangeMax->SetMinSize(wxSize(60, 25));
    labelFrameCurrent->SetMinSize(wxSize(80, 25));
    text_ctrl_1->Enable(false);
    // end wxGlade
}


void SaveVisualizationDialog::do_layout()
{
    // begin wxGlade: SaveVisualizationDialog::do_layout
    wxBoxSizer* sizer_43 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_44 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_45 = new wxBoxSizer(wxHORIZONTAL);
    wxFlexGridSizer* grid_sizer_16 = new wxFlexGridSizer(1, 3, 5, 5);
    wxBoxSizer* sizer_49 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_50 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_48 = new wxBoxSizer(wxVERTICAL);
    wxFlexGridSizer* grid_sizer_15 = new wxFlexGridSizer(3, 3, 5, 5);
    grid_sizer_15->Add(label_42, 0, 0, 0);
    grid_sizer_15->Add(textDirectory, 0, wxEXPAND, 0);
    grid_sizer_15->Add(buttonBrowse, 0, 0, 0);
    grid_sizer_15->Add(label_43, 0, 0, 0);
    grid_sizer_15->Add(textFilePattern, 0, wxEXPAND, 0);
    grid_sizer_15->Add(label_44, 0, 0, 0);
    grid_sizer_15->Add(20, 20, 0, 0, 0);
    grid_sizer_15->Add(label_45, 0, 0, 0);
    grid_sizer_15->Add(20, 20, 0, 0, 0);
    grid_sizer_15->AddGrowableCol(1);
    sizer_43->Add(grid_sizer_15, 0, wxEXPAND, 0);
    grid_sizer_16->Add(80, 20, 0, 0, 0);
    sizer_48->Add(label_49, 0, 0, 0);
    sizer_48->Add(radFramesAll, 0, 0, 0);
    sizer_48->Add(radFramesRange, 0, 0, 0);
    sizer_48->Add(radFramesCurrent, 0, 0, 0);
    grid_sizer_16->Add(sizer_48, 1, wxEXPAND, 0);
    sizer_49->Add(75, 25, 0, 0, 0);
    sizer_49->Add(75, 25, 0, 0, 0);
    sizer_50->Add(label_50, 0, wxALIGN_BOTTOM, 0);
    sizer_50->Add(textRangeFrom, 0, wxLEFT|wxRIGHT, 5);
    sizer_50->Add(label_51, 0, wxALIGN_BOTTOM, 0);
    sizer_50->Add(textRangeTo, 0, wxLEFT|wxRIGHT, 5);
    sizer_50->Add(labelRangeMax, 0, wxALIGN_BOTTOM, 0);
    sizer_49->Add(sizer_50, 1, wxEXPAND, 0);
    sizer_49->Add(labelFrameCurrent, 0, 0, 0);
    grid_sizer_16->Add(sizer_49, 1, wxEXPAND, 0);
    grid_sizer_16->AddGrowableCol(2);
    sizer_43->Add(grid_sizer_16, 0, wxEXPAND, 0);
    sizer_45->Add(text_ctrl_1, 1, wxEXPAND, 0);
    sizer_43->Add(sizer_45, 1, wxEXPAND, 0);
    sizer_44->Add(buttonRun, 0, 0, 0);
    sizer_44->Add(buttonHide, 0, 0, 0);
    sizer_43->Add(sizer_44, 0, wxALIGN_CENTER_HORIZONTAL, 0);
    SetSizer(sizer_43);
    Layout();
    // end wxGlade
}

