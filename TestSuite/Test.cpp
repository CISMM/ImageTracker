//: TestSuite:Test.cpp {O}
#include "Test.h"
#include <iostream>
#include <typeinfo>  // Note: Visual C++ requires /GR
using namespace std;
using namespace TestSuite;

void Test::do_test(bool cond,
  const std::string& lbl, const char* fname,
  long lineno) {
  if (!cond)
    do_fail(lbl, fname, lineno);
  else
    succeed_();
}

void Test::do_fail(const std::string& lbl,
  const char* fname, long lineno) {
  ++nFail;
  if (osptr) {
    *osptr << typeid(*this).name()
           << "failure: (" << lbl << ") , "
           << fname
           << " (line " << lineno << ")\n";
  }
}

long Test::report() const {
  if (osptr) {
    *osptr << "Test \"" << typeid(*this).name()
           << "\":\n\tPassed: " << nPass
           << "\tFailed: " << nFail
           << endl;
  }
  return nFail;
} ///:~
