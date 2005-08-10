#include ".\ItkMagickIO.h"

// Required for operation of ImageMagick
#define QuantumLeap
#include <magick/api.h>

#include "itkImportImageFilter.h"
#include "Logger.h"

// Make sure we initialize ImageMagick the first time through
bool ItkMagickIO::initialized = false;

void ItkMagickIO::Initialize(const char* magickDir)
{
    if (!ItkMagickIO::initialized)
    {
#ifdef _WIN32
        InitializeMagick(magickDir);
#endif
        ItkMagickIO::initialized = true;
    }
}

ItkMagickIO::InternalImageType::Pointer ItkMagickIO::Read(std::string filename)
{
    // Initialize ImageMagick, if needed
    ItkMagickIO::Initialize("");

    // Read image data using ImageMagick
    ExceptionInfo exception;
    Image* pImage;
    ImageInfo* pImageInfo;
    PixelPacket* pPixelPacket;
    ViewInfo* pViewInfo;

    GetExceptionInfo(&exception);
    pImageInfo = CloneImageInfo((ImageInfo*) NULL);
    strcpy(pImageInfo->filename, filename.c_str());
    pImage = ReadImage(pImageInfo, &exception);

    if (pImage == (Image*) NULL)
    {
        char msg[256];
        sprintf(msg, "ItkMagickIO::Read: Error reading image data: %s: %s", 
            exception.reason, exception.description);
        Logger::logError(msg);

        return NULL;
    }

    // Convert image data.  Based on ItkSoftwareGuide:
    // II. User's Guide / Data Representation / Image / 
    // Importing Image Data from a Buffer

    // Import filter
    typedef itk::ImportImageFilter<MagickPixelType, Dimension> ImportType;
    ImportType::Pointer importer = ImportType::New();

    // Setup ITK Image properties from Magick Image
    // region (size & index), origin, spacing
    ImportType::SizeType size;
    size[0] = pImage->columns;
    size[1] = pImage->rows;

    ImportType::IndexType index;
    index.Fill(0);

    ImportType::RegionType region;
    region.SetIndex(index);
    region.SetSize(size);

    double origin[Dimension];
    double spacing[Dimension];
    for (int d = 0; d < Dimension; d++)
    {
        origin[d] = 0.0;
        spacing[d] = 1.0;
    }
    
    importer->SetRegion(region);
    importer->SetOrigin(origin);
    importer->SetSpacing(spacing);

    // Use a caster to convert from Magick pixels to our internal pixels
    CasterType::Pointer caster = CasterType::New();
    caster->SetInput(importer->GetOutput());

    // Read pixel buffer from Magick image
    // Magick::Quantum is a 2 byte (16 bit) data type
    const unsigned int pixelCount = size[0] * size[1];
    MagickPixelType* buffer = new MagickPixelType[pixelCount];
    pViewInfo = OpenCacheView(pImage);
    pPixelPacket = GetCacheView(pViewInfo, 0, 0, size[0], size[1]);
    
    if (!pPixelPacket)
    {
        Logger::logError("ItkMagickIO::Read: Unable to obtain ImageMagick pixel cache.");
        return NULL;
    }
    
    double third = 1.0/3.0;
    // long min = 1 << 16, max = 0;

    // Check to see if all image data is in the high side of each byte
    if (pImage->depth < 16)
    {    
        for (int idx = 0; idx < pixelCount; idx++)
        {
            // max = packet[idx].red > max ? packet[idx].red : max;
            // min = packet[idx].red < min ? packet[idx].red : min;
            // shift image data to the low side of each byte
            buffer[idx] = pPixelPacket[idx].red >> 8; // (short) (third * (packet[idx].red + packet[idx].green + packet[idx].blue)) >> 8;
        }
    }
    else
    {
        for (int idx = 0; idx < pixelCount; idx++)
        {
            // TODO: Do we need all pixels for gray scale?
            // Should we divide by three? Yes, otherwise we could get overflow
            buffer[idx] = pPixelPacket[idx].red; // (third * (packet[idx].red + packet[idx].green + packet[idx].blue));
        }
    }
    
    // Import buffer into itk
    // ITK will handle buffer memory
    importer->SetImportPointer(buffer, pixelCount, true);
    caster->Update();

    // Delete ImageMagick objects
    CloseCacheView(pViewInfo);
    DestroyImageInfo(pImageInfo);
    DestroyImage(pImage);

    return caster->GetOutput();
}


void ItkMagickIO::Write(std::string filename, InternalImageType::Pointer itkImage)
{
    return;
}