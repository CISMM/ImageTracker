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
#include "ConnectVTKITK.h"

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
    // Initialize pointers
	this->reader = 0;
    this->rescaler = 0;
	this->exporter = 0;
	this->importer = 0;
    this->flipper = 0;
    this->actor = 0;
    this->renderer = 0;
    this->style = 0;

    this->firstTime = true;

    // WDR: dialog function VtkViewerDialog for VtkCanvas
    CreateVtkCanvas( this, TRUE );
    this->GetVtkInteractor()->UseCaptureMouseOn();
    // Display...nothing!
    this->DefaultRenderer();
}

VtkCanvas::~VtkCanvas()
{
    // We have to disassemble the pipeline carefully.  If we
    // delete an element at the wrong time, we get an error
    // because someone tries to use an invalid reference.  The
    // "magic" order is ill-documented, but this seems to work.
    if (this->GetVtkInteractor())
    {
        this->renderer->RemoveActor(this->actor);
        this->GetVtkInteractor()->GetRenderWindow()->RemoveRenderer(this->renderer);
        this->GetVtkInteractor()->Delete();
    }

    // Delete the vtk pipeline elements, since these
    // objects aren't smart pointers (unlike itk objects).
    if (this->renderer)
        this->renderer->Delete();
    if (this->actor)
        this->actor->Delete();
    if (this->flipper)
        this->flipper->Delete();
	if (this->importer)
		this->importer->Delete();
    if (this->style)
        this->style->Delete();
}

void VtkCanvas::DefaultRenderer()
{
    // A default renderer has nothing in it.  But, it's neccessary
    // when first displaying the canvas--otherwise, there is an error
    // because vtk has no active renderer to display.
    if (!this->renderer)
        this->renderer = vtkRenderer::New();

    this->GetVtkInteractor()->GetRenderWindow()->AddRenderer(this->renderer);
}

void VtkCanvas::InitializePipeline()
{
    // Delete the old (default) renderer.
    if (this->renderer)
        this->renderer->Delete();

    // Create all the pipeline components
    this->reader = ReaderType::New();
	this->caster = CasterType::New();
    this->rescaler = RescalerType::New();
	this->exporter = ExporterType::New();

	this->importer = vtkImageImport::New();
	this->flipper = vtkImageFlip::New();
    this->actor = vtkImageActor::New();
    this->renderer = vtkRenderer::New();
    this->style = vtkInteractorStyleImage::New(); // restricts motion to xy plane

	// Set up the read image part of the pipeline
	this->caster->SetInput(reader->GetOutput());

    // Rescale intensities for max contrast on 8-bit displays
    this->rescaler->SetOutputMinimum(0);
    this->rescaler->SetOutputMaximum(255);
    this->flipper->SetFilteredAxis(1); // flip the y axis.

    // Assemble part of the pipeline.  Image input depends on
    // whether we are using a file name or an itk image.  But, after
    // we have itk image data, we need to convert it to vtk data, 
    // flip the axis, create an actor, and add it to the renderer.
	this->exporter->SetInput(this->rescaler->GetOutput());
	ConnectITKToVTK<DisplayImageType>(this->exporter, this->importer);
    
	this->flipper->SetInput(this->importer->GetOutput());
    this->actor->SetInput(this->flipper->GetOutput());
    this->renderer->AddActor(this->actor);
    this->GetVtkInteractor()->GetRenderWindow()->AddRenderer(this->renderer);
    this->GetVtkInteractor()->SetInteractorStyle(this->style);
}

void VtkCanvas::SetFileName(std::string filename)
{
    if (this->firstTime)
    {
        this->InitializePipeline();
		this->reader->SetFileName(filename.c_str());
		this->rescaler->SetInput(this->caster->GetOutput());
    }
    else
    {
        // Read the image data into the pipeline
		reader->SetFileName(filename.c_str());
		this->rescaler->SetInput(this->caster->GetOutput());

		// The importer needs to be updated to refresh the image
		this->importer->Update();

        // Removing the existing actor and creating a new one
        // ensures we see the whole image if the new image is
        // different size from the old one.  But, we still 
        // maintain the image transforms (zoom and pan) from the
        // old view.
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
        // Set the image data in the pipeline
        this->rescaler->SetInput(image);
		// The importer needs to be updated to refresh the image
		this->importer->Update();

        // Removing the existing actor and creating a new one
        // ensures we see the whole image if the new image is
        // different size from the old one.  But, we still 
        // maintain the image transforms (zoom and pan) from the
        // old view.
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
