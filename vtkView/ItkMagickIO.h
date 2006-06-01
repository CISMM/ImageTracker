#pragma once
#include <string>

#include "itkCastImageFilter.h"
#include "itkImage.h"
#include "itkImportImageFilter.h"
#include "itkLightObject.h"
#include "itkStatisticsImageFilter.h"
#include "CommonTypes.h"

/*
 * ItkMagickIO provides ITK image reading and writing through 
 * ImageMagick.  Although ITK provides many image IO factories,
 * their tiff readers and writers don't really work all that well.
 * Since ImageMagick works on most if not all the microscopy data
 * produced by the NSRG, we've sort of standardized on reading
 * images with ImageMagick.  This class provides the link between
 * ImageMagick and ITK pipelines.
 *
 * Note: this class is deprecated! In the not too distant past ITK had
 * image file readers that were not able to read as many image files
 * (notably tiffs) as ImageMagick without crashing. This has been
 * remedied with ITK now supporting the latest tiff libraries. Transferring
 * data through ImageMagick was enough of a pain that we're phasing
 * this link out, and relying solely on ITK.
 *
 * Although the logic behind loading images through this class has been changed
 * to use ITK, use of this class is discouraged. At a minimum, its name no
 * longer makes sense.
 */
class ItkMagickIO : public itk::LightObject
{
public:
    // Standard ITK defs
    typedef ItkMagickIO Self;
    typedef itk::LightObject Superclass;
    typedef itk::SmartPointer<Self> Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;

    itkNewMacro(Self);
	itkTypeMacro(ItkMagickIO, LightObject);

    typedef CommonTypes::InternalImageType InternalImageType;
    typedef unsigned short MagickPixelType; // for 16 bit image data
    enum {Dimension = CommonTypes::Dimension};
    typedef itk::Image<MagickPixelType, Dimension> MagickImageType;
    typedef itk::ImportImageFilter<MagickPixelType, Dimension> ImportType;
    typedef itk::CastImageFilter<MagickImageType, InternalImageType> ImportCasterType;
	typedef itk::CastImageFilter<InternalImageType, MagickImageType> ExportCasterType;
	typedef itk::StatisticsImageFilter<MagickImageType> StatisticsType;

    /*
     * Read image data from a file using ImageMagick (which has good
     * tiff readers, and support for 16 bit data, for example) and 
     * immediately transfer that data to an itk::Image object.
     */
    InternalImageType::Pointer Read(std::string filename);

    /*
     * Write an image to a file using ImageMagick.  Not sure we actually
     * need this capability, since ITK's image writers seem to work
     * all right on data in ITK's memory, so this method is not
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
    ImportCasterType::Pointer caster;
};
