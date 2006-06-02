//------------------------------------------------------------------------------
// Header generated by wxDesigner from file: wxCore.wdr
// Do not modify this file, all changes will be lost!
//------------------------------------------------------------------------------

#ifndef __WDR_wxCore_H__
#define __WDR_wxCore_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "wxCore_wdr.h"
#endif

// Include wxWidgets' headers

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/image.h>
#include <wx/statline.h>
#include <wx/spinbutt.h>
#include <wx/spinctrl.h>
#include <wx/splitter.h>
#include <wx/listctrl.h>
#include <wx/treectrl.h>
#include <wx/notebook.h>
#include <wx/grid.h>
#include <wx/toolbar.h>

// Declare window functions

const int ID_TEXT_BOX = 10000;
wxSizer *CreateLogPanel( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_TEST_PANEL = 10001;
wxSizer *CreateLoggerTest( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int FS_SLIDER = 10002;
const int FS_TEXTCTRL = 10003;
const int FS_LABEL_LEFT = 10004;
const int FS_LABEL_RIGHT = 10005;
wxSizer *CreateDoubleSlider( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int IS_SLIDER = 10006;
const int IS_TEXTCTRL = 10007;
const int IS_LABEL_LEFT = 10008;
const int IS_LABEL_RIGHT = 10009;
wxSizer *CreateIntSlider( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

// Declare menubar functions

// Declare toolbar functions

// Declare bitmap functions

#endif

// End of generated file