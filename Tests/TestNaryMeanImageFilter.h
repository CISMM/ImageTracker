#pragma once

#include <string>

#include "itkExceptionObject.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "Logger.h"
#include "NaryMeanImageFilter.h"
#include "Test.h"

class TestNaryMeanImageFilter : 
	public TestSuite::Test
{
public:

	TestNaryMeanImageFilter(){}
	virtual ~TestNaryMeanImageFilter(){}

	void run()
	{
		this->testSingleImage(5);
		this->testSeveralImages();
	}

	typedef itk::Image<unsigned short, 2> ImageType;
	typedef itk::ImageFileReader<ImageType> ReaderType;
	typedef itk::ImageFileWriter<ImageType> WriterType;
	typedef NaryMeanImageFilter<ImageType, ImageType> MeanType;

	void testSingleImage(int count)
	{
		std::string dir = "D:/Nano/data/noise/";
		std::string file = "Noise00.tif";
		std::string inFile = dir + file;
		std::string outFile = dir + "Mean-" + file;

		try
		{
			ReaderType::Pointer reader = ReaderType::New();
			WriterType::Pointer writer = WriterType::New();
			MeanType::Pointer mean = MeanType::New();

			reader->SetFileName(inFile.c_str());
			reader->Update();
			writer->SetInput(mean->GetOutput());
			writer->SetFileName(outFile.c_str());

			for (unsigned int i = 0; i < count; i++)
			{
				mean->SetInput(i, reader->GetOutput());
			}

			writer->Update();
			succeed_();
		}
		catch (itk::ExceptionObject& err)
		{
			std::cerr << "Exception occurred." << std::endl;
			std::cerr << err << std::endl;
			fail_("Exception caught.");
		}
	}

	void testSeveralImages()
	{
		std::string dir = "D:/Nano/data/taylor/";
		std::string prefix = "Vortex";
		std::string ext = ".tif";
		FilePattern pattern(dir, prefix, ext, 1, 40, 3);
		FileSet inSet(&pattern);

		std::string outFile = dir + "Mean-" + prefix + ext;

		try
		{
			MeanType::Pointer mean = MeanType::New();
			WriterType::Pointer writer = WriterType::New();

			writer->SetInput(mean->GetOutput());
			writer->SetFileName(outFile.c_str());

			FileSet::FileIterator it = inSet.GetFileNames()->begin();
			unsigned int i = 0;

			for (FileSet::FileIterator it = inSet.GetFileNames()->begin();
				it != inSet.GetFileNames()->end();
				++it)
			{
				ReaderType::Pointer reader = ReaderType::New();				
				reader->SetFileName(it->c_str());
				reader->Update();
				mean->SetInput(i++, reader->GetOutput());
			}
						
			writer->Update();
			succeed_();
		}
		catch (itk::ExceptionObject& err)
		{
			std::cerr << "Exception occurred." << std::endl;
			std::cerr << err << std::endl;
			fail_("Exception caught.");
		}
	}
};
