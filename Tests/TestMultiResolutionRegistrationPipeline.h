#pragma once

#include <string>

#include "Test.h"

#include "FilePattern.h"
#include "FileSet.h"
#include "Logger.h"
#include "MultiResolutionRegistrationPipeline.h"
#include "ItkMagickIO.h"
#include "StopWatch.h"

class TestMultiResolutionRegistrationPipeline :
	public TestSuite::Test
{
public:
	TestMultiResolutionRegistrationPipeline(){}
	virtual ~TestMultiResolutionRegistrationPipeline(){}

	void run()
	{
		this->testRegistration();
	}

	void testRegistration()
	{
		try
		{
			Logger::debug << "Initializing files." << std::endl;
			std::string dir = "D:/Nano/data/Lisa/WH200311/";
			FileSet filesIn(new FilePattern(dir, "GoodfromWHrh", ".png", 1, 5, 2));
			FileSet filesOut(new FilePattern(dir, "RegPipe-GoodfromWHrh", ".png", 1, 5, 2));
			std::string preview = dir + "Blur-GoodfromWHrh01.png";
			std::string transforms = dir + "Transforms.txt";

			Logger::debug << "Setting up pipeline..." << std::endl;
			MultiResolutionRegistrationPipeline::Pointer pipeline = MultiResolutionRegistrationPipeline::New();

			pipeline->SetInputFiles(filesIn);
			pipeline->SetOutputFiles(filesOut);
			pipeline->SetTransformFile(transforms);

			pipeline->SetNumberOfLevels(1);
			pipeline->SetStartingShrinkFactor(5);
			pipeline->SetUpperThreshold(255);
			pipeline->SetLowerThreshold(100);

			// Save preview image
			ItkMagickIO::Pointer writer = ItkMagickIO::New();
			writer->Write(preview, pipeline->GetPreviewImage());

			Logger::debug << "Running registration..." << std::endl;
			StopWatch watch;
			watch.Start("Registration");
			pipeline->Update();
			watch.Stop("Registration");
			watch.Print();

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