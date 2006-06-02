#pragma once

#include <string>
#include <vector>

#include "itkExceptionObject.h"
#include "itkCenteredRigid2DTransform.h"

#include "Test.h"

#include "FilePattern.h"
#include "FileSet.h"
#include "FileSetImageReader.h"
#include "Logger.h"
#include "MultiResolutionRegistration.h"
#include "RegistrationOutput.h"
#include "StopWatch.h"

class TestMultiResolutionRegistration :
	public TestSuite::Test
{
public:
	TestMultiResolutionRegistration(){}
	virtual ~TestMultiResolutionRegistration(){}

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
			FileSet* filesIn = new FileSet(new FilePattern(dir, "GoodfromWHrh", ".png", 1, 2, 2));
			FileSet* filesOut = new FileSet(new FilePattern(dir, "Reg-GoodfromWHrh", ".png", 1, 2, 2));
			FileSetImageReader* reader = new FileSetImageReader(filesIn);

			Logger::debug << "Setting up pipeline objects..." << std::endl;
			typedef itk::CenteredRigid2DTransform<double> TransformType;
			typedef MultiResolutionRegistration<CommonTypes::InternalImageType, TransformType> RegistrationType;
			RegistrationType::Pointer registrar = RegistrationType::New();

			registrar->SetNumberOfLevels(3);
			registrar->SetStartingShrinkFactor(32);
			registrar->SetFixedImage(reader->CurrentImage());
			registrar->SetMovingImage(reader->NextImage());

			Logger::debug << "Running registration..." << std::endl;
			StopWatch watch;
			watch.Start("Registration");
			registrar->StartRegistration();
			watch.Stop("Registration");
			watch.Print();

			std::vector<TransformType::Pointer> vec;
			vec.push_back(registrar->GetLastTransform());

			Logger::debug << "Saving output images..." << std::endl;
			RegistrationOutput* output = new RegistrationOutput(filesIn, &vec, filesOut);
			output->Update();
			Logger::debug << "Done." << std::endl;

			delete(output);
			delete(reader);
			delete(filesOut);
			delete(filesIn);

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