#pragma once

#include "itkVTKImageExport.h"

#include "vtkImageImport.h"
#include "vtkImageFlip.h"
#include "vtkDataSetSurfaceFilter.h"
#include "vtkArrayCalculator.h"
#include "vtkGlyph3D.h"
#include "vtkMaskPoints.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"

#include "CommonTypes.h"
#include "ItkVtkPipeline.h"

/**
 * Pipeline that connects an itk vector image object to a vtk renderer.
 * This class provides vector image visualization options such glyph type,
 * density, and color mapping.
 */
class Vector2DGlyphItkVtkPipeline :
    public ItkVtkPipeline
{
    public:
        typedef Vector2DGlyphItkVtkPipeline Self;
        typedef ItkVtkPipeline Superclass;
        typedef itk::SmartPointer< Self > Pointer;
        typedef itk::SmartPointer< const Self > ConstPointer;
        itkNewMacro(Self);
    
        typedef ImageTypeV2F2 InputImageType;
        typedef itk::VTKImageExport< InputImageType > ExportType;
    
        virtual void SetInput(itk::DataObject* input);
        virtual void Update();
    
    protected:
        Vector2DGlyphItkVtkPipeline();
        virtual ~Vector2DGlyphItkVtkPipeline();
    
    private:
        ExportType::Pointer exporter;
        vtkImageImport* importer;
        vtkImageFlip* flip;
        vtkDataSetSurfaceFilter* surface;
        vtkArrayCalculator* calculator;
        vtkMaskPoints* mask;
        vtkGlyph3D* glyph;
        vtkPolyDataAlgorithm* glyphSource;
        vtkPolyDataMapper* mapper;
        vtkActor* actor;
};
