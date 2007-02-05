// -*- C++ -*- generated by wxGlade 0.4.1 on Thu Oct 19 12:07:54 2006

#include <wx/wx.h>
#include <wx/image.h>

#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

// begin wxGlade: ::dependencies
// end wxGlade

#include "wxUtils.h"

class AboutDialog: public wxDialog {
public:
    // begin wxGlade: AboutDialog::ids
    // end wxGlade

    AboutDialog(wxWindow* parent, int id, const wxString& title, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=wxDEFAULT_DIALOG_STYLE);
    
    void SetMessage(const std::string& message)
    {
        this->txtLabel->SetLabel(std2wx(message));
    }

private:
    // begin wxGlade: AboutDialog::methods
    void set_properties();
    void do_layout();
    // end wxGlade

protected:
    // begin wxGlade: AboutDialog::attributes
    wxStaticText* txtLabel;
    wxButton* btnOK;
    // end wxGlade
}; // wxGlade: end class


#endif // ABOUTDIALOG_H
