#include "CompositeImageFileSet.h"

CompositeImageFileSet::CompositeImageFileSet()
{
    this->start = NULL;
    this->end = NULL;
}

CompositeImageFileSet::~CompositeImageFileSet()
{
    // release pointers, but do not delete--they are
    // controlled by other classes
    this->start = NULL;
    this->end = NULL;
}

void CompositeImageFileSet::SetStart(ImageFileSet* start)
{
    this->start = start;
}

void CompositeImageFileSet::SetEnd(FilterControlPanel* end)
{
    this->end = end;
}

void CompositeImageFileSet::SetImageIndex(unsigned int index)
{
    if (this->start)
        this->start->SetImageIndex(index);
}

unsigned int CompositeImageFileSet::GetImageIndex()
{
    if (this->start)
        return this->start->GetImageIndex();
    else
        return 0;
}

int CompositeImageFileSet::GetImageCount()
{
    if (this->start)
        return this->start->GetImageCount();
    else
        return 0;
}

ImageFileSet::ImageType* CompositeImageFileSet::GetOutput()
{
    if (this->end)
        return this->end->GetOutput();
    else
        return NULL;
}

ImageFileSet::ImageType* CompositeImageFileSet::GetImage(unsigned int index)
{
    this->SetImageIndex(index);
    return this->GetOutput();
}
