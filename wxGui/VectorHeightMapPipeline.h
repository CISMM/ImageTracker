#pragma once

#include <string>

#include "itkVTKImageExport.h"

#include "vtkActor.h"
#include "vtkArrayCalculator.h"
#include "vtkDataSetSurfaceFilter.h"
#include "vtkImageFlip.h"
#include "vtkImageImport.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyDataNormals.h"
#include "vtkScalarsToColors.h"
#include "vtkWarpScalar.h"

#include "CommonTypes.h"
#include "ItkVtkPipeline.h"
#include "LookupTableUtil.h"

/**
 * \class VectorHeightMapPipeline.h
 * \brief Displays a vector field as a height map.
 * Pipeline that connects an itk vector image object to a vtk renderer. The
 * visualization maps the magnitude of the input vector field to height and
 * color. This class provides the ability to select which component(s) of the
 * vector to use in the vector magnitude computation. Also, scaling and coloring
 * of the height map is adjustable through methods on this class.
 */
class VectorHeightMapPipeline :
    public ItkVtkPipeline
{
public:
    // Common itk typedefs
    typedef VectorHeightMapPipeline Self;
    typedef ItkVtkPipeline Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    
    itkNewMacro(Self);
    itkTypeMacro(VectorHeightMapPipeline, ItkVtkPipeline);
    
    typedef ImageTypeV2F2 InputImageType;
    typedef itk::VTKImageExport< InputImageType > ExportType;
    
    // Magnitude function enumeration
    enum Function
    {
        VectorMagnitude,
        XComponent,
        YComponent
    };
    
    virtual void SetInput(itk::DataObject* input);
    virtual void Update();
    
    virtual wxWindow* CreateWxControl(wxWindow* parent);
    
    void SetScaleFactor(double scale);
    double GetScaleFactor();
    
    void SetColorMapType(ColorMap map);
    ColorMap GetColorMapType();
    
    void SetMagnitudeFunction(Function function);
    Function GetMagnitudeFunction();
    
protected:
    VectorHeightMapPipeline();
    virtual ~VectorHeightMapPipeline();
    
private:
    static const std::string VECTOR_MAGNITUDE;
    static const std::string X_COMPONENT;
    static const std::string Y_COMPONENT;
    
    ExportType::Pointer exporter;
    vtkImageImport* importer;
    vtkImageFlip* flip;
    vtkDataSetSurfaceFilter* surface;
    vtkArrayCalculator* calculator;
    vtkWarpScalar* warp;
    vtkPolyDataNormals* normals;
    vtkPolyDataMapper* mapper;
    vtkActor* actor;
    
    Function function;
    ColorMap colorMapType;
};
