#pragma once

#include "CommonTypes.h"
#include "FileSet.h"

class DemonsPipeline
{
public:
    typedef CommonTypes::ImageType ImageType;

    DemonsPipeline(void);
    ~DemonsPipeline(void);
    void Update(void);
    void SetInputFiles(FileSet* files);
    void SetOutputFiles(FileSet* files);
private:
    FileSet* inFiles;
    FileSet* outFiles;
};
