#ifndef TRANSFORM_GROUP_TEST
#define TRANSFORM_GROUP_TEST

#include <string>

#include "itkCenteredRigid2DTransform.h"

#include "../TestSuite/Test.h"
#include "../TransformGroup.h"

class TestTransformGroup : public TestSuite::Test
{
public:
    typedef itk::CenteredRigid2DTransform<double> TranType;

    TestTransformGroup();
    void run();
    void testSaveTransforms();
    void testLoadTransforms();

private:
    std::string* fileName;
};

#endif
