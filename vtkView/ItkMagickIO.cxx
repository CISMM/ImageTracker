#include ".\ItkMagickIO.h"

#include <Magick++.h>
#include "itkImportImageFilter.h"
#include "../Logger.h"

// Make sure we initialize ImageMagick the first time through
bool ItkMagickIO::initialized = false;

void ItkMagickIO::Initialize(const char* magickDir)
{
    if (!ItkMagickIO::initialized)
    {
#ifdef _WIN32
        Magick::InitializeMagick(magickDir);
#endif
        ItkMagickIO::initialized = true;
    }
}

ItkMagickIO::InternalImageType::Pointer ItkMagickIO::Read(std::string filename)
{
    // Initialize ImageMagick, if needed
    ItkMagickIO::Initialize("");

    // Read image data using ImageMagick
    Magick::Image image(filename);

    if (!image.isValid())
    {
        Logger::logError("ItkMagickIO::Read: Error reading image data: " + filename);
        return NULL;
    }

    // Convert image data.  Based on ItkSoftwareGuide:
    // II. User's Guide / Data Representation / Image / 
    // Importing Image Data from a Buffer

    // Setup ITK Image properties from Magick Image
    // region (size & index), origin, spacing
    ImportType::SizeType size;
    size[0] = image.columns();
    size[1] = image.rows();

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

    // Import filter
    this->importer = ImportType::New();    
    this->importer->SetRegion(region);
    this->importer->SetOrigin(origin);
    this->importer->SetSpacing(spacing);

    // Use a caster to convert from Magick pixels to our internal pixels
    this->caster = CasterType::New();
    this->caster->SetInput(importer->GetOutput());

    // Read pixel buffer from Magick image
    // Magick::Quantum is a 2 byte (16 bit) data type
    const unsigned int pixelCount = size[0] * size[1];
    MagickPixelType* buffer = new MagickPixelType[pixelCount];
    const Magick::PixelPacket* packet = image.getConstPixels(0, 0, size[0], size[1]);
    
    if (!packet)
    {
        Logger::logError("ItkMagickIO::Read: Unable to obtain ImageMagick pixel cache.");
        return NULL;
    }
    
    double third = 1.0/3.0;
    // long min = 1 << 16, max = 0;

    int shift = 0;
    // Check to see if all image data is in the high side of each byte
    // ImageMagick does this by default for all 8 bit images.
    if (sizeof(Magick::Quantum) == 2 && image.channelDepth(Magick::RedChannel) <= 8)
    {
        shift = 8;
    }

    // Copy image data from Magick packet to buffer
    for (int idx = 0; idx < pixelCount; idx++)
    {
        buffer[idx] = packet[idx].red >> shift;
    }
    
    // Import buffer into itk
    // ITK will handle buffer memory
    this->importer->SetImportPointer(buffer, pixelCount, true);
    this->caster->Update();

    return this->caster->GetOutput();
}


void ItkMagickIO::Write(std::string filename, InternalImageType::Pointer itkImage)
{
    return;
}