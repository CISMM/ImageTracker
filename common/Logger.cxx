#include "Logger.h"

LogLevel Logger::level(All);

LogStream Logger::error(Error,      "ERROR:    ");
LogStream Logger::warning(Warning,  "WARNING:  ");
LogStream Logger::info(Info,        "INFO:     ");
LogStream Logger::debug(Debug,      "DEBUG:    ");
LogStream Logger::verbose(Verbose,  "VERBOSE:  ");

StreamRedirector* Logger::redirector = NULL;

ToggleLogStream LogStream::toggleStream(true);

void Logger::logError(const std::string msg)    { Logger::error     << msg << std::endl; }
void Logger::logWarning(const std::string msg)  { Logger::warning   << msg << std::endl; }
void Logger::logWarn(const std::string msg)     { Logger::warning   << msg << std::endl; }
void Logger::logInfo(const std::string msg)     { Logger::info      << msg << std::endl; }
void Logger::logDebug(const std::string msg)    { Logger::debug     << msg << std::endl; }
void Logger::logVerbose(const std::string msg)  { Logger::verbose   << msg << std::endl; }

Logger::Logger()
{
}

Logger::~Logger()
{
    if (Logger::redirector)
        delete Logger::redirector;
}

void Logger::redirect(std::ostream& target)
{
    if (!Logger::redirector)
        Logger::redirector = new StreamRedirector(std::cout, target);
    else
        Logger::redirector->setTarget(target);
}

void Logger::undirect()
{
    if (Logger::redirector)
        delete Logger::redirector;
    Logger::redirector = NULL;
}
