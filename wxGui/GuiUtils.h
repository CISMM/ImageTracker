#pragma once

#include "DataSource.h"
#include "ItkVtkPipeline.h"

ItkVtkPipeline::Pointer GenerateVisualPipeline(DataSource::Pointer source);
