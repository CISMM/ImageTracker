#include "VectorFileSetReader.h"

#include <algorithm>
#include <string>

#include "Logger.h"

VectorFileSetReader::VectorFileSetReader()
    : files(), index(0)
{
    this->reader = ReaderType::New();
}

VectorFileSetReader::~VectorFileSetReader()
{
}

void VectorFileSetReader::SetFileSet(const FileSet& files)
{
    this->files = files;
    
    // prime the reader
    this->SetImageIndex(this->index);
}

FileSet& VectorFileSetReader::GetFileSet()
{
    return this->files;
}

void VectorFileSetReader::SetImageIndex(unsigned int idx)
{
//     std::string function("VectorFileSetReader::SetImageIndex");
//     Logger::verbose << function << ": " << idx << std::endl;
    
    // ensure index is between 0 and size - 1.
    this->index = std::min((int) idx, (int) this->files.size()-1);
    this->index = std::max((int) this->index, 0);
    
    if (this->files.size() > 0)
    {
        this->reader->SetFileName(this->files[this->index].c_str());
    }
}

unsigned int VectorFileSetReader::GetImageIndex()
{
    return this->index;
}

int VectorFileSetReader::GetImageCount()
{
    return this->files.size();
}

VectorFileSet::ImageType* VectorFileSetReader::GetOutput()
{
//     std::string function("VectorFileSetReader::GetOutput");
//     Logger::verbose << function << ": " << this->index << std::endl;

    return this->reader->GetOutput();
}

VectorFileSet::ImageType* VectorFileSetReader::GetImage(unsigned int idx)
{
    this->SetImageIndex(idx);
    return this->GetOutput();
}
