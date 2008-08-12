#pragma once

#include <string>
#include <iostream>

#include "Mutex.h"
#include "MutexLocker.h"

enum LogLevel {
    None    = 0,
    Error   = 100,
    Warning = 200,
    Info    = 300,
    Debug   = 400,
    Verbose = 500,
    All     = 600
};

// Forward delcaration of LogStream
class LogStream;

/**
 * \class Logger
 * \brief A multi-level, streaming logger.
 * 
 * Logger provides access to the logging system. It enables writing
 * application messages of varying levels to output streams. The general
 * format for messaging follows std c++ streaming:
 * Logger::error << "Application error!" << std::endl;
 */
class Logger
{
public:
    static LogLevel getLevel() { return Logger::level; }
    static void setLevel(LogLevel level) { Logger::level = level; }

    // Level of logging currently active.
    static LogLevel level;

    // LogStreams...use them, but don't mess with them.
    static LogStream error;
    static LogStream warning;
    static LogStream info;
    static LogStream debug;
    static LogStream verbose;

    // Logger functions
    static void logError(const std::string msg);
    static void logWarning(const std::string msg);
    static void logWarn(const std::string msg);
    static void logInfo(const std::string msg);
    static void logDebug(const std::string msg);
    static void logVerbose(const std::string msg);
    
    static void SetStream(std::ostream& stream);

protected:
    Logger();
    virtual ~Logger();
};

/**
 * \class ToggleLogStream
 * \brief A LogStream that can be toggled on and off.
 * 
 * A logging class that can be turned on and off (enabled or disabled).
 * ToggleLogStream is thread-safe.  ToggleLogStream can stream to any
 * arbitrary std::ostream object; the default is std::cout.
 */
class ToggleLogStream
{
public:
    ToggleLogStream(bool enable = true, std::ostream& stream = std::cout);
    virtual ~ToggleLogStream();

    /**
     * Stream arbitrary types and values.
     */
    template <typename T>
    ToggleLogStream& operator<<(const T& val)
    {
        if (this->enabled)
        {
                MutexLocker lock(this->mutex);
                (*this->stream) << val;
        }
        return *this;
    }

    /**
     * Handle stream manipulators. Borrowed heavily from:
     * Josuttis, Nicolai. The C++ Standard Library.
     * Addison-Wesley, New York, 2005, p. 612.
     */
    virtual ToggleLogStream& operator<<(std::ostream& (*op)(std::ostream&));

    /** Turn this log stream on. */
    void Enable();
    
    /** Turn this log stream off. */
    void Disable();
    
    /** Turn this log stream on or off. */
    void SetEnabled(bool enable);
    
    /** Determine if this log stream is on or off. */
    bool IsEnabled();
    
    void SetStream(std::ostream& stream);

protected:
    bool enabled;
    std::ostream* stream;

    // A mutex controls access to the log stream; this makes logging
    // tread-safe.
    static Mutex mutex;
private:
};

/**
 * \class LogStream
 * \brief Streams text and other data to output.
 * 
 * LogStream handles streaming of data to output streams. A LogStream
 * has an associated LogLevel that controls whether output is generated.
 * LogStream acts as a gateway to log streaming by appending a standard
 * message to logs and then forwarding logging requests to a ToggleLogStream.
 */
class LogStream
{
public:
    LogStream(LogLevel level = Error, std::string msg = "ERROR: ");
    virtual ~LogStream();
    
    /**
     * Stream arbitrary types and values.
     */
    template <typename T>
    ToggleLogStream& operator<<(const T& val)
    {
        toggleStream.SetEnabled(this->level <= Logger::getLevel());
        return (toggleStream << this->message << val);
    }
    
    virtual ToggleLogStream& operator<<(std::ostream& (*op)(std::ostream&));
    
    ToggleLogStream& GetToggleLogStream();
    
protected:
    LogLevel level;
    std::string message;
    
    // The toggle stream to forward all requests to
    static ToggleLogStream toggleStream;
    
private:
};

