#include "Logger.h"
#include "Mutex.h"

LogLevel Logger::level(All);

LogStream Logger::error(Error,      "ERROR:    ");
LogStream Logger::warning(Warning,  "WARNING:  ");
LogStream Logger::info(Info,        "INFO:     ");
LogStream Logger::debug(Debug,      "DEBUG:    ");
LogStream Logger::verbose(Verbose,  "VERBOSE:  ");

StreamRedirector* Logger::redirector = NULL;

Mutex ToggleLogStream::mutex;
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

ToggleLogStream& ToggleLogStream::operator<<(std::ostream& (*op)(std::ostream&))
{
    if (this->enabled)
    {
        MutexLocker lock(this->mutex);
        (*op)(std::cout);
    }
    return *this;
}

void ToggleLogStream::Enable()
{ this->enabled = true; }

void ToggleLogStream::Disable()
{ this->enabled = false; }

void ToggleLogStream::SetEnabled(bool enable)
{ this->enabled = enable; }

bool ToggleLogStream::GetEnabled()
{ return this->enabled; }
