#include <fstream>
#include <iostream>
#include <string>

#include "Logger.h"

void TestLevel(std::string name, LogLevel level)
{
    Logger::setLevel(level);
    std::cout << "*** Logging level: " << name << " ***" << std::endl;
    Logger::error << "This is an error message" << std::endl;
    Logger::warning << "This is an warning message" << std::endl;
    Logger::info << "This is an info message" << std::endl;
    Logger::debug << "This is an debug message" << std::endl;
    Logger::verbose << "This is an verbose message" << std::endl;
}

int main(int argc, char** argv)
{
    TestLevel("None", None);
    TestLevel("Error", Error);
    TestLevel("Warning", Warning);
    TestLevel("Info", Info);
    TestLevel("Debug", Debug);
    TestLevel("Verbose", Verbose);
    TestLevel("All", All);
    
    std::string filename("log.txt");
    std::cout << "*** Redirecting to " << filename << " ***" << std::endl;
    
    std::ofstream logfile("log.txt");
    Logger::SetStream(logfile);
    
    TestLevel("None", None);
    TestLevel("Error", Error);
    TestLevel("Warning", Warning);
    TestLevel("Info", Info);
    TestLevel("Debug", Debug);
    TestLevel("Verbose", Verbose);
    TestLevel("All", All);
    
    std::cout << "*** Redirecting to std::cout ***" << std::endl;
    Logger::SetStream(std::cout);
    
    TestLevel("None", None);
    TestLevel("Error", Error);
    TestLevel("Warning", Warning);
    TestLevel("Info", Info);
    TestLevel("Debug", Debug);
    TestLevel("Verbose", Verbose);
    TestLevel("All", All);
    
    logfile.close();

}
