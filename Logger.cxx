#include ".\Logger.h"

Logger* Logger::s_instance = (Logger *) NULL;
int Logger::s_level = LOG_LEVEL;

Logger::Logger(int level)
{
    this->theLogger = new StreamLogger();
    Logger::setLevel(level);
}

Logger::~Logger(void)
{
    if (this->theLogger)
        delete theLogger;
}

void Logger::Destroy()
{
    if (Logger::s_instance)
        delete Logger::s_instance;
}

Logger* Logger::Instance()
{
    if (Logger::s_instance == NULL)
    {
        Logger::s_instance = new Logger();
    }

    return s_instance;
}

void Logger::SetLogger(ILog* logger)
{
    Logger::Instance()->SetILog(logger);
}

ILog* Logger::GetILog()
{
    return this->theLogger;
}

void Logger::SetILog(ILog* logger)
{
    ILog* old = this->theLogger;
    this->theLogger = logger;
    delete old;
}

void Logger::logVerbose(const std::string &msg)
{
    Logger::Instance()->theLogger->logVerbose(msg);
}

void Logger::logInfo(const std::string &msg)
{
    Logger::Instance()->theLogger->logInfo(msg);
}

void Logger::logDebug(const std::string &msg)
{
    Logger::Instance()->theLogger->logDebug(msg);
}

void Logger::logWarn(const std::string &msg)
{
    Logger::Instance()->theLogger->logWarn(msg);
}

void Logger::logError(const std::string &msg)
{
    Logger::Instance()->theLogger->logError(msg);
}

void Logger::setLevel(int level)
{
    Logger::s_level = level;
}

int Logger::getLevel()
{
    return Logger::s_level;
}

bool Logger::isVisible()
{
    return Logger::Instance()->theLogger->isVisible();
}

void Logger::setVisible(bool vis)
{
    Logger::Instance()->theLogger->setVisible(vis);
}

StreamLogger::StreamLogger(std::ostream* stream)
{
    this->stream = stream;
    this->setVisible(true);
}

StreamLogger::~StreamLogger()
{
    this->stream = (std::ostream *) NULL;
}

void StreamLogger::doLog(const std::string &msg)
{
    (*this->stream) << msg << std::endl;
}