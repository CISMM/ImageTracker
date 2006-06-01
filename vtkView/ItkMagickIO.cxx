#include ".\ItkMagickIO.h"

// #include <Magick++.h>

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

// We need constitute.h to define pixel types, and apparently it
// doesn't get sucked in by Magick++.  Also, we need to
// specify the directory due to the way Magick++ expects the
// include directory to be set up.
// #include "magick/constitute.h"
#include "Logger.h"

// Make sure we initialize ImageMagick the first time through
bool ItkMagickIO::initialized = false;

void ItkMagickIO::Initialize(const char* magickDir)
{
    if (!ItkMagickIO::initialized)
    {
#ifdef _WIN32
        // Magick::InitializeMagick(magickDir);
#endif
        ItkMagickIO::initialized = true;
    }
}

ItkMagickIO::InternalImageType::Pointer ItkMagickIO::Read(std::string filename)
{
	Logger::warning << "ItkMagickIO: this class is deprecated and its use is discouraged." << std::endl;

	typedef itk::ImageFileReader<MagickImageType> ReaderType;
	ReaderType::Pointer reader = ReaderType::New();
	ImportCasterType::Pointer caster = ImportCasterType::New();

	reader->SetFileName(filename.c_str());
	caster->SetInput(reader->GetOutput());
	caster->Update();
	return caster->GetOutput();

// Old ImageMagick implementation:
/*
    // Initialize ImageMagick, if needed
    ItkMagickIO::Initialize("");

	// Read image data using ImageMagick
    Magick::Image image(filename);

    if (!image.isValid())
    {
        Logger::logError("ItkMagickIO::Read: Error reading image data: " + filename);
        return NULL;
    }

	Logger::verbose << "RGB Channel Depth: " << 
		image.channelDepth(Magick::RedChannel) << ", " <<
		image.channelDepth(Magick::GreenChannel) << ", " <<
		image.channelDepth(Magick::BlueChannel) << std::endl;

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

    ImportType::OutputImageType::PointType origin;
    ImportType::OutputImageType::SpacingType spacing;
    origin.Fill(0.0);
    spacing.Fill(1.0);

    // Import filter
    this->importer = ImportType::New();    
    this->importer->SetRegion(region);
    this->importer->SetOrigin(origin);
    this->importer->SetSpacing(spacing);

    // Use a caster to convert from Magick pixels to our internal pixels
    this->caster = ImportCasterType::New();
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
    // ITK will handle buffer memory (last parameter = true)
    this->importer->SetImportPointer(buffer, pixelCount, true);
    this->caster->Update();

	StatisticsType::Pointer stats = StatisticsType::New();
	stats->SetInput(this->importer->GetOutput());
	stats->Update();
	Logger::verbose << "ItkMagickIO::Read() min: " << stats->GetMinimum() << std::endl;
	Logger::verbose << "ItkMagickIO::Read() max: " << stats->GetMaximum() << std::endl;

    return this->caster->GetOutput();
*/
}


void ItkMagickIO::Write(std::string filename, InternalImageType::Pointer itkImage)
{
	Logger::warning << "ItkMagickIO: this class is deprecated and its use is discouraged." << std::endl;

	typedef itk::ImageFileWriter<MagickImageType> WriterType;
	WriterType::Pointer writer = WriterType::New();
	ExportCasterType::Pointer caster = ExportCasterType::New();

	caster->SetInput(itkImage);
	writer->SetInput(caster->GetOutput());
	writer->SetFileName(filename.c_str());
	writer->Update();

// Old ImageMagick implementation:
/*
    // Initialize ImageMagick, if needed
    ItkMagickIO::Initialize("");

	typedef itk::StatisticsImageFilter<InternalImageType> InternalStatsType;
	InternalStatsType::Pointer internal = InternalStatsType::New();
	internal->SetInput(itkImage);
	internal->Update();
	Logger::verbose << "Minimum (float): " << internal->GetMinimum() << std::endl;
	Logger::verbose << "Maximum (float): " << internal->GetMaximum() << std::endl;

	// Cast the given image to the Magick image type
	ExportCasterType::Pointer caster = ExportCasterType::New();
	StatisticsType::Pointer stats = StatisticsType::New();
	caster->SetInput(itkImage);
	stats->SetInput(caster->GetOutput());
	stats->Update();

	Logger::verbose << "ItkMagickIO::Write() min:\t" << stats->GetMinimum() << std::endl;
	Logger::verbose << "ItkMagickIO::Write() max:\t" << stats->GetMaximum() << std::endl;

	// Retrieve information about the image
	InternalImageType::RegionType region = caster->GetOutput()->GetBufferedRegion();
	InternalImageType::SizeType size = region.GetSize();

	// Retrieve image data buffer
	MagickImageType::PixelContainer* container = caster->GetOutput()->GetPixelContainer();
	container->SetContainerManageMemory(false);
	MagickPixelType* buffer = container->GetImportPointer();

	// Create an ImageMagick image of the appropriate size
	char geometry[40];
	sprintf(geometry, "%dx%d", size[0], size[1]);
	Logger::debug << "Creating Magick::Image of size: " << geometry << std::endl;
	Magick::Image image(geometry, "black");
	image.type(Magick::GrayscaleType);
	Logger::verbose << "RGB Channel Depth: " << 
		image.channelDepth(Magick::RedChannel) << ", " <<
		image.channelDepth(Magick::GreenChannel) << ", " <<
		image.channelDepth(Magick::BlueChannel) << std::endl;

	// Grab the pixel packet from the ImageMagick image
	Magick::PixelPacket* packet = image.getPixels(0, 0, size[0], size[1]);
	if (!packet)
	{
		Logger::logError("ItkMagickIO::Write(): Unable to obtain ImageMagick pixel cache.");
		return;
	}

    // Copy image data from buffer to Magick packet
    const unsigned int pixelCount = size[0] * size[1];
	unsigned short min = buffer[0] << 8;
	unsigned short max = buffer[0] << 8;
	for (int idx = 0; idx < pixelCount; idx++)
    {
		packet[idx].red = 
			packet[idx].green =
			packet[idx].blue =
			buffer[idx];
		min = packet[idx].red < min ? packet[idx].red : min;
		max = packet[idx].red > max ? packet[idx].red : max;
    }

	// Synchronize pixels updates the underlying image
	image.syncPixels();
	Logger::verbose << "Magick min:\t" << min << std::endl;
	Logger::verbose << "Magick max:\t" << max << std::endl;

	Logger::verbose << "RGB Channel Depth: " << 
		image.channelDepth(Magick::RedChannel) << ", " <<
		image.channelDepth(Magick::GreenChannel) << ", " <<
		image.channelDepth(Magick::BlueChannel) << std::endl;

	// Write image
	image.write(filename);

	// Keeping all this for now--this is how you might try to save
	// an ImageMagick image if you read their documentation.  Each
	// run through an 8 bit image reduces the image range--the data
	// are shifted slightly higher, I think when the image is
	// quantized as grayscale.  Messed up.

	//// We have to shift the data from ITK's low-order structure to ImageMagick's
	//// default high-order structure.
	//if (stats->GetMaximum() <= 255)
	//{
	//	unsigned short min = buffer[0] << 8;
	//	unsigned short max = buffer[0] << 8;

	//	for (int i = 0; i < size[0] * size[1]; i++)
	//	{
	//		buffer[i] = buffer[i] << 8;
	//		min = buffer[i] < min ? buffer[i] : min;
	//		max = buffer[i] > max ? buffer[i] : max;
	//	}
	//	Logger::debug << "post-shift min:\t" << min << std::endl;
	//	Logger::debug << "post-shift max:\t" << max << std::endl;
	//}

	//// Create a new ImageMagick image
	//Magick::Image image(size[0], size[1], "R", Magick::ShortPixel, buffer);

	//Logger::debug << "RGB Channel Depth: " << 
	//	image.channelDepth(Magick::RedChannel) << ", " <<
	//	image.channelDepth(Magick::GreenChannel) << ", " <<
	//	image.channelDepth(Magick::BlueChannel) << std::endl;

	//image.type(Magick::GrayscaleType);
	//Logger::debug << "RGB Channel Depth: " << 
	//	image.channelDepth(Magick::RedChannel) << ", " <<
	//	image.channelDepth(Magick::GreenChannel) << ", " <<
	//	image.channelDepth(Magick::BlueChannel) << std::endl;

	//if (stats->GetMaximum() <= 255)
	//{
	//	image.depth(8);
	//	image.channelDepth(Magick::RedChannel, 8);
	//	image.channelDepth(Magick::GreenChannel, 8);
	//	image.channelDepth(Magick::BlueChannel, 8);
	//}

	//Logger::debug << "RGB Channel Depth: " << 
	//	image.channelDepth(Magick::RedChannel) << ", " <<
	//	image.channelDepth(Magick::GreenChannel) << ", " <<
	//	image.channelDepth(Magick::BlueChannel) << std::endl;
*/
}