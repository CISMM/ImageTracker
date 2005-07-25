#include ".\ItkMagickIO.h"
#include "itkImportImageFilter.h"
#include "Logger.h"

ItkMagickIO::InternalImageType::Pointer ItkMagickIO::MagickToItk(Magick::Image magImage)
{
    // Convert image data.  Based on ItkSoftwareGuide:
    // II. User's Guide / Data Representation / Image / 
    // Importing Image Data from a Buffer

    // Import filter
    typedef itk::ImportImageFilter<MagickPixelType, Dimension> ImportType;
    ImportType::Pointer importer = ImportType::New();

    // Setup ITK Image properties from Magick Image
    // region (size & index), origin, spacing
    ImportType::SizeType size;
    size[0] = magImage.size().width();
    size[1] = magImage.size().height();

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
    // magImage.normalize();
    const unsigned int pixelCount = size[0] * size[1];
    MagickPixelType * buffer = new MagickPixelType[pixelCount];
    const Magick::PixelPacket* packet = magImage.getConstPixels(0, 0, size[0], size[1]);
    
    // Note: we'd like to use Magick::Image.writePixels() to copy image data,
    // but writePixels works with unsigned char pixels, which are not large 
    // enough to hold 12 bit pixels...
    // magImage.writePixels(Magick::GrayQuantum, (MagickPixelType *) buffer);

    double third = 1.0/3.0;
    char text[80];
    sprintf(text, 
        "Quantum size: %i\t Image depth: %i\tRed Depth: %i", sizeof(Magick::Quantum), magImage.depth(), magImage.channelDepth(Magick::RedChannel));
    Logger::logDebug(text);
    long min = 1 << 16, max = 0;

    // Check to see if all image data is in the high side of each byte
    if (sizeof(Magick::Quantum) == 2 && magImage.channelDepth(Magick::RedChannel) <= 8)
    {    
        for (int idx = 0; idx < pixelCount; idx++)
        {
            max = packet[idx].red > max ? packet[idx].red : max;
            min = packet[idx].red < min ? packet[idx].red : min;
            // shift image data to the low side of each byte
            buffer[idx] = packet[idx].red >> 8; // (short) (third * (packet[idx].red + packet[idx].green + packet[idx].blue)) >> 8;
        }
    }
    else
    {
        for (int idx = 0; idx < pixelCount; idx++)
        {
            // TODO: Do we need all pixels for gray scale?
            // Should we divide by three? Yes, otherwise we could get overflow
            buffer[idx] = packet[idx].red; // (third * (packet[idx].red + packet[idx].green + packet[idx].blue));
        }
    }
    sprintf(text, "Minimum: %i\tMaximum: %i", min, max);
    Logger::logDebug(text);
    
    // Import buffer into itk
    // ITK will handle buffer memory
    importer->SetImportPointer(buffer, pixelCount, true);
    caster->Update();

    return caster->GetOutput();
}

Magick::Image ItkMagickIO::ItkToMagick(ItkMagickIO::InternalImageType::Pointer itkImage)
{
    return NULL;
}

ItkMagickIO::InternalImageType::Pointer ItkMagickIO::Read(std::string filename)
{
    Magick::Image image(filename);
    image.depth(8);
    return ItkMagickIO::MagickToItk(image);
}

void ItkMagickIO::Write(std::string filename, InternalImageType::Pointer itkImage)
{
    return;
}