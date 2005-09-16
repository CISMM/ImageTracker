#include "FileSetImageReader.h"
#include "itkStatisticsImageFilter.h"

#include "Logger.h"

FileSetImageReader::FileSetImageReader(FileSet* fileSet)
{
    this->reader = ItkMagickIO::New();
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
        this->fileIt = this->fileSet->GetFileNames()->begin();
    }
}

FileSet* FileSetImageReader::GetFileSet()
{
    return this->fileSet;
}

bool FileSetImageReader::HasNext()
{
    bool next = 
        this->fileSet != NULL &&
        this->fileIt != NULL;

    if (next)
    {
        *(this->fileIt)++;
        next = this->fileIt != this->fileSet->GetFileNames()->end();
        *(this->fileIt)--;
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
    this->fileIt = this->fileSet->GetFileNames()->begin();
    return this->CurrentImage();
}

FileSetImageReader::InternalImageType::Pointer FileSetImageReader::CurrentImage()
{
    return this->reader->Read(this->CurrentFileName());

    // typedef itk::StatisticsImageFilter<InternalImageType> StatsType;
    // Compute and print max/min for image
    //StatsType::Pointer stats = StatsType::New();
    //stats->SetInput(this->reader->Read(this->CurrentFileName()));
    //stats->Update();
    //char text[80];
    //sprintf(text, "Min: %5.2f", stats->GetMinimum());
    //Logger::logDebug(text);
    //sprintf(text, "Max: %5.2f", stats->GetMaximum());
    //Logger::logDebug(text);
    //return stats->GetOutput();
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