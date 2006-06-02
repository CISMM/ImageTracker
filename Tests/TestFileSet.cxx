#include "TestFileSet.h"

#include <string>

#include "FileSet.h"
#include "FilePattern.h"

TestFileSet::TestFileSet(void)
{
}

TestFileSet::~TestFileSet(void)
{
}

void TestFileSet::run()
{
    this->testCreateFileSet();
}

void TestFileSet::testCreateFileSet()
{
    std::string dir = "C:\\TestDir\\";
    FilePattern* pPat = new FilePattern(dir, "test", ".ext", 0, 10, 3);
    FileSet* pSet = new FileSet(pPat);

    FileSet::FileVector* vec = pSet->GetFileNames();
    FileSet::FileIterator filesIt = vec->begin();

    test_(filesIt->size() == 11);

    for ( ; filesIt != vec->end(); *filesIt++)
    {
        std::string name = pSet->GetDirectory() + *filesIt;
        std::cout << "File: " << name << std::endl;
    }
}