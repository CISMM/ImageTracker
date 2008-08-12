#pragma once

#include "itkLightObject.h"
#include "itkImageFileReader.h"

#include "FileSet.h"
#include "VectorFileSet.h"

/**
 * \class VectorFileSetReader
 * \brief Implementation of VectorFileSet that reads files from disk.
 * 
 * This VectorFileSet reads image files directly from disk.
 * The output image is always VectorFileSet::VectorImageType.
 */
class VectorFileSetReader 
    : public itk::LightObject, 
      public VectorFileSet
{
public:
    typedef VectorFileSetReader Self;
    typedef itk::LightObject Superclass;
    typedef itk::SmartPointer<Self> Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;
    itkNewMacro(Self);
    
    typedef itk::ImageFileReader<VectorFileSet::ImageType> ReaderType;

    /**
     * Set the file names that this VectorFileSet should read
     * if it reads from disk.
     */
    virtual void SetFileSet(const FileSet& files);
    
    virtual FileSet& GetFileSet();

    // VectorFileSet implementation
    virtual void SetImageIndex(unsigned int idx);
    virtual unsigned int GetImageIndex();
    virtual int GetImageCount();
    virtual VectorFileSet::ImageType* GetOutput();
    virtual VectorFileSet::ImageType* GetImage(unsigned int index);

protected:
    VectorFileSetReader();
    virtual ~VectorFileSetReader();

private:
    // not implemented
    VectorFileSetReader(const Self& other);
    void operator=(const Self& other);    

    ReaderType::Pointer reader;
    FileSet files;
    unsigned int index;
};
