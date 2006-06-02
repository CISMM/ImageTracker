#pragma once

#include <string>

#include "itkExceptionObject.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkVector.h"

#include "FilePattern.h"
#include "FileSet.h"
#include "FileSetImageReader.h"
#include "Logger.h"
#include "RegistrationMotionFilter.h"
#include "StopWatch.h"
#include "Test.h"

class TestRegistrationMotionFilter :
    public TestSuite::Test
{
public:
    TestRegistrationMotionFilter(){}
    virtual ~TestRegistrationMotionFilter(){}

    void run()
    {
        this->testOpticFlow();
    }

    void testOpticFlow()
    {
		try
		{
			// Types
			Logger::logDebug("Defining types...");
			typedef itk::Image<float, 2> InputImageType;
			typedef itk::Vector<double, 2> VectorType;
			typedef itk::Image<VectorType, 2> VectorImageType;
			typedef itk::ImageFileReader<InputImageType> ReaderType;
			typedef itk::ImageFileWriter<VectorImageType> WriterType;
			typedef RegistrationMotionFilter<InputImageType, InputImageType> FlowFilterType;

			// Files
			Logger::logDebug("Choosing files...");
	        
			std::string dir = "D:\\beastwoo\\UNC_CS\\Research\\Nano\\data\\test\\";
			FileSet* filesIn = new FileSet(new FilePattern(dir, "RotateNoise-", ".png", 2, 3, 2));
			FileSet* filesOut = new FileSet(filesIn, "RegMotion-", "mha");
			FileSetImageReader* reader = new FileSetImageReader(filesIn);
			reader->FirstImage();
			FileSet::FileIterator outIt = filesOut->GetFileNames()->begin();

			// Objects
			Logger::logDebug("Creating pipeline objects...");
			FlowFilterType::Pointer filter = FlowFilterType::New();
			WriterType::Pointer writer = WriterType::New();

			// Parameters
			filter->SetPatchRadius(5);
			filter->SetMaximumDisplacement(5);

			StopWatch* pWatch = new StopWatch();
			std::string img1, img2, out;
			while (reader->HasNext() && outIt != filesOut->GetFileNames()->end())
			{
				img1 = reader->CurrentFileName();

				// Pipeline
				Logger::logDebug("Connecting pipeline...");
				filter->SetInput1((InputImageType::ConstPointer) reader->CurrentImage());
				filter->SetInput2((InputImageType::ConstPointer) reader->NextImage());
				writer->SetInput(filter->GetOutput());
	            
				img2 = reader->CurrentFileName();
				out = *outIt;
				*(outIt)++;
				writer->SetFileName(out.c_str());

				// Execute
				Logger::logDebug(img1 + " & " + img2 + " => " + out);
				Logger::logDebug("Executing...");
				pWatch->Start("Start: " + out);
				writer->Update();
				pWatch->Stop("Finished");
				Logger::logDebug("Done.");
				pWatch->Print();
			}

			delete (pWatch);
			delete (filesIn);
			delete (filesOut);
			delete (reader);

			succeed_();
		}
		catch (itk::ExceptionObject &err)
		{
			std::cout << "Exception caught!" << std::endl;
			std::cout << err << std::endl;
			fail_("Exception occurred.");
		}
    }
};