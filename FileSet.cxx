#include "FileSet.h"

FileSet::FileSet(void)
{
}

FileSet::FileSet(FilePattern* pattern)
{
    pattern->Validate();

    this->directory = pattern->directory;

    int count = pattern->end - pattern->start + 1;
    std::vector<std::string>* pNames = new std::vector<std::string>();

    for (int i = 0; i < count; i++)
    {
        std::string name = 
            pattern->prefix + 
            this->PadInt(pattern->start+i, pattern->places) + 
            pattern->extension;
        pNames->push_back(name);
    }

    this->SetFileNames(pNames);
}

FileSet::~FileSet(void)
{
}

void FileSet::SetFileNames(std::vector<std::string>* names)
{
    this->fileNames = names;
}

void FileSet::SetDirectory(std::string dir)
{
    this->directory = dir;
}

std::vector<std::string>* FileSet::GetFileNames()
{
    return this->fileNames;
}

std::string FileSet::GetDirectory()
{
    return this->directory;
}

std::string FileSet::FullFileName(FileIterator it)
{
    return this->GetDirectory() + *it;
}

std::string FileSet::PadInt(int anInt, unsigned int places)
{
    std::string intStr = itoa(anInt, "", 10);
    while (intStr.size() < places)
    {
        intStr.insert(intStr.begin(), '0');
    }

    return intStr;
}