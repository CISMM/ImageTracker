#pragma once

#include <string>

#include "itkCastImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageRegionConstIterator.h"

#include "CommonTypes.h"
#include "FileSet.h"

/*
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
    typedef CommonTypes::InputImageType InputImageType;
    typedef CommonTypes::InternalImageType InternalImageType;
    typedef itk::ImageFileReader<InputImageType> ReaderType;
    typedef itk::CastImageFilter<InputImageType, InternalImageType> CasterType;
    typedef itk::ImageRegionConstIterator<InternalImageType> IteratorType;

    FileSetImageReader(FileSet* fileSet = (FileSet *) NULL);
    ~FileSetImageReader(void);

    void SetFileSet(FileSet* fileSet);
    FileSet* GetFileSet(void);

    /*
     * Determines whether there are more files left to be
     * read.
     */
    bool HasNext(void);

    /*
     * Returns the name of the current image file.
     */
    std::string CurrentFileName(void);

    /*
     * Returns the name of the next image file, *and* advances
     * this FileSet so that file is the current file.
     */
    std::string NextFileName(void);

    /*
     * Returns the first image in the associated file set.  Resets
     * the FileSet so the first image is the current image.
     */
    InternalImageType::Pointer FirstImage(void);

    /*
     * Returns the current itk::Image.
     */
    InternalImageType::Pointer CurrentImage(void);

    /*
     * Returns the next itk::Image, *and* advances this
     * FileSet so that image is the current image.
     */
    InternalImageType::Pointer NextImage(void);

    /************************************************************************/
    /* Gets the minimum and maximum pixel value for this reader's current image.
    /************************************************************************/
    void GetMinMax(InternalImageType::PixelType &min, InternalImageType::PixelType &max);

private:
    FileSet* fileSet;
    FileSet::FileVector::const_iterator fileIt;

    /**
     * Advance the FileSet iterator.
     */
    void Next(void);
};
