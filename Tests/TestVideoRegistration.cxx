#include "TestVideoRegistration.h"

#include <string>
#include <iostream>

#include "itkExceptionObject.h"

#include "BlurFilterSchedule.h"
#include "FileSet.h"
#include "ImageRegistration.h"
#include "MultiImageRegistrationProcessor.h"
#include "RegistrationOutput.h"
#include "TransformGroup.h"

TestVideoRegistration::TestVideoRegistration(void)
{
}

TestVideoRegistration::~TestVideoRegistration(void)
{
}

void TestVideoRegistration::run()
{
    this->testUpdate();
}

void TestVideoRegistration::testUpdate()
{
    try
    {
        std::cout << "Setting up IO files." << std::endl;
        std::string inDir = "C:\\UNC CS\\Research\\Nano\\data\\Lin\\";
        std::string outDir = "C:\\UNC CS\\Research\\Nano\\data\\Lin\\out\\";

        std::string prefix = "MovingStageSS";
        std::string ext = ".tif";
        int start = 1;
        int end = 2;
        int places = 1;
        FileSet* pInFiles = new FileSet(new FilePattern(inDir, prefix, ext, start, end, places));
        FileSet* pOutFiles = new FileSet(new FilePattern(outDir, "Blur-" + prefix, ext, start, end, places));

        std::cout << "Setting up registrar." << std::endl;
        ImageRegistration* pRegistrar = new ImageRegistration();
        double smooth = 4.0;
        pRegistrar->SetBlurSchedule(new BlurFilterSchedule(smooth, smooth, 1, 2.5 * smooth));

        std::cout << "Setting up registration processor." << std::endl;
        MultiImageRegistrationProcessor* pProc = new MultiImageRegistrationProcessor();
        pProc->SetSource(pInFiles);
        pProc->SetImageRegistration(pRegistrar);
        pProc->SetAlignmentMode(MultiImageRegistrationProcessor::ALIGN_WITH_PREVIOUS);

        std::cout << "Running registration." << std::endl;
        MultiImageRegistrationProcessor::TransformVector* pTrans = pProc->Update();

        std::cout << "Saving transform data." << std::endl;
        std::string outTran = outDir + "Transforms.txt";
        TransformGroup::SaveTransforms(pTrans, outTran);

        std::cout << "Saving registered images." << std::endl;
        RegistrationOutput* pRegOut = new RegistrationOutput(pInFiles, pTrans, pOutFiles, RegistrationOutput::COMPOSE_PRE);
        pRegOut->Update();

        succeed_();
    }
    catch (itk::ExceptionObject &err)
    {
        std::cout << "Exception object caught!" << std::endl;
        std::cout << err << std::endl;
        fail_("itkExceptionObject");
    }

}