#include "TestFileSetImageReader.h"

#include "..\FilePattern.h"
#include "..\FileSet.h"
#include "..\FileSetImageReader.h"

TestFileSetImageReader::TestFileSetImageReader(void)
{
}

TestFileSetImageReader::~TestFileSetImageReader(void)
{
}

void TestFileSetImageReader::run()
{
    this->testHasNext();
    this->testFileSetNames();
}

void TestFileSetImageReader::testHasNext()
{
    FileSet* pFS = new FileSet(new FilePattern("c:\\test\\", "test", ".txt", 1, 10, 2));
    FileSetImageReader* pIR = new FileSetImageReader(pFS);

    while (pIR->HasNext())
    {
        std::cout << "Filename => " << pIR->NextFileName() << std::endl;
    }

    delete(pFS);
    delete(pIR);
}

void TestFileSetImageReader::testFileSetNames()
{
    FileSet* pFS = new FileSet(new FilePattern("c:\\test\\", "test", ".txt", 1, 10, 2));
    FileSetImageReader* pIR = new FileSetImageReader(pFS);

    std::cout << "True  => " << (0 == 0) << std::endl;
    std::cout << "False => " << (0 == 1) << std::endl;

    std::string exp;
    for (int i = 1; i < 10; i++)
    {
        std::string strI = itoa(i, "", 10);
        exp = "c:\\test\\test0" + strI + ".txt";
        std::cout << "Expected => " << exp << std::endl;
        std::cout << "Filename => " << pIR->CurrentFileName() << std::endl;
        std::cout << "HasNext  => " << pIR->HasNext() << std::endl;
        test_(pIR->HasNext());
        test_(pIR->CurrentFileName() == exp);
        pIR->NextFileName();
    }

    exp = "c:\\test\\test10.txt";
    std::cout << "Expected => " << exp << std::endl;
    std::cout << "Filename => " << pIR->CurrentFileName() << std::endl;
    std::cout << "HasNext  => " << pIR->HasNext() << std::endl;
    test_(!(pIR->HasNext()));
    test_(pIR->CurrentFileName() == exp);

    delete(pFS);
    delete(pIR);
}