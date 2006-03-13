#include "Logger.h"

LogLevel Logger::level(All);
LogStream Logger::error(Error);
LogStream Logger::warning(Warning);
LogStream Logger::info(Info);
LogStream Logger::debug(Debug);
LogStream Logger::verbose(Verbose);
StreamRedirector* Logger::redirector = NULL;

void Logger::logError(const std::string msg) { Logger::error << "ERROR:   " << msg << std::endl; }
void Logger::logWarning(const std::string msg) { Logger::warning << "WARN:    " << msg << std::endl; }
void Logger::logWarn(const std::string msg) { Logger::warning << "WARN:    " << msg << std::endl; }
void Logger::logInfo(const std::string msg) { Logger::info << "INFO:    " << msg << std::endl; }
void Logger::logDebug(const std::string msg) { Logger::debug << "DEBUG:   " << msg << std::endl; }
void Logger::logVerbose(const std::string msg) { Logger::verbose << "VERBOSE: " << msg << std::endl; }

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
