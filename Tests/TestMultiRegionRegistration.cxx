
#include "TestMultiRegionRegistration.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkExceptionObject.h"

#include "../FileSet.h"
#include "../ImageRegistration.h"
#include "../MultiRegionRegistration.h"
#include "../MultiRegionRegistrationPipeline.h"
#include "../StopWatch.h"

TestMultiRegionRegistration::TestMultiRegionRegistration()
{
}

void TestMultiRegionRegistration::run()
{
    this->testUpdate();
}

void TestMultiRegionRegistration::testUpdate()
{
    try
    {
        std::cout << "Setting up IO files." << std::endl;
        std::string inDir  = "C:\\UNC CS\\Research\\nano\\data\\Lisa\\out\\WH200311\\";
        std::string outDir = "C:\\UNC CS\\Research\\nano\\data\\Lisa\\out\\WH200311\\";
        //std::string inDir  = "C:\\UNC CS\\Research\\nano\\data\\test\\";
        //std::string outDir = "C:\\UNC CS\\Research\\nano\\data\\test\\";

        std::string prefix = "Blur6MeanWHrh01-";
        std::string ext = ".png";
        int start = 2;
        int end = 4;
        int places = 2;
        FileSet* pInFiles = new FileSet(new FilePattern(inDir, prefix, ext, start, end, places));
        FileSet* pOutFiles = new FileSet(new FilePattern(outDir, "RegionT2D-" + prefix, ".vtk", start, end-1, places));

        //Set up file registerer
        std::cout << "Setting up registration performer." << std::endl;
        MultiRegionRegistration* registrar = new MultiRegionRegistration();
        registrar->SetRadiusOfInterest(5);
        registrar->SetROIRatio(2.0);

        std::cout << "Setting up registration pipeline." << std::endl;
        MultiRegionRegistrationPipeline* pPipeline = new MultiRegionRegistrationPipeline();
        pPipeline->SetRegistrar(registrar);
        pPipeline->SetSource(pInFiles);
        pPipeline->SetDestination(pOutFiles);
        
        std::cout << "Running registration...." << std::endl;
        StopWatch* pWatch = new StopWatch();
        pWatch->Start("Started");
        pPipeline->Update();
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
