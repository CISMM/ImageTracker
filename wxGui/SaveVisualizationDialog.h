// -*- C++ -*- generated by wxGlade 0.4.1 on Sun Sep 30 23:43:00 2007

#include <wx/wx.h>
#include <wx/image.h>

#ifndef SAVEVISUALIZATIONDIALOG_H
#define SAVEVISUALIZATIONDIALOG_H

// begin wxGlade: ::dependencies
// end wxGlade


class SaveVisualizationDialog: public wxDialog {
public:
    // begin wxGlade: SaveVisualizationDialog::ids
    // end wxGlade

    SaveVisualizationDialog(wxWindow* parent, int id, const wxString& title, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxTHICK_FRAME);
    
    enum IDs
    {
        BTN_BROWSE = 3070,
        TXT_RANGE_FROM,
        TXT_RANGE_TO
    };
    
    std::string GetFileFormat();
    int GetIndexStart();
    int GetIndexEnd();

private:
    // begin wxGlade: SaveVisualizationDialog::methods
    void set_properties();
    void do_layout();
    // end wxGlade

    bool firstTime;
    
protected:
    // begin wxGlade: SaveVisualizationDialog::attributes
    wxStaticText* label_42;
    wxTextCtrl* textDirectory;
    wxButton* btnBrowse;
    wxStaticText* label_43;
    wxTextCtrl* textFilePattern;
    wxStaticText* label_44;
    wxStaticText* label_45;
    wxStaticText* label_49;
    wxRadioButton* radFramesAll;
    wxRadioButton* radFramesRange;
    wxRadioButton* radFramesCurrent;
    wxStaticText* label_50;
    wxTextCtrl* textRangeFrom;
    wxStaticText* label_51;
    wxTextCtrl* textRangeTo;
    wxStaticText* labelRangeMax;
    wxStaticText* labelFrameCurrent;
    wxTextCtrl* text_ctrl_1;
    wxButton* btnRun;
    wxButton* btnHide;
    // end wxGlade

    DECLARE_EVENT_TABLE();

public:
    void OnBrowse(wxCommandEvent &event); // wxGlade: <event_handler>
    void OnRangeFrom(wxCommandEvent &event); // wxGlade: <event_handler>
    void OnRangeTo(wxCommandEvent &event); // wxGlade: <event_handler>
    void OnRun(wxCommandEvent &event); // wxGlade: <event_handler>
    void OnHide(wxCommandEvent &event); // wxGlade: <event_handler>
    
    virtual bool TransferDataToWindow();
    virtual bool TransferDataFromWindow();
}; // wxGlade: end class


#endif // SAVEVISUALIZATIONDIALOG_H
