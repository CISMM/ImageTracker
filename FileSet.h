#pragma once

#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include "FilePattern.h"

class FileSet
{
public:
    typedef std::vector<std::string> FileVector;
    typedef std::vector<std::string>::iterator FileIterator;

    FileSet(void);
    FileSet(FilePattern* pattern);
    ~FileSet(void);

    void SetFileNames(std::vector<std::string>* names);
    std::vector<std::string>* GetFileNames(void);
    void SetDirectory(std::string dir);
    std::string GetDirectory(void);
    std::string FullFileName(FileIterator it);
private:
    std::string PadInt(int anInt, unsigned int places);

    std::vector<std::string>* fileNames;
    std::string directory;
};
