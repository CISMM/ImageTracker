#include "FileSetImageReader.h"

FileSetImageReader::FileSetImageReader(void)
{
}

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
    this->fileIt = this->fileSet->GetFileNames()->begin();
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
    return this->fileSet->FullFileName(this->fileIt);
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

FileSetImageReader::ImageType::Pointer FileSetImageReader::CurrentImage()
{
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(this->CurrentFileName().c_str());
    reader->Update();
    return reader->GetOutput();
}

FileSetImageReader::ImageType::Pointer FileSetImageReader::NextImage()
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