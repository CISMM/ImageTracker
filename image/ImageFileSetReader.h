#pragma once

#include "itkLightObject.h"

#include "FileSet.h"
#include "ImageFileSet.h"
#include "ImageFileSetTypes.h"

/**
 * \class ImageFileSetReader
 * \brief Implementation of ImageFileSet that reads files from disk.
 * 
 * This ImageFileSet reads image files directly from disk.  This class
 * handles several input image pixel types by maintaining loading and
 * casting pipelines for each type.  Setting the PixelType for this
 * ImageFileReader switches among these pipelines.  The output image
 * is always ImageFileSet::ImageType.
 */
class ImageFileSetReader
    : public itk::LightObject, 
      public ImageFileSet
{
public:
    typedef ImageFileSetReader Self;
    typedef itk::LightObject Superclass;
    typedef itk::SmartPointer<Self> Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;
    itkNewMacro(Self);
    
    /**
     * Set the file names that this ImageFileSet should read
     * if it reads from disk.
     */
    virtual void SetFileSet(const FileSet& files);
    
    virtual FileSet& GetFileSet();
    
    /**
     * Set the type of input data that this ImageFileSet reads.  This
     * may be ignored in some implementations.
     */
    virtual void SetPixelType(ScalarPixelType type);
    
    // ImageFileSet implementation
    virtual void SetImageIndex(unsigned int idx);
    virtual unsigned int GetImageIndex();
    virtual int GetImageCount();
    virtual ImageFileSet::ImageType* GetOutput();
    virtual ImageFileSet::ImageType* GetImage(unsigned int index);
    
protected:
    ImageFileSetReader();
    virtual ~ImageFileSetReader();
    
private:
    // not implemented
    ImageFileSetReader(const Self& other);
    void operator=(const Self& other);
    
    ReaderTypeUC2::Pointer readerUC2;
    ReaderTypeUS2::Pointer readerUS2;
    ReaderTypeF2::Pointer readerF2;
    
    CasterTypeUC2::Pointer casterUC2;
    CasterTypeUS2::Pointer casterUS2;
    CasterTypeF2::Pointer casterF2;
    
    ScalarPixelType pixelType;
    FileSet files;
    unsigned int index;
};
