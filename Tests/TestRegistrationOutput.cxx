#include "TestRegistrationOutput.h"

#include "../FileSet.h"
#include "../RegistrationOutput.h"
#include "../TransformGroup.h"

TestRegistrationOutput::TestRegistrationOutput(void)
{
}

TestRegistrationOutput::~TestRegistrationOutput(void)
{
}

void TestRegistrationOutput::run()
{
    this->testUpdate();
}

void TestRegistrationOutput::testUpdate()
{
    try
    {
        std::string inDir  = "C:\\UNC CS\\Research\\nano\\data\\Goldstein\\060404 OD3a let99RNAi 1c 3\\";
        std::string outDir = "C:\\UNC CS\\Research\\nano\\data\\Goldstein\\out\\060404 OD3a let99RNAi 1c 3\\";

        std::string prefix = "060404let991c3";
        std::string ext = ".png";
        int start = 7;
        int end = 117;
        int places = 3;
        FileSet* pInFiles = new FileSet(new FilePattern(inDir, prefix, ext, start, end, places));
        FileSet* pOutFiles = new FileSet(new FilePattern(outDir, "Trans-" + prefix, ext, start, end, places));

        std::string inTran = "C:\\UNC CS\\Research\\nano\\data\\Goldstein\\out\\060404 OD3a let99RNAi 1c 3\\Transforms.xfs";
        TransformGroup::TransformVector* transforms = TransformGroup::LoadTransforms(inTran);

        RegistrationOutput* pReg = new RegistrationOutput(pInFiles, transforms, pOutFiles, RegistrationOutput::COMPOSE_PRE);
        pReg->Update();
        succeed_();
    }
    catch (itk::ExceptionObject &err)
    {
        std::cout << "Exception object caught!" << std::endl;
        std::cout << err << std::endl;
        fail_("itkExceptionObject");
    }
}