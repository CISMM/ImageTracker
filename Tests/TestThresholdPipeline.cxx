#include "TestThresholdPipeline.h"

#include <string>
#include "itkExceptionObject.h"

#include "FilePattern.h"
#include "FileSet.h"
#include "ThresholdPipeline.h"

TestThresholdPipeline::TestThresholdPipeline(void)
{
}

TestThresholdPipeline::~TestThresholdPipeline(void)
{
}

void TestThresholdPipeline::run()
{
    this->testUpdate();
}

void TestThresholdPipeline::testUpdate()
{
    try
    {
        std::string inDir = "C:\\UNC CS\\Research\\Nano\\data\\Lin\\";
        std::string outDir = "C:\\UNC CS\\Research\\Nano\\data\\Lin\\out\\";

        std::string prefix = "MovingStageSS";
        FileSet* pInFiles = new FileSet(new FilePattern(inDir, prefix, ".png", 1, 6, 1));
        FileSet* pOutFiles = new FileSet(new FilePattern(outDir, "Thresh-" + prefix, ".png", 1, 6, 1));

        ThresholdPipeline::PixelType min = 100;
        ThresholdPipeline::PixelType max = 255;

        ThresholdPipeline* pThresh = new ThresholdPipeline();
        pThresh->SetInputFiles(pInFiles);
        pThresh->SetOutputFiles(pOutFiles);
        pThresh->ThresholdBelow(min);
        // pThresh->ThresholdAbove(max);
        pThresh->Update();
        succeed_();
    }
    catch (itk::ExceptionObject &err)
    {
        std::cout << "Exception object caught!" << std::endl;
        std::cout << err << std::endl;
        fail_("itkExceptionObject");
    }
}