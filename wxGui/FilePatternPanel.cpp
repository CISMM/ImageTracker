// -*- C++ -*- generated by wxGlade 0.4.1 on Fri Oct 12 11:02:51 2007

#include "FilePatternPanel.h"

#include "FileUtils.h"
#include "wxUtils.h"

FilePatternPanel::FilePatternPanel(wxWindow* parent, int id, const wxPoint& pos, const wxSize& size, long style):
    wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL),
    pattern(".", "file-%04d.tif", 0, 10)
{
    // begin wxGlade: FilePatternPanel::FilePatternPanel
    label_62 = new wxStaticText(this, wxID_ANY, wxT("Directory"));
    textDirectory = new wxTextCtrl(this, TEXT_DIRECTORY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    buttonBrowse = new wxButton(this, BUTTON_BROWSE, wxT("&Browse"));
    label_57_copy = new wxStaticText(this, wxID_ANY, wxT("Prefix"));
    textPrefix = new wxTextCtrl(this, TEXT_PREFIX, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    label_58_copy = new wxStaticText(this, wxID_ANY, wxT("Number format"));
    textNumberFormat = new wxTextCtrl(this, TEXT_NUMBER_FORMAT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    label_7 = new wxStaticText(this, wxID_ANY, wxT("(e.g. %04d)"));
    label_59_copy = new wxStaticText(this, wxID_ANY, wxT("Extension"));
    textExtension = new wxTextCtrl(this, TEXT_EXTENSION, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    label_63_copy = new wxStaticText(this, wxID_ANY, wxT("File index range"));
    label_64_copy = new wxStaticText(this, wxID_ANY, wxT("From"));
    textRangeStart = new wxTextCtrl(this, TEXT_RANGE_START, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    label_65_copy = new wxStaticText(this, wxID_ANY, wxT("To"));
    textRangeEnd = new wxTextCtrl(this, TEXT_RANGE_END, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    label_60_copy = new wxStaticText(this, wxID_ANY, wxT("Example:"));
    labelExample = new wxStaticText(this, wxID_ANY, wxEmptyString);

    set_properties();
    do_layout();
    // end wxGlade
}

bool FilePatternPanel::TransferDataToWindow()
{
    this->textDirectory->SetValue(nano::std2wx(this->pattern.directory));

    std::string prefix(PrefixPart(this->pattern.format));
    std::string format(FormatPart(this->pattern.format));
    std::string extension(ExtensionPart(this->pattern.format));
    
    this->textPrefix->SetValue(nano::std2wx(prefix));
    this->textNumberFormat->SetValue(nano::std2wx(format));
    this->textExtension->SetValue(nano::std2wx(extension));
    this->textRangeStart->SetValue(wxString::Format(wxT("%d"), this->pattern.start));
    this->textRangeEnd->SetValue(wxString::Format(wxT("%d"), this->pattern.end));
    
    this->UpdateExample();
    
    return true;
}

bool FilePatternPanel::TransferDataFromWindow()
{
    return true;
}

BEGIN_EVENT_TABLE(FilePatternPanel, wxPanel)
    // begin wxGlade: FilePatternPanel::event_table
    EVT_TEXT_ENTER(TEXT_DIRECTORY, FilePatternPanel::OnDirectory)
    EVT_BUTTON(BUTTON_BROWSE, FilePatternPanel::OnBrowse)
    EVT_TEXT_ENTER(TEXT_PREFIX, FilePatternPanel::OnPrefix)
    EVT_TEXT_ENTER(TEXT_NUMBER_FORMAT, FilePatternPanel::OnFormat)
    EVT_TEXT_ENTER(TEXT_EXTENSION, FilePatternPanel::OnExtension)
    EVT_TEXT_ENTER(TEXT_RANGE_START, FilePatternPanel::OnRangeStart)
    EVT_TEXT_ENTER(TEXT_RANGE_END, FilePatternPanel::OnRangeEnd)
    // end wxGlade
END_EVENT_TABLE();


void FilePatternPanel::OnDirectory(wxCommandEvent &event)
{
    std::string dir = nano::wx2std(this->textDirectory->GetValue());
    CapDirectory(dir);
    this->pattern.directory = dir;
    this->textDirectory->SetValue(nano::std2wx(dir));
    this->UpdateExample();
}


void FilePatternPanel::OnBrowse(wxCommandEvent &event)
{
    wxDirDialog dirDlg(this, wxT("Choose a directory"), this->textDirectory->GetValue());
    if (dirDlg.ShowModal() == wxID_OK)
    {
        std::string dir = nano::wx2std(dirDlg.GetPath());
        CapDirectory(dir);
        this->textDirectory->SetValue(nano::std2wx(dir));
        this->pattern.directory = dir;
        this->UpdateExample();
    }
}


void FilePatternPanel::OnPrefix(wxCommandEvent &event)
{
    this->pattern.format = nano::wx2std(this->textPrefix->GetValue().
            Append(this->textNumberFormat->GetValue()).
            Append(this->textExtension->GetValue()));
    this->UpdateExample();
}


void FilePatternPanel::OnFormat(wxCommandEvent &event)
{
    this->pattern.format = nano::wx2std(this->textPrefix->GetValue().
            Append(this->textNumberFormat->GetValue()).
            Append(this->textExtension->GetValue()));
    this->UpdateExample();
}


void FilePatternPanel::OnExtension(wxCommandEvent &event)
{
    this->pattern.format = nano::wx2std(this->textPrefix->GetValue().
            Append(this->textNumberFormat->GetValue()).
            Append(this->textExtension->GetValue()));
    this->UpdateExample();
}


void FilePatternPanel::OnRangeStart(wxCommandEvent &event)
{
    unsigned long start = 0;
    if (this->textRangeStart->GetValue().ToULong(&start))
    {
        this->pattern.start = start;
    }
    else
    {
        this->pattern.start = 0;
        this->textRangeStart->SetValue(wxT("0"));
    }
    this->UpdateExample();
}


void FilePatternPanel::OnRangeEnd(wxCommandEvent &event)
{
    unsigned long end = 0;
    if (this->textRangeEnd->GetValue().ToULong(&end))
    {
        this->pattern.end = end;
    }
    else
    {
        this->pattern.end = 0;
        this->textRangeEnd->SetValue(wxT("0"));
    }
    this->UpdateExample();
}


// wxGlade: add FilePatternPanel event handlers


void FilePatternPanel::UpdateExample()
{
    wxString format(nano::std2wx(pattern.directory));
    format.Append(nano::std2wx(pattern.format));
    this->labelExample->SetLabel(wxString::Format(format, pattern.start));
}

void FilePatternPanel::SetFilePattern(const FilePattern& pat)
{
    this->pattern = pat;
}

FilePattern FilePatternPanel::GetFilePattern()
{
    return this->pattern;
}

void FilePatternPanel::SetExtensionEnabled(bool enable)
{
    this->textExtension->Enable(enable);
}

bool FilePatternPanel::GetExtensionEnabled()
{
    return this->textExtension->IsEnabled();
}

void FilePatternPanel::SetRangeEnabled(bool enable)
{
    this->textRangeStart->Enable(enable);
    this->textRangeEnd->Enable(enable);
}

bool FilePatternPanel::GetRangeEnabled()
{
    return this->textRangeStart->IsEnabled();
}

void FilePatternPanel::set_properties()
{
    // begin wxGlade: FilePatternPanel::set_properties
    textDirectory->SetToolTip(wxT("The directory in which all files reside"));
    buttonBrowse->SetToolTip(wxT("Find a directory on this computer"));
    textPrefix->SetToolTip(wxT("The alpha-numeric component common to all files in this pattern"));
    textNumberFormat->SetToolTip(wxT("A C printf-style number format"));
    textExtension->SetToolTip(wxT("The file extension"));
    textRangeStart->SetToolTip(wxT("The starting index of files in this pattern"));
    textRangeEnd->SetToolTip(wxT("The final index of files in the pattern"));
    // end wxGlade
}

void FilePatternPanel::do_layout()
{
    // begin wxGlade: FilePatternPanel::do_layout
    wxBoxSizer* sizer_57 = new wxBoxSizer(wxVERTICAL);
    wxFlexGridSizer* grid_sizer_21 = new wxFlexGridSizer(6, 3, 5, 5);
    wxBoxSizer* sizer_31 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_62_copy = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_61_copy = new wxBoxSizer(wxHORIZONTAL);
    grid_sizer_21->Add(label_62, 0, wxALIGN_CENTER_VERTICAL, 0);
    grid_sizer_21->Add(textDirectory, 0, wxEXPAND, 0);
    grid_sizer_21->Add(buttonBrowse, 0, 0, 0);
    grid_sizer_21->Add(label_57_copy, 0, 0, 0);
    grid_sizer_21->Add(textPrefix, 0, wxEXPAND, 0);
    grid_sizer_21->Add(20, 20, 0, 0, 0);
    grid_sizer_21->Add(label_58_copy, 0, 0, 0);
    grid_sizer_21->Add(textNumberFormat, 0, wxEXPAND, 0);
    grid_sizer_21->Add(label_7, 0, 0, 0);
    grid_sizer_21->Add(label_59_copy, 0, 0, 0);
    grid_sizer_21->Add(textExtension, 0, wxEXPAND, 0);
    grid_sizer_21->Add(20, 20, 0, 0, 0);
    grid_sizer_21->Add(label_63_copy, 0, wxRIGHT|wxALIGN_CENTER_VERTICAL, 10);
    sizer_61_copy->Add(label_64_copy, 0, wxRIGHT|wxALIGN_CENTER_VERTICAL, 5);
    sizer_61_copy->Add(textRangeStart, 0, 0, 0);
    sizer_31->Add(sizer_61_copy, 1, wxEXPAND, 0);
    sizer_62_copy->Add(label_65_copy, 0, wxRIGHT|wxALIGN_CENTER_VERTICAL, 5);
    sizer_62_copy->Add(textRangeEnd, 0, 0, 0);
    sizer_31->Add(sizer_62_copy, 1, wxEXPAND, 0);
    grid_sizer_21->Add(sizer_31, 1, wxEXPAND, 0);
    grid_sizer_21->Add(20, 20, 0, 0, 0);
    grid_sizer_21->Add(label_60_copy, 0, wxTOP, 10);
    grid_sizer_21->Add(labelExample, 1, wxTOP|wxEXPAND, 10);
    grid_sizer_21->Add(20, 20, 0, 0, 0);
    grid_sizer_21->AddGrowableCol(1);
    sizer_57->Add(grid_sizer_21, 0, wxTOP|wxBOTTOM|wxEXPAND, 10);
    SetSizer(sizer_57);
    sizer_57->Fit(this);
    // end wxGlade
}

