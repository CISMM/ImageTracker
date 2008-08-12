#include "ItkVectorPipeline.h"

ItkVectorPipeline::ItkVectorPipeline()
{
    this->input = NULL;
}

ItkVectorPipeline::~ItkVectorPipeline()
{
    this->input = NULL;
}

void ItkVectorPipeline::SetInput(VectorFileSet* input)
{
    this->input = input;
}

VectorFileSet* ItkVectorPipeline::GetInput()
{
    return this->input;
}
