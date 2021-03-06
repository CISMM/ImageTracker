// -*- C++ -*- generated by wxGlade 0.4.1 on Thu Oct 19 12:07:54 2006

#include "AboutDialog.h"

#include "wxUtils.h"

AboutDialog::AboutDialog(wxWindow* parent, int id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
    wxDialog(parent, id, title, pos, size, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)
{
    // begin wxGlade: AboutDialog::AboutDialog
    textMessage = new wxTextCtrl(this, wxID_ANY, wxT("ImageTracker"), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_AUTO_URL|wxTE_CENTRE);
    buttonOK = new wxButton(this, wxID_OK, wxT("&OK"));

    set_properties();
    do_layout();
    // end wxGlade
}


void AboutDialog::set_properties()
{
    // begin wxGlade: AboutDialog::set_properties
    SetTitle(wxT("About ImageTracker"));
    SetSize(wxSize(330, 100));
    buttonOK->SetDefault();
    // end wxGlade
}


void AboutDialog::do_layout()
{
    // begin wxGlade: AboutDialog::do_layout
    wxBoxSizer* sizer_8 = new wxBoxSizer(wxVERTICAL);
    sizer_8->Add(textMessage, 1, wxEXPAND, 0);
    sizer_8->Add(buttonOK, 0, wxALIGN_CENTER_HORIZONTAL, 0);
    SetSizer(sizer_8);
    Layout();
    // end wxGlade
}

void AboutDialog::SetMessage(const std::string& message)
{
    this->textMessage->SetValue(nano::std2wx(message));
}
