#pragma once
#include <string>

#include "itkImage.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "ItkMagickIO.h"
#include "Logger.h"
#include "..\TestSuite\Test.h"
#include "ImageWindow.h"

class TestItkMagickIO : public TestSuite::Test
{
public:
    TestItkMagickIO() {}
    ~TestItkMagickIO() {}

    void run()
    {
		this->CreateTestImage();
        this->testMagickReadWriteTiff();
		this->testMagickReadWrite8Bit();
		this->testMagickReadWrite16bit();
		// ITK cannot read and write certain TIFF images.
		this->testItkReadWrite();
		this->testMagickAPIReadWrite();
    }

    void testMagickReadWriteTiff()
    {
        std::string dir = "D:/Nano/data/Fibrin/lolo-2004-05-14-2narea/";
        std::string name = "mani415.tif";
		std::string inName = dir + name;
		std::string outName = dir + "magickIO-" + name;

		Logger::verbose << "Size of MagickPixelType (unsigned char):\t" << sizeof(ItkMagickIO::MagickPixelType) << std::endl;
		Logger::verbose << "Size of InputPixelType (unsigned short):\t" << sizeof(CommonTypes::InputPixelType) << std::endl;
		Logger::verbose << "Size of InternalPixelType (float):\t" << sizeof(CommonTypes::InternalPixelType) << std::endl;
		Logger::verbose << "Size of Magick::PixelPacket: \t" << sizeof(Magick::PixelPacket) << std::endl;

        ItkMagickIO::Pointer ioObj = ItkMagickIO::New();
		Logger::debug << "Reading " << inName << " using ImageMagick..." << std::endl;
        ItkMagickIO::InternalImageType::Pointer img = ioObj->Read(inName);
		Logger::debug << "Read." << std::endl;
		Logger::debug << "Writing " << outName << " using ImageMagick..." << std::endl;
		ioObj->Write(outName, img);
		Logger::debug << "Wrote." << std::endl;
		succeed_();
        // Logger::logDebug("Displaying image...");
        // ImageWindow::ImShow(img, name);
    }

	void testMagickReadWrite8Bit()
	{
        std::string dir = "D:/Nano/data/Lisa/WH200311/";
        std::string name = "GoodfromWHrh01.png";
        //std::string dir = "D:/Nano/data/test/Quenot/";
        //std::string name = "magickIO-magickIO-Ice00.tif";
		std::string inName = dir + name;
		std::string outName = dir + "magickIO-" + name;

		ItkMagickIO::Pointer io = ItkMagickIO::New();
		Logger::debug << "Reading: " << inName << " using ImageMagick..." << std::endl;
		ItkMagickIO::InternalImageType::Pointer img = io->Read(inName);
		Logger::debug << "Read." << std::endl;

		Logger::debug << "Writing: " << outName << " using ImageMagick..." << std::endl;
		io->Write(outName, img);
		Logger::debug << "Wrote." << std::endl;
		succeed_();
	}

	void testMagickReadWrite16bit()
	{
        //std::string dir = "D:/Nano/data/Lisa/WH200311/";
        //std::string name = "GoodfromWHrh01.png";
        std::string dir = "D:/Nano/data/test/Quenot/";
        std::string name = "Ice00.tif";
		std::string inName = dir + name;
		std::string outName = dir + "magickIO-16b-" + name;

		Logger::debug << "Reading: " << inName << " using ImageMagick..." << std::endl;
		ItkMagickIO::Pointer io = ItkMagickIO::New();
		ItkMagickIO::InternalImageType::Pointer img = io->Read(inName);

		Logger::debug << "Rescaling image to full 16 bits." << std::endl;
		typedef itk::RescaleIntensityImageFilter<ItkMagickIO::InternalImageType, ItkMagickIO::InternalImageType> RescaleType;
		RescaleType::Pointer rescaler = RescaleType::New();
		rescaler->SetInput(img);
		rescaler->SetOutputMinimum(0);
		rescaler->SetOutputMaximum(65535);
		rescaler->Update();
		Logger::debug << "Writing: " << outName << " using ImageMagick..." << std::endl;
		io->Write(outName, rescaler->GetOutput());
		succeed_();
	}

	void testItkReadWrite()
	{
		typedef itk::Image<unsigned char, 2> ImageType;
		typedef itk::ImageFileReader<ImageType> ReaderType;
		typedef itk::ImageFileWriter<ImageType> WriterType;

		ReaderType::Pointer reader = ReaderType::New();
		WriterType::Pointer writer = WriterType::New();

        //std::string dir = "D:/Nano/data/Fibrin/lolo-2004-05-14-2narea/";
        //std::string name = "mani415.tif";
        std::string dir = "D:/Nano/data/Lisa/WH200311/";
        std::string name = "GoodfromWHrh01.png";
		std::string inName = dir + name;
		std::string outName = dir + "itkIO-" + name;
		reader->SetFileName(inName.c_str());
		writer->SetInput(reader->GetOutput());
		writer->SetFileName(outName.c_str());

		Logger::debug << "Reading " << inName << " using ITK..." << std::endl;
		reader->Update();
		Logger::debug << "Read." << std::endl;
		Logger::debug << "Writing " << outName << " using ITK..." << std::endl;
		writer->Update();
		Logger::debug << "Wrote." << std::endl;
		succeed_();
	}

	void testMagickAPIReadWrite()
	{
        std::string dir = "D:/Nano/data/Lisa/WH200311/";
        std::string name = "GoodfromWHrh01.png";
		std::string inName = dir + name;
		std::string outName = dir + "magickAPI-" + name;

		Logger::debug << "Reading " << inName << " using Magick API." << std::endl;
		Magick::Image image(inName);

		Logger::debug << "RGB Channel Depth after read: " << 
			image.channelDepth(Magick::RedChannel) << ", " <<
			image.channelDepth(Magick::GreenChannel) << ", " <<
			image.channelDepth(Magick::BlueChannel) << std::endl;

		const unsigned int pixelCount = image.columns() * image.rows();
		Magick::PixelPacket* packet = image.getPixels(0, 0, image.columns(), image.rows());
		
		unsigned short min16 = packet[0].red;
		unsigned short max16 = packet[0].red;
		unsigned short min8 = packet[0].red >> 8;
		unsigned short max8 = packet[0].red >> 8;
		for (int idx = 0; idx < pixelCount; idx++)
		{
			min16 = packet[idx].red < min16 ? packet[idx].red : min16;
			max16 = packet[idx].red > max16 ? packet[idx].red : max16;
			packet[idx].red = packet[idx].red >> 8;
			min8 = packet[idx].red < min8 ? packet[idx].red : min8;
			max8 = packet[idx].red > max8 ? packet[idx].red : max8;
		}

		Logger::debug << "Magick min(16): " << min16 << std::endl;
		Logger::debug << "Magick max(16): " << max16 << std::endl;
		Logger::debug << "Magick min(8): " << min8 << std::endl;
		Logger::debug << "Magick max(8): " << max8 << std::endl;

		image.syncPixels();
		Logger::debug << "RGB Channel Depth after shift right: " << 
			image.channelDepth(Magick::RedChannel) << ", " <<
			image.channelDepth(Magick::GreenChannel) << ", " <<
			image.channelDepth(Magick::BlueChannel) << std::endl;

		min16 = packet[0].red << 8;
		max16 = packet[0].red << 8;
		min8 = packet[0].red;
		max8 = packet[0].red;
		for (int idx = 0; idx < pixelCount; idx++)
		{
			min8 = packet[idx].red < min8 ? packet[idx].red : min8;
			max8 = packet[idx].red > max8 ? packet[idx].red : max8;
			packet[idx].red = packet[idx].red << 8;
			min16 = packet[idx].red < min16 ? packet[idx].red : min16;
			max16 = packet[idx].red > max16 ? packet[idx].red : max16;
		}

		Logger::debug << "Magick min(16): " << min16 << std::endl;
		Logger::debug << "Magick max(16): " << max16 << std::endl;
		Logger::debug << "Magick min(8): " << min8 << std::endl;
		Logger::debug << "Magick max(8): " << max8 << std::endl;

		image.syncPixels();
		Logger::debug << "RGB Channel Depth after shift left: " << 
			image.channelDepth(Magick::RedChannel) << ", " <<
			image.channelDepth(Magick::GreenChannel) << ", " <<
			image.channelDepth(Magick::BlueChannel) << std::endl;

		Logger::debug << "Writing " << outName << " using Magick API." << std::endl;
		image.write(outName);
	}

	void CreateTestImage()
	{
		std::string dir = "D:/Nano/data/test/";
		std::string file = "gradient.tif";
		std::string out = dir + file;
		Logger::debug << "Creating 256x256 image." << std::endl;
		Magick::Image image("256x256", "black");
		Logger::debug << "RGB Channel Depth: " << 
			image.channelDepth(Magick::RedChannel) << ", " <<
			image.channelDepth(Magick::GreenChannel) << ", " <<
			image.channelDepth(Magick::BlueChannel) << std::endl;

		Logger::debug << "Filling pixel packet." << std::endl;
		Magick::PixelPacket* packet = image.getPixels(0, 0, 256, 256);
		unsigned short yoff = 0;
		for (unsigned short y = 0; y < 256; y++)
		{
			yoff = y * 256;
			for (unsigned short x = 0; x < 256; x++)
			{
				packet[yoff + x].red = y;	// << 8;
				packet[yoff + x].green = y;	// << 8;
				packet[yoff + x].blue = y;	// << 8;
			}
		}
		Logger::debug << "RGB Channel Depth: " << 
			image.channelDepth(Magick::RedChannel) << ", " <<
			image.channelDepth(Magick::GreenChannel) << ", " <<
			image.channelDepth(Magick::BlueChannel) << std::endl;

		image.type(Magick::GrayscaleType);

		Logger::debug << "RGB Channel Depth: " << 
			image.channelDepth(Magick::RedChannel) << ", " <<
			image.channelDepth(Magick::GreenChannel) << ", " <<
			image.channelDepth(Magick::BlueChannel) << std::endl;

		Logger::debug << "Synching pixels and writing: " << out << std::endl;
		image.syncPixels();
		image.write(out);
	}
};