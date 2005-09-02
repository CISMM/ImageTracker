#pragma once
#include <string>

#include "itkCastImageFilter.h"
#include "itkImage.h"
#include "itkImportImageFilter.h"
#include "itkLightObject.h"
#include "../CommonTypes.h"

class ItkMagickIO : public itk::LightObject
{
public:
    // Standard ITK defs
    typedef ItkMagickIO Self;
    typedef itk::LightObject Superclass;
    typedef itk::SmartPointer<Self> Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;

    itkNewMacro(Self);

    typedef CommonTypes::InternalImageType InternalImageType;
    typedef unsigned short MagickPixelType; // for 16 bit image data
    enum {Dimension = CommonTypes::Dimension};
    typedef itk::Image<MagickPixelType, Dimension> MagickImageType;
    typedef itk::ImportImageFilter<MagickPixelType, Dimension> ImportType;
    typedef itk::CastImageFilter<MagickImageType, InternalImageType> CasterType;

    /*
     * Read image data from a file using ImageMagick (which has good
     * tiff readers, and support for 16 bit data, for example) and 
     * immediately transfer that data to an itk::Image object.
     */
    InternalImageType::Pointer Read(std::string filename);

    /*
     * Write an image to a file using ImageMagick.  Not sure we actually
     * need this capability, since ITK's image writers seem to work
     * all right on data in itk's memory, so this method is not
     * implemented.
     */
    void Write(std::string filename, InternalImageType::Pointer itkImage);

protected:
    ItkMagickIO() 
    {
        this->importer = NULL;
        this->caster = NULL;
    }
    virtual ~ItkMagickIO() {}

private:
    ItkMagickIO(const Self&); // no copy constructor
    void operator=(const Self&); // no assignment operator

    static bool initialized;
    static void Initialize(const char* magickDir);

    ImportType::Pointer importer;
    CasterType::Pointer caster;
};
