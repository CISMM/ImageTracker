#include "ItkImagePipeline.h"

ItkImagePipeline::ItkImagePipeline()
{
    this->input = NULL;
}

ItkImagePipeline::~ItkImagePipeline()
{
    this->input = NULL;
}

void ItkImagePipeline::SetInput(ImageFileSet* input)
{
    this->input = input;
}

ImageFileSet* ItkImagePipeline::GetInput()
{
    return this->input;
}
