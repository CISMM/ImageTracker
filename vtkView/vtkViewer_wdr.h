//------------------------------------------------------------------------------
// Header generated by wxDesigner from file: vtkViewer.wdr
// Do not modify this file, all changes will be lost!
//------------------------------------------------------------------------------

#ifndef __WDR_vtkViewer_H__
#define __WDR_vtkViewer_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "vtkViewer_wdr.h"
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

const int ID_VTK_INTERACTOR = 10000;
wxSizer *CreateVtkCanvas( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_CANVAS = 10001;
wxSizer *CreateVtkViewer( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

// Declare menubar functions

const int ID_FILE = 10002;
const int ID_FILE_OPEN = 10003;
const int ID_FILE_EXIT = 10004;
wxMenuBar *VtkViewerMenu();

// Declare toolbar functions

// Declare bitmap functions

#endif

// End of generated file
