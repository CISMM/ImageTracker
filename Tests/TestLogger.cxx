#include ".\TestLogger.h"
#include "..\Logger.h"

TestLogger::TestLogger(void)
{
}

TestLogger::~TestLogger(void)
{
}

void TestLogger::run()
{
    this->testStreamLogger();
    this->testDelete();
}

void TestLogger::testStreamLogger()
{
    Logger::logVerbose("Verbose message");
    Logger::logDebug("Debug message");
    Logger::logInfo("Info message");
    Logger::logWarn("Warn message");
    Logger::logError("Error message");
    succeed_();

    if (Logger::isVisible())
    {
        Logger::logInfo("Logger visible");
    }
    else
    {
        Logger::logInfo("Logger invisible");
    }


    Logger::logInfo("Setting log level to warning");
    Logger::setLevel(LOG_WARN);
    Logger::logVerbose("Verbose message");
    Logger::logDebug("Debug message");
    Logger::logInfo("Info message");
    Logger::logWarn("Warn message");
    Logger::logError("Error message");
}

void TestLogger::testDelete()
{
    Logger::setLevel(LOG_INFO);
    Logger::logInfo("Deleting logger....");
    Logger::Destroy();
}