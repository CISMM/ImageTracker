
#include "TestMultiRegionRegistration.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkExceptionObject.h"

#include "../FileSet.h"
#include "../ImageRegistration.h"
#include "../Logger.h"
#include "../MultiRegionRegistration.h"
#include "../MultiRegionRegistrationPipeline.h"
#include "../StopWatch.h"

TestMultiRegionRegistration::TestMultiRegionRegistration()
{
}

void TestMultiRegionRegistration::run()
{
    Logger::logInfo("Running TestMultiRegionRegistration...");
    this->testUpdate();
}

void TestMultiRegionRegistration::testUpdate()
{
    try
    {
        Logger::logDebug("TestMultiRegionRegistration: \n\tSetting up IO files....");
        std::string dir = "D:\\beastwoo\\UNC_CS\\Research\\Nano\\data\\test\\";
        FileSet* filesIn = new FileSet(new FilePattern(dir, "RotateNoise-", ".png", 2, 3, 2));
        FileSet* filesOut = new FileSet(filesIn, "RegMulti-", "mha");

        //Set up file registerer
        Logger::logDebug("\tSetting up registration performer....");
        MultiRegionRegistration* registrar = new MultiRegionRegistration();
        registrar->SetRadiusOfInterest(5);
        registrar->SetROIRatio(2.0);

        Logger::logDebug("\tSetting up registration pipeline.");
        MultiRegionRegistrationPipeline* pPipeline = new MultiRegionRegistrationPipeline();
        pPipeline->SetRegistrar(registrar);
        pPipeline->SetSource(filesIn);
        pPipeline->SetDestination(filesOut);
        
        Logger::logDebug("\tRunning registration....");
        StopWatch* pWatch = new StopWatch();
        pWatch->Start("Started MultiRegionRegistration");
        pPipeline->Update(false); // true: find features first
        pWatch->Stop("Finished");
        pWatch->Print();

        delete(pWatch);
        delete(filesIn);
		delete(filesOut);
        delete(registrar);
        delete(pPipeline);

        succeed_();
    }
    catch ( itk::ExceptionObject &err)
    {
        std::cout << "Exception caught!" << std::endl; 
        std::cout << err << std::endl; 
        fail_("Exception occurred.");
    }
}
