#pragma once

#include <string>
#include <iostream>

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
class StreamRedirector;

/**
 * Logger provides access to the logging system. It allows you to write
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

    /*
     * Redirect logger output.  Users must call undirect() before
     * the target ostream is destroyed.
     */
    static void redirect(std::ostream& target);

    /*
     * Stop redirection of output.
     */
    static void undirect();

protected:
    Logger();
    virtual ~Logger();

    static StreamRedirector* redirector;
};

/**
* LogStream handles streaming of data to output streams. A LogStream
* has an associated LogLevel that controls whether output is generated.
*/
class LogStream
{
public:
    LogStream(LogLevel level = Error) : level(level) {}
    virtual ~LogStream() {}

    /*
    * Stream arbitrary types and values.
    */
    template <typename T>
        LogStream& operator<<(const T& val)
    {
        if (this->level <= Logger::getLevel())
            std::cout << val;
        return *this;
    }

    /*
     * Handle stream manipulators. Borrowed heavily from:
     * Josuttis, Nicolai. The C++ Standard Library.
     * Addison-Wesley, New York, 2005, p. 612.
     */
    virtual LogStream& operator<<(std::ostream& (*op)(std::ostream&))
    {
        if (this->level <= Logger::getLevel())
            (*op)(std::cout);
        return *this;
    }

protected:
    LogLevel level;
    // TODO: Add support for redirecting output to files or other streams...
};

/*
 * Redirects a source ostream to a target ostream. Redirection continues
 * until the StreamRedirector is destroyed.
 */
class StreamRedirector
{
public:
    StreamRedirector(std::ostream& source, std::ostream& target)
    {
        this->target = NULL;
        this->originalStream = &source;
        this->originalBuffer = source.rdbuf();
        this->setTarget(target);
    }
    virtual ~StreamRedirector()
    {
        this->originalStream->rdbuf(this->originalBuffer);
    }

    void setTarget(std::ostream& oTarget)
    {
        if (this->target)
            this->target->flush();

        this->target = &oTarget;
        this->target->copyfmt(*this->originalStream);
        this->originalStream->rdbuf(this->target->rdbuf());
    }

protected:
    std::streambuf* originalBuffer;
    std::ostream* originalStream;
    std::ostream* target;
};
