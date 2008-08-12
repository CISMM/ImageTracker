#include "ImageFileSetReader.h"

#include <algorithm>
#include <string>

#include "Logger.h"

ImageFileSetReader::ImageFileSetReader()
    : pixelType(ShortPixel), files(), index(0)
{
    this->readerUC2 = ReaderTypeUC2::New();
    this->readerUS2 = ReaderTypeUS2::New();
    this->readerF2 = ReaderTypeF2::New();
    
    this->casterUC2 = CasterTypeUC2::New();
    this->casterUS2 = CasterTypeUS2::New();
    this->casterF2 = CasterTypeF2::New();
    
    this->casterUC2->SetInput(this->readerUC2->GetOutput());
    this->casterUS2->SetInput(this->readerUS2->GetOutput());
    this->casterF2->SetInput(this->readerF2->GetOutput());
}

ImageFileSetReader::~ImageFileSetReader()
{
}

void ImageFileSetReader::SetFileSet(const FileSet& files)
{
    this->files = files;
    
    // prime the current reader
    this->SetImageIndex(this->index);
}

FileSet& ImageFileSetReader::GetFileSet()
{
    return this->files;
}

void ImageFileSetReader::SetPixelType(ScalarPixelType type)
{
    this->pixelType = type;
    
    // prime the reader
    this->SetImageIndex(this->index);
}

void ImageFileSetReader::SetImageIndex(unsigned int idx)
{
//     std::string function("ImageFileSetReader::SetImageIndex");
//     Logger::verbose << function << ": " << idx << std::endl;
    
    // ensure index is between 0 and size - 1.
    this->index = std::min((int) idx, (int) this->files.size()-1);
    this->index = std::max((int) this->index, 0);
    
    if (this->files.size() > 0)
    {
        if (this->pixelType == CharPixel)
            this->readerUC2->SetFileName(this->files[this->index].c_str());
        else if (this->pixelType == ShortPixel)
            this->readerUS2->SetFileName(this->files[this->index].c_str());
        else
            this->readerF2->SetFileName(this->files[this->index].c_str());
    }
}

unsigned int ImageFileSetReader::GetImageIndex()
{
    return this->index;
}

int ImageFileSetReader::GetImageCount()
{
    return this->files.size();
}

ImageFileSet::ImageType* ImageFileSetReader::GetOutput()
{
//     std::string function("ImageFileSetReader::GetOutput");
//     Logger::verbose << function << ": " << this->index << std::endl;

    if (this->pixelType == CharPixel)
        return this->casterUC2->GetOutput();
    else if (this->pixelType == ShortPixel)
        return this->casterUS2->GetOutput();
    else
        return this->casterF2->GetOutput();
}

ImageFileSet::ImageType* ImageFileSetReader::GetImage(unsigned int idx)
{
    this->SetImageIndex(idx);
    return this->GetOutput();
}
