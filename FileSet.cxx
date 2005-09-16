#include "FileSet.h"

FileSet::FileSet(const FilePattern* pattern)
{
    pattern->Validate();

    int count = pattern->end - pattern->start + 1;
    this->fileNames = new FileVector();

    for (int i = pattern->start; i <= (int) pattern->end; i++)
    {
        std::string name(pattern->directory +
            pattern->prefix + 
            this->PadInt(i, pattern->places) + 
            pattern->extension);
        this->fileNames->push_back(name);
    }
}

FileSet::FileSet(FileVector* names)
{
    this->SetFileNames(names);
}

FileSet::FileSet(const FileSet* files, const std::string prefix)
{
    this->fileNames = new FileVector();
    
    std::string::size_type idx = std::string::npos;

    for (FileSet::FileVector::iterator fit = files->fileNames->begin(); fit != files->fileNames->end(); fit++)
    {
        std::string filename = *fit;
        this->fileNames->push_back(
            this->DirectoryPart(filename) + 
            prefix + 
            this->FilePart(filename));
    }
}

FileSet::FileSet(const FileSet* files, const std::string prefix, const std::string ext)
{
    this->fileNames = new FileSet::FileVector();

    for (FileSet::FileVector::iterator fit = files->fileNames->begin(); fit != files->fileNames->end(); fit++)
    {
        std::string filename = *fit;
        std::string newName = 
            this->DirectoryPart(filename) + 
            prefix + 
            this->FilePart(filename);

        std::string::size_type idx = newName.find_last_of(".");
        this->fileNames->push_back(newName.substr(0, idx+1) + ext);
    }
}

void FileSet::SetFileNames(FileVector * names)
{
    this->fileNames = names;
}

std::vector<std::string>* FileSet::GetFileNames()
{
    return this->fileNames;
}

std::string FileSet::GetDirectory()
{
    std::string dir("");
    
    if (this->fileNames != NULL && !this->fileNames->empty())
    {
        dir = this->DirectoryPart((*this->fileNames)[0]);
    }

    return dir;
}

void FileSet::SetDirectory(std::string dir)
{
    for (std::string::size_type i = 0; i < this->fileNames->size(); i++)
    {
        (*this->fileNames)[i] = dir + this->FilePart((*this->fileNames)[i]);
    }
}

std::string FileSet::PadInt(int anInt, unsigned int places)
{
    char buffer[40];
    std::string intStr(itoa(anInt, buffer, 10));
    while (intStr.size() < places)
    {
        intStr.insert(intStr.begin(), '0');
    }

    return intStr;
}

std::string::size_type FileSet::DirectoryIndex(const std::string filename)
{
    // find the index of the last "\"
    return filename.rfind("\\");
}

std::string FileSet::DirectoryPart(const std::string filename)
{
    std::string dir("");
    std::string::size_type idx = DirectoryIndex(filename);
    if (idx != std::string::npos)
    {
        // the directory part from the start 
        // up to and including the last "/"
        dir = filename.substr(0, idx+1);
    }

    return dir;
}

std::string FileSet::FilePart(const std::string filename)
{
    std::string file("");
    std::string::size_type idx = DirectoryIndex(filename);
    if (idx != std::string::npos)
    {
        // the file part is everything after the last "/"
        file = filename.substr(idx+1);
    }

    return file;
}