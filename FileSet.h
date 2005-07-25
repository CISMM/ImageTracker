#pragma once

#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include "FilePattern.h"

/*
 * Represents a group of files.  The file group consists of
 * a directory and a list of file names in that directory.
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
    FileSet(void);

    /* 
     * Create a FileSet based on a file pattern.
     */
    FileSet(FilePattern * pattern);

    /*
     * Create a FileSet given a vector of file names.
     */
    FileSet(FileVector * names);

    /*
     * Create a new FileSet by prepending a prefix to the front
     * of all file names.
     */
    FileSet(FileSet * files, std::string prefix);

    /*
     * Create a new FileSet by prepending a prefix to the front
     * of all file names and changing the extension.
     */
    FileSet(FileSet* files, std::string prefix, std::string ext);

    /*
     * Destructor.
     */
    ~FileSet(void);

    /*
     * Set the list of file names for this FileSet. (Overwrites previous list.)
     */
    void SetFileNames(FileVector * names);

    /*
     * Returns the list of file names in this FileSet.
     */
    FileVector * GetFileNames(void);

    /*
     * Set the directory associated with this FileSet.
     */ 
    void SetDirectory(std::string dir);

    /*
     * Get the directory for this FileSet.
     */
    std::string GetDirectory(void);

    /*
     * Return the full file name (directory + file name) for the file
     * indicated by the given vector iterator.
     */
    std::string FullFileName(FileIterator it);
private:
    /*
     * Prepend a given integer with zeros to the given number of places.
     */
    std::string PadInt(int anInt, unsigned int places);

    FileVector * fileNames;
    std::string directory;
};
