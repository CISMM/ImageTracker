#pragma once

#include <cassert>
#include <iostream>
#include <string>
//#include "FileSet.h"

class FilePattern
{
public:
    FilePattern(std::string dir, std::string pre, std::string ext, unsigned int start, unsigned int end, unsigned int places);
    FilePattern(void);
    ~FilePattern(void);
    bool Validate(void);

    std::string directory;
    std::string prefix;
    std::string extension;
    unsigned int start;
    unsigned int end;
    unsigned int places;
};
