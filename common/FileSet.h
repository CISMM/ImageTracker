#pragma once

#include <string>
#include <vector>
#include "FilePattern.h"

/**
 * Represents a group of files.
 */
class FileSet
{
public:
    /*
     * A vector of file names.
     */
    typedef std::vector<std::string> FileVector;

    /*
     * Default constructor.
     */ 
    FileSet() 
    {
    }

    /* 
     * Create a FileSet based on a file pattern.
     */
    FileSet(const FilePattern& pattern);

    /*
     * Create a FileSet given a vector of file names.
     */
    FileSet(const FileVector& names);

    /*
     * Create a new FileSet by prepending a prefix to the front
     * of all file names.
     */
    FileSet(const FileSet& files, const std::string& prefix);

    /*
     * Create a new FileSet by prepending a prefix to the front
     * of all file names and changing the extension.
     */
    FileSet(const FileSet& files, const std::string& prefix, const std::string& ext);

    /*
     * Destructor.
     */
    ~FileSet(void) 
    {}

    /*
     * Set the list of file names for this FileSet. (Overwrites previous list.)
     */
    void SetFileNames(const FileVector& names);

    /*
     * Returns the list of file names in this FileSet as a vector of strings.
     */
    const FileVector& GetFileNames(void) const;

    /*
     * Get the directory for this FileSet.
     */
    std::string GetDirectory(void) const;

    /*
     * Sets the directory for all file names in the FileSet.
     */
    void SetDirectory(const std::string& dir);

    /*
     * Returns the number of files in the FileSet.
     */
    unsigned int size() const
    {
        return this->fileNames.size();
    }

    const std::string operator[](unsigned int index)
    {
        return this->fileNames[index];
    }

private:
    std::string::size_type DirectoryIndex(const std::string& filename) const;
    std::string DirectoryPart(const std::string& filename) const;
    std::string FilePart(const std::string& filename) const;

    FileVector fileNames;
};
