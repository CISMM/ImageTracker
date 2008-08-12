#include "Logger.h"
#include "Mutex.h"

LogLevel Logger::level(All);

LogStream Logger::error(Error,      "ERROR:    ");
LogStream Logger::warning(Warning,  "WARNING:  ");
LogStream Logger::info(Info,        "INFO:     ");
LogStream Logger::debug(Debug,      "DEBUG:    ");
LogStream Logger::verbose(Verbose,  "VERBOSE:  ");

Mutex ToggleLogStream::mutex;
ToggleLogStream LogStream::toggleStream(true);

void Logger::logError(const std::string msg)    { Logger::error     << msg << std::endl; }
void Logger::logWarning(const std::string msg)  { Logger::warning   << msg << std::endl; }
void Logger::logWarn(const std::string msg)     { Logger::warning   << msg << std::endl; }
void Logger::logInfo(const std::string msg)     { Logger::info      << msg << std::endl; }
void Logger::logDebug(const std::string msg)    { Logger::debug     << msg << std::endl; }
void Logger::logVerbose(const std::string msg)  { Logger::verbose   << msg << std::endl; }

Logger::Logger()
{}

Logger::~Logger()
{}

void Logger::SetStream(std::ostream& stream)
{
    // Setting the stream on one LogStream changes all LogStreams.
    Logger::error.GetToggleLogStream().SetStream(stream);
}

//--- ToggleLogStream ---//

ToggleLogStream::ToggleLogStream(bool enable, std::ostream& stream)
    : enabled(enable)
{
    this->stream = &stream;
}

ToggleLogStream::~ToggleLogStream()
{}

ToggleLogStream& ToggleLogStream::operator<<(std::ostream& (*op)(std::ostream&))
{
    if (this->enabled)
    {
        MutexLocker lock(this->mutex);
        (*op)(*this->stream);
    }
    return *this;
}

void ToggleLogStream::Enable()
{ this->enabled = true; }

void ToggleLogStream::Disable()
{ this->enabled = false; }

void ToggleLogStream::SetEnabled(bool enable)
{ this->enabled = enable; }

bool ToggleLogStream::IsEnabled()
{ return this->enabled; }

void ToggleLogStream::SetStream(std::ostream& stream)
{
    this->stream = &stream;
}

//--- LogStream ---//

LogStream::LogStream(LogLevel level, std::string msg) 
    : level(level),
      message(msg)
{}
                
LogStream::~LogStream() 
{}

ToggleLogStream& LogStream::operator<<(std::ostream& (*op)(std::ostream&))
{
    toggleStream.SetEnabled(this->level <= Logger::getLevel());
    return (toggleStream << this->message << (*op));
}

ToggleLogStream& LogStream::GetToggleLogStream()
{
    return LogStream::toggleStream;
}
