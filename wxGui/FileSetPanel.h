// -*- C++ -*- generated by wxGlade 0.6.3 on Thu Jul 31 10:07:27 2008

#include <wx/wx.h>
#include <wx/image.h>

#ifndef FILESETPANEL_H
#define FILESETPANEL_H

// begin wxGlade: ::dependencies
// end wxGlade

// begin wxGlade: ::extracode

// end wxGlade

#include <string>

#include "FilePatternDialog.h"
#include "FileSet.h"

/**
 * \class FileSetPanel
 * \brief Control for specifying a FileSet.
 * 
 * Control that enables users to create a list of files.
 */
class FileSetPanel: public wxPanel {
public:
    // begin wxGlade: FileSetPanel::ids
    enum {
        BUTTON_ADD_EXAMPLE = wxID_HIGHEST + 1131,
        BUTTON_ADD_SELECTION = wxID_HIGHEST + 1133,
        BUTTON_ADD_PATTERN = wxID_HIGHEST + 1135,
        BUTTON_REMOVE_SELECTION = wxID_HIGHEST + 1137
    };
    // end wxGlade

    FileSetPanel(wxWindow* parent, int id, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=0);
    
    FileSet& GetFileSet();
    void SetFileSet(const FileSet& files);
    int GetCount();
    
    void SetFilter(const std::string& filter);

private:
    // begin wxGlade: FileSetPanel::methods
    void set_properties();
    void do_layout();
    // end wxGlade

protected:
    // begin wxGlade: FileSetPanel::attributes
    wxStaticBox* sizerFileList_staticbox;
    wxListBox* listFiles;
    wxButton* buttonAddExample;
    wxButton* buttonAddSelection;
    wxButton* buttonAddPattern;
    wxButton* buttonRemoveSelection;
    // end wxGlade

    DECLARE_EVENT_TABLE();
    
    std::string filter;
    FileSet files;
    FilePatternDialog* dialogFilePattern;

public:
    virtual void OnAddExample(wxCommandEvent &event); // wxGlade: <event_handler>
    virtual void OnAddSelection(wxCommandEvent &event); // wxGlade: <event_handler>
    virtual void OnAddPattern(wxCommandEvent &event); // wxGlade: <event_handler>
    virtual void OnRemoveSelection(wxCommandEvent &event); // wxGlade: <event_handler>
}; // wxGlade: end class


#endif // FILESETPANEL_H
