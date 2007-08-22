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
class VectorGlyphPipeline :
    public ItkVtkPipeline
{
    public:
        typedef VectorGlyphPipeline Self;
        typedef ItkVtkPipeline Superclass;
        typedef itk::SmartPointer< Self > Pointer;
        typedef itk::SmartPointer< const Self > ConstPointer;
        itkNewMacro(Self);
    
        typedef ImageTypeV2F2 InputImageType;
        typedef itk::VTKImageExport< InputImageType > ExportType;
    
        virtual void SetInput(itk::DataObject* input);
        virtual void Update();
        
        virtual wxWindow* CreateWxControl(wxWindow* parent);
    
        /**
         * Get/Set the ratio between data set pixels and masked glyphs.
         */
        void SetMaskRatio(int ratio) 
        { 
            this->mask->SetOnRatio(ratio); 
            this->mask->Modified();
        }
        int GetMaskRatio() 
        { 
            return this->mask->GetOnRatio(); 
        }
        
        /**
         * Get/Set the vector magnitude scale factor.
         */
        void SetScaleFactor(double factor) 
        { 
            this->glyph->SetScaleFactor(factor);
            this->glyph->Modified();
        }
        double GetScaleFactor() { return this->glyph->GetScaleFactor(); }
        
        /**
         * Get/Set the visibility of the glyphs.
         */
        void SetVisibility(int visible) 
        { this->actor->SetVisibility(visible); }
        int GetVisibility() 
        { return this->actor->GetVisibility(); }
        
    protected:
        VectorGlyphPipeline();
        virtual ~VectorGlyphPipeline();
    
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
