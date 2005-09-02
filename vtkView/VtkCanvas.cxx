/////////////////////////////////////////////////////////////////////////////
// Name:        VtkCanvas.cxx
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   XX
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "VtkCanvas.cxx"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "VtkCanvas.h"

// WDR: class implementations

//----------------------------------------------------------------------------
// VtkCanvas
//----------------------------------------------------------------------------

// WDR: event table for VtkCanvas

BEGIN_EVENT_TABLE(VtkCanvas,wxPanel)
END_EVENT_TABLE()

VtkCanvas::VtkCanvas( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxPanel( parent, id, position, size, style )
{
    this->rescaler = NULL;
    this->converter = NULL;
    this->flipper = NULL;
    this->actor = NULL;
    this->renderer = NULL;
    this->firstTime = true;

    // WDR: dialog function VtkViewerDialog for VtkCanvas
    CreateVtkCanvas( this, TRUE );
    this->GetVtkInteractor()->UseCaptureMouseOn();
    this->DefaultRenderer();
}

VtkCanvas::~VtkCanvas()
{
    if (this->GetVtkInteractor())
    {
        this->renderer->RemoveActor(this->actor);
        this->GetVtkInteractor()->GetRenderWindow()->RemoveRenderer(this->renderer);
        this->GetVtkInteractor()->Delete();
    }
    if (this->renderer)
        this->renderer->Delete();
    if (this->actor)
        this->actor->Delete();
    if (this->flipper)
        this->flipper->Delete();
}

void VtkCanvas::DefaultRenderer()
{
    if (!this->renderer)
        this->renderer = vtkRenderer::New();

    this->GetVtkInteractor()->GetRenderWindow()->AddRenderer(this->renderer);
}

void VtkCanvas::InitializePipeline()
{
    if (this->renderer)
        this->renderer->Delete();

    this->reader = ItkMagickIO::New();
    this->rescaler = RescalerType::New();
    this->converter = ConverterType::New();
    this->flipper = vtkImageFlip::New();
    this->actor = vtkImageActor::New();
    this->renderer = vtkRenderer::New();

    this->rescaler->SetOutputMinimum(0);
    this->rescaler->SetOutputMaximum(255);
    this->flipper->SetFilteredAxis(1); // flip the y axis.

    this->converter->SetInput(this->rescaler->GetOutput());
    this->flipper->SetInput(this->converter->GetOutput());
    this->actor->SetInput(this->flipper->GetOutput());
    this->renderer->AddActor(this->actor);
    this->GetVtkInteractor()->GetRenderWindow()->AddRenderer(this->renderer);
}

void VtkCanvas::SetFileName(std::string filename)
{
    if (this->firstTime)
    {
        this->InitializePipeline();
        this->rescaler->SetInput(this->reader->Read(filename.c_str()));
    }
    else
    {
        this->rescaler->SetInput(this->reader->Read(filename.c_str()));    
        this->renderer->RemoveActor(this->actor);
        this->actor->Delete();
        this->actor = vtkImageActor::New();
        this->actor->SetInput(this->flipper->GetOutput());
        this->renderer->AddActor(this->actor);
    }

    this->GetVtkInteractor()->GetRenderWindow()->Render();
    this->firstTime = false;
}

void VtkCanvas::SetItkImage(InputImageType::Pointer image)
{
    if (this->firstTime)
    {
        this->InitializePipeline();
        this->rescaler->SetInput(image);
    }
    else
    {
        this->rescaler->SetInput(image);
        this->renderer->RemoveActor(this->actor);
        this->actor->Delete();
        this->actor = vtkImageActor::New();
        this->actor->SetInput(this->flipper->GetOutput());
        this->renderer->AddActor(this->actor);
    }

    this->GetVtkInteractor()->GetRenderWindow()->Render();
    this->firstTime = false;
}

// WDR: handler implementations for VtkCanvas
