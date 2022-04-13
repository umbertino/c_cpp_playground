/**
 * @file Logger.h
 * @author umbertino@mailbox.org
 * @brief A class that provides logging capabilities
 * @date 2022-03-11
 *
 */

#pragma once

// own includes

// std includes
#include <ostream>
#include <iostream>
#include <sstream>
#include <string_view>

// boost includes
#include <boost/lockfree/queue.hpp>
#include <boost/lockfree/spsc_queue.hpp>
#include <boost/chrono.hpp>
#include <boost/thread/thread.hpp>
#include <boost/filesystem/convenience.hpp>

// some convenience macros
#define __BASENAME__ (boost::filesystem::path(__FILE__).filename().string())
#define __FILE_EXT__ (boost::filesystem::path(__FILE__).filename().extension().string())
#define __LOCATION__ __BASENAME__ << ":" << __FUNCTION__ << ":" << __LINE__
#define GMS(inst) inst.userGetMsgStream()

class Logger
{
public:
    /**
     * @brief Enumeration type for log-levels
     *
     */
    typedef enum
    {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERR,
        FATAL
    } LogLevel;

    /**
     * @brief A structure type containing flags to handle the logger's tags
     *
     */
    typedef struct
    {
        bool counter : 1;
        bool date : 1;
        bool timeStamp : 1;
        bool level : 1;
    } LogTag;

    /**
     * @brief An enumeration type to determine the resolution of the timestamp
     *
     */
    typedef enum
    {
        SEC = 0,
        MILI = 1,
        MICRO = 2,
        NANO = 3
    } TimeStampResolution;

    /**
     * @brief determine whether logging is done via console or file
     *
     */
    typedef enum
    {
        CONSOLE,
        FILE
    } LogType;

    /**
     * @brief A structure that describes the raw information for a log message
     *
     */
    typedef struct
    {
        unsigned long counter;
        Logger::LogLevel level;
        boost::chrono::system_clock::time_point now;
        std::string userMessage;
    } RawMessage;

    /**
     * @brief Depending on its fill-level the logqueue has one of three condition colors
     *
     */
    typedef enum
    {
        GREEN,
        ORANGE,
        RED
    } LogQueueColor;

    /**
     * @brief A general information structure representing the fill-level and color condition
     *
     */
    typedef struct
    {
        size_t fillLevel;
        Logger::LogQueueColor condition;
    } LogQueueStatus;

    /**
     * @brief Construct a new Logger object
     *        This is the default constructor for ready to run console logger.
     *        It is possible to change settings and properties via user API
     *
     */
    Logger();

    /**
     * @brief Construct a new Logger object
     *        This constructor allows to instatciate a logger which is configurable
     *        by a ini configfile. It is not possible to change settings and properties
     *        via user API
     *
     *
     * @param configFilename
     */
    Logger(const std::string& configFilename);

    /**
     * @brief Destroy the Logger object
     *
     */
    ~Logger();

    /**
     * @brief User method to start a logger session
     *        On starting all statistics are reset
     *
     * @param instance the logger instance to be started
     * @return std::error_code (0, std::generic_category()) on success
     */
    static std::error_code LOG_START(Logger& instance);

    /**
     * @brief User method to terminate a logger session
     *        On stopping the statistics are logged out
     *        to the configured output channel
     *
     * @param instance the logger instance to be stopped
     * @return std::error_code (0, std::generic_category()) on success
     */
    static std::error_code LOG_STOP(Logger& instance);

    /**
     * @brief User method to suppress/pause logging
     *        All further attempts to log will be ignored
     *        and the statistics value do not alter
     *
     * @param instance the logger instance to be paused
     * @return std::error_code (0, std::generic_category()) on success
     */
    static std::error_code LOG_SUPPRESS(Logger& instance);

    /**
     * @brief User method to resume logging
     *        The logger continues to log messaging
     *        and updates the statistics
     *
     * @param instance the logger instance to be resumed
     * @return std::error_code (0, std::generic_category()) on success
     */
    static std::error_code LOG_RESUME(Logger& instance);

    /**
     * @brief User method to configure predefined log-tags to be prepended to the user log-message
     *        This method can be used only for a default logger
     *
     * @param instance the logger instance that shall be configured regarding log-tags
     * @param logTags the log-tags flags to be set/reset
     * @return std::error_code (0, std::generic_category()) on success
     */
    static std::error_code LOG_SET_TAGS(Logger& instance, Logger::LogTag logTags);

    /**
     * @brief User method to determine the log-severity to be logged
     *        It sets the lowest level, means severity that has to be logged at least.
     *        This method can be used only for a default logger
     *
     * @param instance the logger instance that shall be configured regarding severity level
     * @param level the severity level of type Logger::LogLevel
     *              Note: TRACE < DEBUG < INFO < WARN < ERR < FATAL
     * @return std::error_code (0, std::generic_category()) on success
     */
    static std::error_code LOG_SET_LEVEL(Logger& instance, Logger::LogLevel level);

    /**
     * @brief User method to set the timestamp's resolution
     *        This method can be used only for a default logger
     *
     * @param instance the logger instance that shall be configured regarding timestamp properties
     * @param resolution the resolution of the timestamp to be set
     * @return std::error_code (0, std::generic_category()) on success
     */
    static std::error_code LOG_SET_TIME_STAMP_RESOLUTION(Logger& instance, Logger::TimeStampResolution resolution);

    /**
     * @brief User method to log out a method of severity "TRACE"
     *
     * @param instance the logger instance that shall log a trace-message
     * @param messageStream a message stream that contains the user message to be logged
     *                      Use as follows: messageStream << <your_message>
     *                      The logger instance has a built-in stream which can be used
     *                      instead of providing one.
     * @return std::error_code  (0, std::generic_category()) on success
     */
    static std::error_code LOG_TRACE(Logger& instance, std::ostream& messageStream);

    /**
     * @brief User method to log out a method of severity "DEBUG"
     *
     * @param instance the logger instance that shall log a debug-message
     * @param messageStream a message stream that contains the user message to be logged
     *                      Use as follows: messageStream << <your_message>
     *                      The logger instance has a built-in stream which can be used
     *                      instead of providing one.
     * @return std::error_code  (0, std::generic_category()) on success
     */
    static std::error_code LOG_DEBUG(Logger& instance, std::ostream& messageStream);

    /**
    * @brief User method to log out a method of severity "INFO"
     *
     * @param instance the logger instance that shall log a info-message
     * @param messageStream a message stream that contains the user message to be logged
     *                      Use as follows: messageStream << <your_message>
     *                      The logger instance has a built-in stream which can be used
     *                      instead of providing one.
     * @return std::error_code  (0, std::generic_category()) on success
     */
    static std::error_code LOG_INFO(Logger& instance, std::ostream& messageStream);

    /**
    * @brief User method to log out a method of severity "WARN"
     *
     * @param instance the logger instance that shall log a warning-message
     * @param messageStream a message stream that contains the user message to be logged
     *                      Use as follows: messageStream << <your_message>
     *                      The logger instance has a built-in stream which can be used
     *                      instead of providing one.
     * @return std::error_code  (0, std::generic_category()) on success
     */
    static std::error_code LOG_WARN(Logger& instance, std::ostream& messageStream);

    /**
    * @brief User method to log out a method of severity "ERROR"
     *
     * @param instance the logger instance that shall log a error-message
     * @param messageStream a message stream that contains the user message to be logged
     *                      Use as follows: messageStream << <your_message>
     *                      The logger instance has a built-in stream which can be used
     *                      instead of providing one.
     * @return std::error_code  (0, std::generic_category()) on success
     */
    static std::error_code LOG_ERROR(Logger& instance, std::ostream& messageStream);

    /**
    * @brief User method to log out a method of severity "FATAL"
     *
     * @param instance the logger instance that shall log a fatal-message
     * @param messageStream a message stream that contains the user message to be logged
     *                      Use as follows: messageStream << <your_message>
     *                      The logger instance has a built-in stream which can be used
     *                      instead of providing one.
     * @return std::error_code (0, std::generic_category()) on success
     */
    static std::error_code LOG_FATAL(Logger& instance, std::ostream& messageStream);

    /**
     * @brief User method to query the current log level
     *
     * @param instance the logger instance to be queried regarding log-level
     * @return Logger::LogLevel the current log level set
     */
    static Logger::LogLevel LOG_GET_LEVEL(Logger& instance);

    /**
     * @brief User method to get the built-in stream for the log message
     *
     * @return std::ostream& the built-in user stream
     */
    std::ostream& userGetMsgStream();

    /**
     * @brief Alternative user method for Logger::LOG_START()
     *
     * @return std::error_code (0, std::generic_category()) on success
     */
    std::error_code userStartLog();

    /**
      * @brief Alternative user method for Logger::LOG_STOP()
     *
     * @return std::error_code (0, std::generic_category()) on success
     */
    std::error_code userStopLog();

    /**
     * @brief Alternative user method for Logger::LOG_RESUME()
     *
     * @return std::error_code (0, std::generic_category()) on success
     */
    std::error_code userResumeLog();

    /**
     * @brief Alternative user method for Logger::LOG_SUPPRESS()
     *
     * @return std::error_code (0, std::generic_category()) on success
     */
    std::error_code userSuppressLog();

    /**
     * @brief Alternative user method for Logger::LOG_SET_TAGS
     *
     * @param logTags the log-tags flags to be set/reset
     * @return std::error_code  (0, std::generic_category()) on success
     */
    std::error_code userSetLogTags(Logger::LogTag logTags);

    /**
     * @brief Alternative user method for Logger::LOG_SET_LEVEL
     *
     *@param level the severity level of type Logger::LogLevel
     *             Note: TRACE < DEBUG < INFO < WARN < ERR < FATAL
     * @return std::error_code (0, std::generic_category()) on success
     */
    std::error_code userSetLogLevel(Logger::LogLevel level);

    /**
     * @brief Alternative user method for Logger::LOG_SET_TIME_STAMP_RESOLUTION
     *
     * @param resolution the resolution of the timestamp to be set
     * @return std::error_code (0, std::generic_category()) on success
     */
    std::error_code userSetTimeStampResolution(Logger::TimeStampResolution resolution);

    /**
     * @brief User method to log a message
     *
     * @param level the severity of the message of type Logger::LogLevel
     * @param msg a message stream that contains the user message to be logged
     *              Use as follows: msg << <your_message>
     *              The logger instance has a built-in stream which can be used
     *              instead of providing one.
     * @return std::error_code (0, std::generic_category()) on success
     */
    std::error_code userLog(Logger::LogLevel level, const std::ostream& msg);

    /**
     * @brief Alternative user method for Logger::LOG_GET_LEVEL
     *
     * @return Logger::LogLevel the current log level set
     */
    Logger::LogLevel userGetLogLevel();

private:
    // private static members

    /**
     * @brief Defines the watermark / fill-level of the logger's queue
     *        corresponding to the orange condition in units of percent
     *
     */
    static constexpr unsigned char LOG_MESSAGE_ORANGE_WMARK_PERCENT = 33;

    /**
    * @brief Defines the watermark / fill-level of the logger's queue
     *       corresponding to the red condition in units of percent
     *
     */
    static constexpr unsigned char LOG_MESSAGE_RED_WMARK_PERCENT = 67;

    /**
     * @brief Defines the minimum number that shall be logged into
     *        a log file before opening a new one
     *
     */
    static constexpr unsigned short MIN_LOGS_PER_FILE = 100;

    /**
    * @brief Defines the maximum number that shall be logged into
     *        a log file before opening a new one
     *
     */
    static constexpr unsigned short MAX_LOGS_PER_FILE = 10000;

    /**
     * @brief Defines the size of the logger's message queue
     *
     */
    static constexpr unsigned short LOG_MESSAGE_QUEUE_SIZE = 1024;

    /**
     * @brief Defines the number of messages in the logger's queue
     *        corresponding to the orange condition. It depends on
     *        Logger::LOG_MESSAGE_QUEUE_SIZE
     *
     */
    static constexpr unsigned short LOG_MESSAGE_QUEUE_ORANGE_THRESHLD = (Logger::LOG_MESSAGE_QUEUE_SIZE * LOG_MESSAGE_ORANGE_WMARK_PERCENT) / 100;

    /**
     * @brief Defines the number of messages in the logger's queue
     *        corresponding to the red condition. It depends on
     *        Logger::LOG_MESSAGE_QUEUE_SIZE
     *
     */
    static constexpr unsigned short LOG_MESSAGE_QUEUE_RED_THRESHLD = (Logger::LOG_MESSAGE_QUEUE_SIZE * LOG_MESSAGE_RED_WMARK_PERCENT) / 100;

    /**
     * @brief The binary logarithm corresponding to the minimum log-thread period
     *
     */
    static constexpr unsigned short LOG_MIN_THREAD_PERIOD_LD = 3;

    /**
     * @brief The minimum log-thread period in units of microseconds
     *
     */
    static constexpr unsigned long LOG_MIN_THREAD_PERIOD_US = 1 << Logger::LOG_MIN_THREAD_PERIOD_LD;

    /**
     * @brief The binary logarithm corresponding to the maximum log-thread period
     *
     */
    static constexpr unsigned short LOG_MAX_THREAD_PERIOD_LD = 18;

    /**
     * @brief The maximum log-thread period in units of microseconds
     *
     */
    static constexpr unsigned long LOG_MAX_THREAD_PERIOD_US = 1 << Logger::LOG_MAX_THREAD_PERIOD_LD;

    /**
     * @brief The binary logarithm corresponding to the default log-thread period
     *
     */
    static constexpr unsigned char LOG_DEFAULT_THREAD_PERIOD_LD = 10;

    /**
     * @brief The default log-thread period in units of microseconds
     *
     */
    static constexpr unsigned long DEFAULT_THREAD_PERIOD_US = 1 << Logger::LOG_DEFAULT_THREAD_PERIOD_LD;

    /**
     * @brief The log-thread period corresponding to a green log-queue level
     *
     */
    static constexpr unsigned long LOG_GREEN_THREAD_PERIOD_US = Logger::LOG_MAX_THREAD_PERIOD_US;

    /**
     * @brief The log-thread period corresponding to an orange log-queue level
     *
     */
    static constexpr unsigned long LOG_ORANGE_THREAD_PERIOD_US = Logger::DEFAULT_THREAD_PERIOD_US;

    /**
     * @brief The log-thread period corresponding to a red log-queue level
     *
     */
    static constexpr unsigned long LOG_RED_THREAD_PERIOD_US = Logger::LOG_MIN_THREAD_PERIOD_LD;

    /**
     * @brief The minimum number of log messages that shall be logged in one log-thread cycle
     *
     */
    static constexpr unsigned short LOG_MIN_LOGS_AT_ONCE = 1;

    /**
     * @brief The maximum number of log messages that shall be logged in one log-thread cycle
     *
     */
    static constexpr unsigned short LOG_MAX_LOGS_AT_ONCE = Logger::LOG_MESSAGE_QUEUE_SIZE >> 2;

    /**
     * @brief The default number of log messages that shall be logged in one log-thread cycle
     *
     */
    static constexpr unsigned short LOG_DEFAULT_LOGS_AT_ONCE = Logger::LOG_MAX_LOGS_AT_ONCE >> 1;

    /**
     * @brief A translater from Logger::LogLevel to a string
     *
     */
    static constexpr std::string_view LOG_LEVEL_2_STRING[6] = {"TRACE", "DEBUG", "INFO ", "WARN ", "ERROR", "FATAL"};

    /**
     * @brief A stream used to discard or empty other message streams
     *
     */
    static std::ostream nirvana;

    /**
     * @brief Returns date string based on a time point
     *
     * @param now
     * @return std::string
     */
    inline static std::string getDateStr(boost::chrono::system_clock::time_point now);

    /**
     * @brief Returns a timestamp string based on a time point and desired resolution
     *
     * @param now the timepoint to be considered
     * @param resolution the desired resolution of the timestamp
     * @return std::string the desired timestamp
     */
    inline static std::string getTimeStr(boost::chrono::system_clock::time_point now, Logger::TimeStampResolution resolution);

    /**
     * @brief Represents the log-tags configured. Can be queried using Logger::LogTag
     *
     */
    Logger::LogTag logTags;

    /**
     * @brief The point in time the logger was started
     *
     */
    boost::chrono::system_clock::time_point loggerStartTime;

    /**
     * @brief The point in time the logger was stopped
     *
     */
    boost::chrono::system_clock::time_point loggerStopTime;

        /**
     * @brief Represents the timestamp's resolution configured.
     *
     */
        Logger::TimeStampResolution timeStampResolution;

    /**
     * @brief Represents the number of logs configured per log-file
     *
     */
    unsigned short logsPerFile;

    /**
     * @brief Represents the current log-file number beeing written (1 .... n)
     *
     */
    unsigned short logFileCounter;

    /**
     * @brief Represents the number of log messages that have been written
     *        successfully into the logger's queue
     *
     */
    unsigned long logInCounter;

    /**
     * @brief Represents the number of log messages that have been
     *        successfully written out of the logger's queue
     *        to the log-channel (file or console)
     *
     */
    unsigned long logOutCounter;

    /**
     * @brief Represents the number of messages that could not be written into
     *        the logger's queue. Only relevant if the queue's overload policy
     *        is configured to discard
     *
     */
    unsigned long logDiscardCounter;

    /**
     * @brief Represents the current log-level set
     *
     */
    Logger::LogLevel logLevel;

    /**
     * @brief Represents the type of logging (console or file)
     *
     */
    Logger::LogType logType;

    /**
     * @brief Represents the configuration mode of the logger. If true
     *        the logger is configured by a configuration ini-file. Else
     *        it is a default console logger
     *
     */
    bool iniFileMode;

    /**
     * @brief If true the log-thread period and the number of messages to be logged out during
     *        one thread cycleis is adjusted dynamically based on the log-queue fill-level
     *
     */
    bool logQMonEnabled;

    /**
     * @brief If true every message is logged into the log-queue. The drawback is a delay
     *        in case the log-queue is full. Else the overload policy is "discard" and messages
     *        are discarded if the log-queue is full
     *
     */
    bool logQOverloadWait;

    /**
     * @brief Indicates if the logger has been started.
     *
     */
    bool loggerStarted;

    /**
     * @brief Indicates if the logger shall suppress log messages. (Pausing)
     *
     */
    bool loggerSuppressed;

    /**
     * @brief The internal message stream that takes the user's log message
     *
     */
    std::ostringstream userMessageStream;

    /**
     * @brief A message stream that containes the complete log message including Tags.
     *
     */
    std::ostringstream fullMessageStream;

    /**
     * @brief The log-channel configured foe logging. This can be a fiel or a console.
     *
     */
    std::ostream* logOutChannel;

    /**
     * @brief The Single-Producer-Single-Consumer log-message-queue
     *
     */
    boost::lockfree::spsc_queue<Logger::RawMessage, boost::lockfree::capacity<Logger::LOG_MESSAGE_QUEUE_SIZE>> logMessageOutputQueue;

    /**
     * @brief The handle associated to the log-thread
     *
     */
    boost::thread logThreadHandle;

    /**
     * @brief This method parses the configuration ini-file and
     *        sets up the logger accordingly
     *
     * @param configFilename the name of the configuration ini-file
     * @return std::error_code  (0, std::generic_category()) on success
     */
    std::error_code parseConfigFile(const std::string& configFilename);

    /**
     * @brief Sets the logger's log-tags
     *
     * @param logTags the log-tags flags to be set/reset
     */
    void setLogTags(Logger::LogTag logTags);

    /**
     * @brief Set the logger's severity level
     *
     * @param level the severity level of type Logger::LogLevel
     *              Note: TRACE < DEBUG < INFO < WARN < ERR < FATAL
     */
    void setLogLevel(Logger::LogLevel level);

    /**
     * @brief Set the logger's timestamp resolution
     *
     * @param resolution the resolution of the timestamp to be set
     */
    void setTimeStampResolution(Logger::TimeStampResolution resolution);

    /**
     * @brief A message that transforms a log-queue element into a full log-message
     *
     * @param raw The raw log element
     * @return std::string
     */
    std::string formatLogMessage(Logger::RawMessage raw);

    /**
     * @brief This methods logs out the next message at the front of the queue
     *
     * @return unsigned short the number of messages in the queue
     *         that can be read before logging the next message.
     *         A 0-value indicates an empty log-queue
     */
    unsigned short logOutNextMessage();

    /**
     * @brief This is the thread that logs out the messages from the log-queue
     *        to the configured log-channel stream
     *
     */
    void logThread();

    /**
     * @brief Returns a new file stream
     *
     * @param fileCounter an integer value appended to the stream-name
     * @return std::ofstream* a file stream as: YYY-MM-DD_hh_mm_ss_fileCounter.log
     */
    inline static std::ofstream* getNewLogFile(unsigned short fileCounter);
};
