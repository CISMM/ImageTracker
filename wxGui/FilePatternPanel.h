// -*- C++ -*- generated by wxGlade 0.4.1 on Fri Oct 12 11:02:51 2007

#include <wx/wx.h>
#include <wx/image.h>

#ifndef FILEPATTERNPANEL_H
#define FILEPATTERNPANEL_H

#include "FilePattern.h"

// begin wxGlade: ::dependencies
// end wxGlade


class FilePatternPanel: public wxPanel {
public:
    // begin wxGlade: FilePatternPanel::ids
    // end wxGlade
    
    enum IDs
    {
        TXT_DIRECTORY=7090,
        BTN_BROWSE,
        TXT_PREFIX,
        TXT_NUMBER_FORMAT,
        TXT_EXTENSION,
        TXT_RANGE_START,
        TXT_RANGE_END
    };
    
    FilePatternPanel(wxWindow* parent, int id, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=0);

    virtual bool TransferDataToWindow();
    virtual bool TransferDataFromWindow();
    
    void SetFilePattern(const FilePattern& pattern);
    FilePattern GetFilePattern();
    
    void SetExtensionEnabled(bool enable);
    bool GetExtensionEnabled();
    void SetRangeEnabled(bool enable);
    bool GetRangeEnabled();
    
private:
    // begin wxGlade: FilePatternPanel::methods
    void set_properties();
    void do_layout();
    // end wxGlade

    FilePattern pattern;
    
protected:
    // begin wxGlade: FilePatternPanel::attributes
    wxStaticText* label_62;
    wxTextCtrl* textDirectory;
    wxButton* buttonBrowse;
    wxStaticText* label_57_copy;
    wxTextCtrl* textPrefix;
    wxStaticText* label_58_copy;
    wxTextCtrl* textNumberFormat;
    wxStaticText* label_7;
    wxStaticText* label_59_copy;
    wxTextCtrl* textExtension;
    wxStaticText* label_63_copy;
    wxStaticText* label_64_copy;
    wxTextCtrl* textRangeStart;
    wxStaticText* label_65_copy;
    wxTextCtrl* textRangeEnd;
    wxStaticText* label_60_copy;
    wxStaticText* labelExample;
    // end wxGlade

    DECLARE_EVENT_TABLE();
    
    void UpdateExample();

public:
    void OnDirectory(wxCommandEvent &event); // wxGlade: <event_handler>
    void OnBrowse(wxCommandEvent &event); // wxGlade: <event_handler>
    void OnPrefix(wxCommandEvent &event); // wxGlade: <event_handler>
    void OnFormat(wxCommandEvent &event); // wxGlade: <event_handler>
    void OnExtension(wxCommandEvent &event); // wxGlade: <event_handler>
    void OnRangeStart(wxCommandEvent &event); // wxGlade: <event_handler>
    void OnRangeEnd(wxCommandEvent &event); // wxGlade: <event_handler>
}; // wxGlade: end class


#endif // FILEPATTERNPANEL_H
