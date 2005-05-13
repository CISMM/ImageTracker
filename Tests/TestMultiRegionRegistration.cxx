
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
        std::string inDir("C:\\UNC_CS\\Research\\nano\\data\\Fibrin\\lolo-2004-05-14-2narea\\");
        std::string outDir("C:\\UNC_CS\\Research\\nano\\data\\Fibrin\\lolo-2004-05-14-2narea\\");
        //std::string inDir  = "C:\\UNC_CS\\Research\\nano\\data\\test\\Quenot\\";
        //std::string outDir = "C:\\UNC_CS\\Research\\nano\\data\\test\\Quenot\\";

        std::string prefix = "mani4";
        std::string ext = ".png";
        int start = 15;
        int end = 16;
        int places = 2;
        FileSet* pInFiles = new FileSet(new FilePattern(inDir, prefix, ext, start, end, places));
        FileSet* pOutFiles = new FileSet(new FilePattern(outDir, "HarrisT2D-" + prefix, ".mha", start, end-1, places));

        //Set up file registerer
        Logger::logDebug("\tSetting up registration performer....");
        MultiRegionRegistration* registrar = new MultiRegionRegistration();
        registrar->SetRadiusOfInterest(5);
        registrar->SetROIRatio(2.0);

        Logger::logDebug("\tSetting up registration pipeline.");
        MultiRegionRegistrationPipeline* pPipeline = new MultiRegionRegistrationPipeline();
        pPipeline->SetRegistrar(registrar);
        pPipeline->SetSource(pInFiles);
        pPipeline->SetDestination(pOutFiles);
        
        Logger::logDebug("\tRunning registration....");
        StopWatch* pWatch = new StopWatch();
        pWatch->Start("Started");
        pPipeline->Update(true);
        pWatch->Stop("Finished");
        pWatch->Print();

        delete(pWatch);
        delete(pInFiles);
        delete(pOutFiles);
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
