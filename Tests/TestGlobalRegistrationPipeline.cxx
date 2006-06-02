#include ".\TestGlobalRegistrationPipeline.h"
#include <string>
#include <iostream>

#include "itkExceptionObject.h"
#include "FilePattern.h"
#include "FileSet.h"
#include "GlobalRegistrationPipeline.h"
#include "TransformGroup.h"
#include "Logger.h"

TestGlobalRegistrationPipeline::TestGlobalRegistrationPipeline(void)
{
}

TestGlobalRegistrationPipeline::~TestGlobalRegistrationPipeline(void)
{
}

void TestGlobalRegistrationPipeline::run()
{
    this->testUpdateAll();
}

void TestGlobalRegistrationPipeline::testUpdateAll()
{
    try
    {
        Logger::logInfo("Setting up IO files.");
        std::string inDir = "C:\\UNC_CS\\Research\\Nano\\data\\Lisa\\WH200311\\";
        std::string outDir = "C:\\UNC_CS\\Research\\Nano\\data\\Lisa\\out\\WH200311\\";

        std::string prefix = "GoodfromWHrh";
        std::string ext = ".png";
        int start = 1;
        int end = 10;
        int places = 2;
        FileSet* pInFiles = new FileSet(new FilePattern(inDir, prefix, ext, start, end, places));
        FileSet* pOutFiles = new FileSet(new FilePattern(outDir, "NewReg-" + prefix, ext, start, end, places));

        Logger::logInfo("Setting up registration processor.");
        GlobalRegistrationPipeline *pipeline = new GlobalRegistrationPipeline();
        pipeline->SetInput(pInFiles);
        pipeline->SetOutput(pOutFiles);
        pipeline->SetSmoothSigma(5.0);

        Logger::logInfo("Running registration.");
        GlobalRegistrationPipeline::TransformVector* trans = pipeline->UpdateAll();

        Logger::logInfo("Saving transform data.");
        pipeline->SetTransformFile(outDir + prefix + "-Transforms.txt");
        pipeline->SaveTransforms();

        Logger::logInfo("Saving transformed images");
        pipeline->SaveImages();

        succeed_();
    }
    catch (itk::ExceptionObject &error)
    {
        std::cout << "Exception object caught!" << std::endl;
        std::cout << error << std::endl;
        fail_("itkExceptionObject");
    }
}