#include "FileSet.h"

FileSet::FileSet(const FilePattern& pattern)
    : fileNames()
{
    pattern.Validate();

    for (int i = pattern.start; i <= (int) pattern.end; i++)
    {
        char cname[512];
        std::string format = pattern.directory + "/" + pattern.format;
        sprintf(cname, format.c_str(), i);
        this->fileNames.push_back(std::string(cname));
    }
}

FileSet::FileSet(const FileVector& names)
    : fileNames(names)
{
}

FileSet::FileSet(const FileSet& files, const std::string& prefix)
    : fileNames()
{
    for (FileVector::const_iterator fit = files.fileNames.begin(); fit != files.fileNames.end(); ++fit)
    {
        std::string filename = *fit;
        std::string newname = 
            this->DirectoryPart(filename) + 
            prefix + 
            this->FilePart(filename);
        this->fileNames.push_back(newname);
    }
}

FileSet::FileSet(const FileSet& files, const std::string& prefix, const std::string& ext)
    : fileNames()
{
    for (FileVector::const_iterator fit = files.fileNames.begin(); fit != files.fileNames.end(); fit++)
    {
        std::string filename = *fit;
        std::string newname = 
            this->DirectoryPart(filename) + 
            prefix + 
            this->FilePart(filename);

        std::string::size_type idx = newname.find_last_of(".");
        this->fileNames.push_back(newname.substr(0, idx+1) + ext);
    }
}

void FileSet::SetFileNames(const FileVector& names)
{
    this->fileNames = names;
}

const FileSet::FileVector& FileSet::GetFileNames() const
{
    return this->fileNames;
}

std::string FileSet::GetDirectory() const
{
    std::string dir("");

    if (!this->fileNames.empty())
    {
        dir = this->DirectoryPart(this->fileNames[0]);
    }

    return dir;
}

void FileSet::SetDirectory(const std::string& dir)
{
    for (std::string::size_type i = 0; i < this->fileNames.size(); i++)
    {
        this->fileNames[i] = dir + this->FilePart(this->fileNames[i]);
    }
}

std::string::size_type FileSet::DirectoryIndex(const std::string& filename) const
{
    // find the index of the last "\"
    return filename.rfind("/");
}

std::string FileSet::DirectoryPart(const std::string& filename) const
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

std::string FileSet::FilePart(const std::string& filename) const
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
