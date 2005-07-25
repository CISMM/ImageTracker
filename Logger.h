#pragma once
#include <string>
#include <iostream>

enum
{
    LOG_NONE = 1000,
    LOG_ERROR,
    LOG_WARN,
    LOG_INFO,
    LOG_DEBUG,
    LOG_VERBOSE,
    LOG_ALL
};

const static int LOG_LEVEL = LOG_INFO;

class ILog;

/*
 * A singleton class that provides logging methods.  The class
 * forwards logging information to a single ILog instance.  Logging can
 * occur at multiple levels, based on importance of the message.
 * These levels include: errors, warnings, info, debug, and verbose.
 */
class Logger
{
public:
    ~Logger(void);

    /*
     * Log a verbose message.
     */
    static void logVerbose(const std::string &msg);
    
    /*
     * Log a debug message.
     */
    static void logDebug(const std::string &msg);

    /*
     * Log an informational message.
     */
    static void logInfo(const std::string &msg);

    /*
     * Log a warning message.
     */
    static void logWarn(const std::string &msg);

    /*
     * Log an error message.
     */
    static void logError(const std::string &msg);

    /*
     * Get the current logging level.
     */
    static int getLevel();

    /*
     * Set the logging level to use.
     */
    static void setLevel(int level);

    /*
     * Determines whether the current logger is visible.  (Only applies to window-based loggers.)
     */
    static bool isVisible();

    /*
     * Sets whether the current logger is visible.  (Only applies to window-based loggers.)
     */
    static void setVisible(bool vis);
    
    /*
     * Destroy the singleton Logger instance and the associated ILog instance.
     */
    static void Destroy();

    /*
     * Sets the ILog instance to use as a logging target.
     */
    static void SetLogger(ILog* logger);

protected:
    /*
     * Instance grabbing method--allows access to the singleton Logger.
     */
    static Logger* Instance();

    /*
     * Logger constructor.  Protected so instances cannot be created.
     */
    Logger(int level=LOG_LEVEL);

    /* 
     * Retrieves the ILog instance that this Logger controls.
     */
    ILog* GetILog();

    /*
     * Sets the ILog instance that this Logger controls.
     */
    void SetILog(ILog* logger);

private:  
    static Logger* s_instance;
    static int s_level;
    ILog* theLogger;
};

/*
 * Interface which all logging classes must implement.  Allows for
 * logging at different levels, and controlling the output of the
 * logger.  This interface is easy to extend--one only needs to specify
 * how logging is performed and how an ILog is destroyed.
 */
class ILog
{
public:
    virtual ~ILog() {}
    void logVerbose(const std::string &msg) {this->doLog(LOG_VERBOSE, "VERBOSE: " + msg); }
    void logDebug(const std::string &msg) { this->doLog(LOG_DEBUG, "DEBUG:   " + msg); }
    void logInfo(const std::string &msg) { this->doLog(LOG_INFO, "INFO:    " + msg); }
    void logWarn(const std::string &msg) { this->doLog(LOG_WARN, "WARN:    " + msg); }
    void logError(const std::string &msg) { this->doLog(LOG_ERROR, "ERROR:   " + msg); }
    bool isVisible() { return this->visible; }
    void setVisible(bool vis) { this->visible = vis; }
protected:
    void doLog(int level, const std::string &msg) { if (level <= Logger::getLevel()) this->doLog(msg); }

    /*
     * Log a message.  Implementation classes must define this method to determine how messages are actually logged.
     */
    virtual void doLog(const std::string &msg) = 0;
private:
    bool visible;
};

/*
 * A default logger that logs to an output stream.
 */
class StreamLogger : public ILog
{
public:
    /*
     * Default constructor.  Specifies output to std::cout by default.
     */
    StreamLogger(std::ostream* stream = &std::cout);

    /*
     * Destructor.
     */
    ~StreamLogger();

protected:
    void doLog(const std::string &msg);

private:
    std::ostream* stream;
};