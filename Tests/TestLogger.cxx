#include "TestLogger.h"
#include "Logger.h"

TestLogger::TestLogger(void)
{
}

TestLogger::~TestLogger(void)
{
}

void TestLogger::run()
{
    this->testStreamLogger();
}

void TestLogger::testStreamLogger()
{
    Logger::logVerbose("Verbose message");
    Logger::logDebug("Debug message");
    Logger::logInfo("Info message");
    Logger::logWarn("Warn message");
    Logger::logError("Error message");
    succeed_();

    Logger::logInfo("Setting log level to warning");
	LogLevel level = Logger::getLevel();
	Logger::setLevel(Warning);
    Logger::logVerbose("Verbose message");
    Logger::logDebug("Debug message");
    Logger::logInfo("Info message");
    Logger::logWarn("Warn message");
    Logger::logError("Error message");

	// Reset
	Logger::setLevel(level);
	succeed_();
}
