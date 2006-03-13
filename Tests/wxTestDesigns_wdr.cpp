//------------------------------------------------------------------------------
// Source code generated by wxDesigner from file: wxTestDesigns.wdr
// Do not modify this file, all changes will be lost!
//------------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "wxTestDesigns_wdr.h"
#endif

// For compilers that support precompilation
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// Include private header
#include "wxTestDesigns_wdr.h"


// Euro sign hack of the year
#if wxUSE_UNICODE
    #define __WDR_EURO__ wxT("\u20ac")
#else
    #if defined(__WXMAC__)
        #define __WDR_EURO__ wxT("\xdb")
    #elif defined(__WXMSW__)
        #define __WDR_EURO__ wxT("\x80")
    #else
        #define __WDR_EURO__ wxT("\xa4")
    #endif
#endif

// Custom source
#include "../wxGUI/LoggerPanel.h"

// Implement window functions

wxSizer *CreateTestDialog( wxWindow *parent, bool call_fit, bool set_sizer )
{
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    LoggerPanel *item1 = new LoggerPanel( parent, ID_LOG_PANEL, wxDefaultPosition, wxSize(200,160), 0 );
    item0->Add( item1, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}

// Implement menubar functions

wxMenuBar *MyMenuBarFunc()
{
    wxMenuBar *item0 = new wxMenuBar;
    
    wxMenu* item1 = new wxMenu;
    item1->Append( ID_TEST, wxT("Run Tests"), wxT("") );
    item1->Append( wxID_EXIT, wxT("Quit"), wxT("") );
    item0->Append( item1, wxT("File") );
    
    return item0;
}

// Implement toolbar functions

// Implement bitmap functions


// End of generated file
