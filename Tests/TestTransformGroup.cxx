
#include "TestTransformGroup.h"

TestTransformGroup::TestTransformGroup()
{
    this->fileName = new std::string();
    this->fileName->append("TestTG.dat");
}

void TestTransformGroup::run()
{
    this->testSaveTransforms();
    this->testLoadTransforms();
}

void TestTransformGroup::testSaveTransforms()
{
    TranType::Pointer xForm1 = TranType::New();
    TranType::Pointer xForm2 = TranType::New();
    TranType::Pointer xForm3 = TranType::New();
    
    // First transform
    TranType::ParametersType* params = new TranType::ParametersType(xForm1->GetNumberOfParameters());
    params->put(0, 0.0);
    params->put(1, 11.0);
    params->put(2, 12.0);
    params->put(3, 13.0);
    params->put(4, 14.0);

    xForm1->SetIdentity();
    xForm1->SetParameters(*params);
    xForm1->SetAngleInDegrees(10.0);

    // Second transform
    params = new TranType::ParametersType(xForm2->GetNumberOfParameters());
    params->put(0, 0.0);
    params->put(1, 21.0);
    params->put(2, 22.0);
    params->put(3, 23.0);
    params->put(4, 24.0);

    xForm2->SetIdentity();
    xForm2->SetParameters(*params);
    xForm2->SetAngleInDegrees(20.0);

    // Third transform
    params = new TranType::ParametersType(xForm3->GetNumberOfParameters());
    params->put(0, 0.0);
    params->put(1, 31.0);
    params->put(2, 32.0);
    params->put(3, 33.0);
    params->put(4, 34.0);

    xForm3->SetIdentity();
    xForm3->SetParameters(*params);
    xForm3->SetAngleInDegrees(30.0);

    TransformGroup::TransformVector* pTranVec = new TransformGroup::TransformVector();
    pTranVec->push_back((TransformGroup::TransformPointer) xForm1);
    pTranVec->push_back((TransformGroup::TransformPointer) xForm2);
    pTranVec->push_back((TransformGroup::TransformPointer) xForm3);

    // Save
    TransformGroup::SaveTransforms(pTranVec, *(this->fileName));

    // If we've gotten this far, with no errors, the file has written...the Load
    // function tests the contents.
    this->succeed_();
}

void TestTransformGroup::testLoadTransforms()
{
    TransformGroup::TransformVector* pTranVec = TransformGroup::LoadTransforms(*(this->fileName));
    test_(pTranVec->size() == 3);
    TransformGroup::TransformVector::iterator tranIt = pTranVec->begin();

    int count = 0;
    while (tranIt != pTranVec->end())
    {
        count++;
        TransformGroup::TransformPointer pTran = *tranIt;
        TransformGroup::TransformType::ParametersType params = pTran->GetParameters();
        for (int i=0; i < params.GetNumberOfElements(); i++)
        {
            if (i != 0)
                test_(params.get(i) == (double) (count * 10 + i));
            // @todo test for angle == 10 * count degrees, in radians.
        }      

        tranIt++;
    }   
}