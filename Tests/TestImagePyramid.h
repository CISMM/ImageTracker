#pragma once
#include <string>

#include "itkCastImageFilter.h"
#include "itkExceptionObject.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMultiResolutionPyramidImageFilter.h"

#include "Logger.h"
#include "Test.h"

class TestImagePyramid :
    public TestSuite::Test
{
public:
	TestImagePyramid(){}
	~TestImagePyramid(){}

	void run()
	{
		this->testPyramid(6);
	}

	void testPyramid(int levels = 4)
	{
		typedef itk::Image<unsigned char, 2> ImageType;
		typedef itk::Image<float, 2> InternalImageType;
		typedef itk::ImageFileReader<ImageType> ReaderType;
		typedef itk::ImageFileWriter<ImageType> WriterType;
		typedef itk::CastImageFilter<ImageType, InternalImageType> CastInType;
		typedef itk::CastImageFilter<InternalImageType, ImageType> CastOutType;
		typedef itk::MultiResolutionPyramidImageFilter<InternalImageType, InternalImageType> PyramidType;

		std::string dir = "D:/Nano/data/Lisa/WH200311/";
		std::string base = "GoodfromWHrh01";
		std::string ext = ".png";
		std::string inFile = dir + base + ext;

		try
		{
			Logger::debug << "Creating pipeline objects." << std::endl;
			ReaderType::Pointer reader = ReaderType::New();
			CastInType::Pointer castIn = CastInType::New();
			CastOutType::Pointer castOut = CastOutType::New();
			PyramidType::Pointer pyramid = PyramidType::New();
			WriterType::Pointer writer = WriterType::New();

			long shrink = pow((long) 2, (long) levels-1);
			reader->SetFileName(inFile.c_str());
			castIn->SetInput(reader->GetOutput());
			

			std::string outFile = dir + base + "-orig" + ext;
			Logger::debug << "Saving original file: " << outFile << std::endl;
			castOut->SetInput(castIn->GetOutput());
			writer->SetFileName(outFile.c_str());
			writer->SetInput(castOut->GetOutput());
			writer->Update();

			Logger::debug << "Initializing pipeline with: " << std::endl;
			Logger::debug << "   File:   " << inFile << std::endl;
			Logger::debug << "   Levels: " << levels << std::endl;
			Logger::debug << "   Shrink: " << shrink << std::endl;
			pyramid->SetInput(castIn->GetOutput());
			pyramid->SetStartingShrinkFactors(shrink);
			pyramid->SetNumberOfLevels(levels);
			pyramid->SetNumberOfThreads(1);
			Logger::debug << "Updating pyramid..." << std::endl;
			pyramid->Update();
			
			char num[5];
			for (unsigned int i = 0; i < levels; i++)
			{
				sprintf(num, "%d", i);
				outFile = dir + base + "-" + num + ext;
				Logger::debug << "Writing: " << outFile << ", level " << i << std::endl;		
				castOut->SetInput(pyramid->GetOutput(i));
				writer->SetFileName(outFile.c_str());
				writer->Update();
			}

			succeed_();
		}
		catch (itk::ExceptionObject &err)
		{
			Logger::error << "Exception caught!" << std::endl;
			std::cerr << err << std::endl;
			fail_("Exception occurred.");
		}
	}
};