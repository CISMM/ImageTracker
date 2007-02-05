#include "GuiUtils.h"

#include "ScalarImageItkVtkPipeline.h"
#include "Logger.h"

ItkVtkPipeline::Pointer GenerateVisualPipeline(DataSource::Pointer source)
{
    ItkVtkPipeline::Pointer view;
    switch (source->GetPixelDataType())
    {
        case ScalarChar:
        case ScalarShort:
        case ScalarFloat:
            Logger::verbose << "ImageTrackerController::GenerateViewPipeline: making ScalarImageItkVtkPipeline" << std::endl;
            view = ScalarImageItkVtkPipeline::New();
            break;
        case VectorFloat:
            Logger::warning << "ImageTrackerController::GenerateViewPipeline: VectorFloat not yet recognized." << std::endl;
            break;
        default:
            Logger::warning << "ImageTrackerController::GenerateViewPipeline: DataSource not recognized!" << std::endl;
            break;
    }
    
    return view;
}
