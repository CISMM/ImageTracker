#pragma once

#include <string>

#include "itkImageFileReader.h"

#include "CommonTypes.h"
#include "FileSet.h"

/**
 * Provides an easy way to read all files in a FileSet as
 * ITK Images.  itk::ImageFileReaders cannot be reused in 
 * certain situations; specifically, one cannot expect the
 * output from the same ImageFileReader taken at different
 * times (i.e. with different file names) to be different.
 * One must use different ImageFileReaders when performing
 * registration.  This class provides that functionality.
 */
class FileSetImageReader
{
public:
    typedef CommonTypes::ImageType ImageType;
    typedef itk::ImageFileReader<ImageType> ReaderType;

    FileSetImageReader(void);
    FileSetImageReader(FileSet* fileSet);
    ~FileSetImageReader(void);

    void SetFileSet(FileSet* fileSet);
    FileSet* GetFileSet(void);

    /**
     * Determines whether there are more files left to be
     * read.
     */
    bool HasNext(void);

    /**
     * Returns the name of the current image file.
     */
    std::string CurrentFileName(void);

    /**
     * Returns the name of the next image file, *and* advances
     * this FileSet so that file is the current file.
     */
    std::string NextFileName(void);

    /**
     * Returns the current itk::Image.
     */
    ImageType::Pointer CurrentImage(void);

    /**
     * Returns the next itk::Image, *and* advances this
     * FileSet so that image is the current image.
     */
    ImageType::Pointer NextImage(void);

private:
    FileSet* fileSet;
    FileSet::FileIterator fileIt;

    /**
     * Advance the FileSet iterator.
     */
    void Next(void);
};
