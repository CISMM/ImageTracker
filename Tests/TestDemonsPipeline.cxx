#include "TestDemonsPipeline.h"

#include "itkExceptionObject.h"

#include "..\DemonsPipeline.h"
#include "..\FileSet.h"
#include "..\Logger.h"

TestDemonsPipeline::TestDemonsPipeline(void)
{
}

TestDemonsPipeline::~TestDemonsPipeline(void)
{
}

void TestDemonsPipeline::run()
{
    Logger::logInfo("Running TestDemonsPipeline...");
    this->testUpdate();
}

void TestDemonsPipeline::testUpdate()
{
    try
    {
        // Set up in/out files
        std::string inDir  = "C:\\UNC_CS\\Research\\nano\\data\\test\\Quenot\\";
        std::string outDir = "C:\\UNC_CS\\Research\\nano\\data\\test\\Quenot\\";

        std::string prefix = "Ice";
        std::string ext = ".png";
        int start = 0;
        int end = 1;
        int places = 2;
        FileSet* pInFiles = new FileSet(new FilePattern(inDir, prefix, ext, start, end, places));
        FileSet* pOutFiles = new FileSet(new FilePattern(outDir, "Demons-" + prefix, ".vtk", start, end-1, places));
        
        // Set up Demons registration
        DemonsPipeline* pDemons = new DemonsPipeline();
        pDemons->SetInputFiles(pInFiles);
        pDemons->SetOutputFiles(pOutFiles);
        
        // Run registration
        pDemons->Update();

        delete pDemons;
        delete pInFiles;
        delete pOutFiles;

        succeed_();
    }
    catch (itk::ExceptionObject &err)
    {
        std::cout << "Exception caught!" << std::endl;
        std::cout << err << std::endl;
        fail_("Exception occurred.");
    }
}