#include "FileSetImageReader.h"
#include "itkStatisticsImageFilter.h"

#include "Logger.h"
#include "ImageUtils.h"

FileSetImageReader::FileSetImageReader(FileSet* fileSet)
{
    this->SetFileSet(fileSet);
}

FileSetImageReader::~FileSetImageReader(void)
{
}

void FileSetImageReader::SetFileSet(FileSet* fileSet)
{
    this->fileSet = fileSet;
    
    if (this->fileSet)
    {
        this->fileIt = this->fileSet->GetFileNames().begin();
    }
}

FileSet* FileSetImageReader::GetFileSet()
{
    return this->fileSet;
}

bool FileSetImageReader::HasNext()
{
    bool next = this->fileSet != NULL;

    if (next)
    {
        ++this->fileIt;
        next = this->fileIt != this->fileSet->GetFileNames().end();
        --this->fileIt;
    }

    return next;
}

std::string FileSetImageReader::CurrentFileName()
{
    return *(this->fileIt);
}

std::string FileSetImageReader::NextFileName()
{
    if (this->HasNext())
    {
        this->Next();
        return this->CurrentFileName();
    }

    // Else
    return NULL;
}

FileSetImageReader::InternalImageType::Pointer FileSetImageReader::FirstImage()
{
    this->fileIt = this->fileSet->GetFileNames().begin();
    return this->CurrentImage();
}

FileSetImageReader::InternalImageType::Pointer FileSetImageReader::CurrentImage()
{
    return ReadImage<InternalImageType>(this->CurrentFileName());
}

FileSetImageReader::InternalImageType::Pointer FileSetImageReader::NextImage()
{
    if (this->HasNext())
    {
        this->Next();
        return this->CurrentImage();
    }

    // else
    return NULL;
}

void FileSetImageReader::Next()
{
    *(this->fileIt)++;
}

void FileSetImageReader::GetMinMax(InternalImageType::PixelType &min, InternalImageType::PixelType &max)
{
    typedef itk::StatisticsImageFilter<InternalImageType> StatsType;
    StatsType::Pointer stats = StatsType::New();
    stats->SetInput(this->CurrentImage());
    stats->Update();
    min = stats->GetMinimum();
    max = stats->GetMaximum();
}
