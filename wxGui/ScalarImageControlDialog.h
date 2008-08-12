// -*- C++ -*- generated by wxGlade 0.6.3 on Mon Aug  4 15:49:54 2008

#include <wx/wx.h>
#include <wx/image.h>

#ifndef SCALARIMAGECONTROLDIALOG_H
#define SCALARIMAGECONTROLDIALOG_H

// begin wxGlade: ::dependencies
// end wxGlade

// begin wxGlade: ::extracode
#include "ScalarImageControlPanel.h"

// end wxGlade

class ScalarImageControlDialog: public wxDialog {
public:
    // begin wxGlade: ScalarImageControlDialog::ids
    // end wxGlade

    ScalarImageControlDialog(wxWindow* parent, int id, const wxString& title, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=wxDEFAULT_DIALOG_STYLE);
    
    virtual bool TransferDataToWindow();

private:
    // begin wxGlade: ScalarImageControlDialog::methods
    void set_properties();
    void do_layout();
    // end wxGlade

protected:
    // begin wxGlade: ScalarImageControlDialog::attributes
    ScalarImageControlPanel* panelScalarImageControl;
    // end wxGlade
}; // wxGlade: end class


#endif // SCALARIMAGECONTROLDIALOG_H