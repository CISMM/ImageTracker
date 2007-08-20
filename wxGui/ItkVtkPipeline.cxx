#include "ItkVtkPipeline.h"
#include "vtkCollectionIterator.h"

ItkVtkPipeline::ItkVtkPipeline()
    : props()
{
}

ItkVtkPipeline::~ItkVtkPipeline()
{
}

ItkVtkPipeline::PropCollectionType& ItkVtkPipeline::GetProps()
{
    return this->props;
}

void ItkVtkPipeline::AddPropsTo(vtkRenderer* renderer)
{
    for (PropCollectionType::iterator it = this->GetProps().begin();
         it != this->GetProps().end(); 
         ++it)
    {
        renderer->AddActor(*it);
    }
}

void ItkVtkPipeline::RemovePropsFrom(vtkRenderer* renderer)
{
    for (PropCollectionType::iterator it = this->GetProps().begin();
         it != this->GetProps().end(); 
         ++it)
    {
        renderer->RemoveActor(*it);
    }
}
