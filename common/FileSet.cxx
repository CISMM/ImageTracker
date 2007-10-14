#include "FileSet.h"
#include "FileUtils.h"

FileSet::FileSet(const FilePattern& pattern)
    : fileNames()
{
    pattern.Validate();
    
    std::string directory = pattern.directory;
    CapDirectory(directory);

    std::string format(directory + pattern.format);
    for (int i = pattern.start; i <= (int) pattern.end; i++)
    {
        char cname[512];
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
            DirectoryPart(filename) + 
            prefix + 
            FilePart(filename);
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
            DirectoryPart(filename) + 
            prefix + 
            FilePart(filename);

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
        dir = DirectoryPart(this->fileNames[0]);
    }

    return dir;
}

void FileSet::SetDirectory(const std::string& dir)
{
    for (std::string::size_type i = 0; i < this->fileNames.size(); i++)
    {
        this->fileNames[i] = dir + FilePart(this->fileNames[i]);
    }
}

