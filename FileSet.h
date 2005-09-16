#pragma once

#include <string>
#include <vector>
#include "FilePattern.h"

/*
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
     * A FileVector iterator.
     */
    typedef std::vector<std::string>::iterator FileIterator;

    /*
     * Default constructor.
     */ 
    FileSet(void) 
    {
        this->fileNames = new FileVector();
    }

    /* 
     * Create a FileSet based on a file pattern.
     */
    FileSet(const FilePattern* pattern);

    /*
     * Create a FileSet given a vector of file names.
     */
    FileSet(FileVector * names);

    /*
     * Create a new FileSet by prepending a prefix to the front
     * of all file names.
     */
    FileSet(const FileSet* files, const std::string prefix);

    /*
     * Create a new FileSet by prepending a prefix to the front
     * of all file names and changing the extension.
     */
    FileSet(const FileSet* files, const std::string prefix, const std::string ext);

    /*
     * Destructor.
     */
    ~FileSet(void) 
    {}

    /*
     * Set the list of file names for this FileSet. (Overwrites previous list.)
     */
    void SetFileNames(FileVector* names);

    /*
     * Returns the list of file names in this FileSet.
     */
    FileVector* GetFileNames(void);

    /*
     * Get the directory for this FileSet.
     */
    std::string GetDirectory(void);

    /*
     * Sets the directory for all file names in the FileSet.
     */
    void SetDirectory(std::string dir);

private:
    /*
     * Prepend a given integer with zeros to the given number of places.
     */
    std::string PadInt(int anInt, unsigned int places);
    std::string::size_type DirectoryIndex(const std::string filename);
    std::string DirectoryPart(const std::string filename);
    std::string FilePart(const std::string filename);

    FileVector* fileNames;
};
