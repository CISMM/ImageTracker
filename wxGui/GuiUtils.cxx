#include "GuiUtils.h"

#include "ScalarImageItkVtkPipeline.h"
#include "VectorGlyphPipeline.h"
#include "VectorHeightMapPipeline.h"
#include "Logger.h"

ItkVtkPipeline::Pointer GenerateVisualPipeline(DataSource::Pointer source)
{
    std::string function("GenerateVisualPipeline");
    ItkVtkPipeline::Pointer view;
    switch (source->GetPixelDataType())
    {
        case ScalarChar:
        case ScalarShort:
        case ScalarFloat:
            Logger::verbose << function << ": making ScalarImageItkVtkPipeline" << std::endl;
            view = ScalarImageItkVtkPipeline::New();
            break;
        case VectorFloat:
            // Logger::verbose << function << ": making VectorGlyphPipeline" << std::endl;
            // view = VectorGlyphPipeline::New();
            Logger::verbose << function << ": making VectorHeightMapPipeline" << std::endl;
            view = VectorHeightMapPipeline::New();
            break;
        default:
            Logger::warning << "ImageTrackerController::GenerateViewPipeline: DataSource not recognized!" << std::endl;
            break;
    }
    
    return view;
}
