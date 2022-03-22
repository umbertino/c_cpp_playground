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
     * @brief
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
     * @brief
     *
     */
    typedef enum
    {
        ALL_TAGS_OFF = 0b00000000,
        COUNTER = 0b00000001,
        TIME_STAMP = 0b00000010,
        LEVEL = 0b00000100,
        ALL_TAGS_ON = 0x11111111
    } LogTag;

    /**
     * @brief
     *
     */
    typedef enum
    {
        CONSOLE,
        FILE
    } LogType;

    /**
     * @brief
     *
     */
    typedef enum
    {
        ALL_PROPS_OFF = 0b00000000,
        DATE = 0b00000001, //YYYY-MM-DD
        SECS = 0b00000010, //HH:MM:SS
        MILISECS = 0b00000100, // HH:MM:SS.mmm
        MICROSECS = 0b00001000, // HH:MM:SS.mmm.uuu
        NANOSECS = 0b00010000, // HH:MM:SS.mmm.uuu.nnn
        ALL_PROPS_ON = 0b11111111
    } TimeStampProperty;

    /**
     * @brief
     *
     */
    typedef enum
    {
        DATE_MASK = 0b00000001,
        TIME_MASK = 0b00011110
    } Masks;

    /**
     * @brief
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
     * @brief
     *
     */
    typedef enum
    {
        GREEN,
        ORANGE,
        RED
    } LogQueueColor;

    /**
     * @brief 
     *
     */
    typedef struct
    {
        size_t fillLevel;
        Logger::LogQueueColor condition;
    } LogQueueStatus;

    /**
     * @brief Construct a new Logger object
     *
     */
    Logger();

    /**
     * @brief Construct a new Logger object
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
     * @brief
     *
     * @param instance
     * @return std::error_code
     */
    static std::error_code LOG_START(Logger& instance);

    /**
     * @brief
     *
     * @param instance
     * @return std::error_code
     */
    static std::error_code LOG_STOP(Logger& instance);

    /**
     * @brief
     *
     * @param instance
     * @return std::error_code
     */
    static std::error_code LOG_SUPPRESS(Logger& instance);

    /**
     * @brief
     *
     * @param instance
     * @return std::error_code
     */
    static std::error_code LOG_RESUME(Logger& instance);

    /**
     * @brief
     *
     * @param instance
     * @param logTags
     * @return std::error_code
     */
    static std::error_code LOG_SET_TAGS(Logger& instance, unsigned char logTags);

    /**
     * @brief
     *
     * @param instance
     * @param level
     * @return std::error_code
     */
    static std::error_code LOG_SET_LEVEL(Logger& instance, Logger::LogLevel level);

    /**
     * @brief
     *
     * @param instance
     * @param properties
     * @return std::error_code
     */
    static std::error_code LOG_SET_TIME_STAMP_PROPS(Logger& instance, unsigned char properties);

    /**
     * @brief
     *
     * @param instance
     * @param messageStream
     * @return std::error_code
     */
    static std::error_code LOG_TRACE(Logger& instance, std::ostream& messageStream);

    /**
     * @brief
     *
     * @param instance
     * @param messageStream
     * @return std::error_code
     */
    static std::error_code LOG_DEBUG(Logger& instance, std::ostream& messageStream);

    /**
     * @brief
     *
     * @param instance
     * @param messageStream
     * @return std::error_code
     */
    static std::error_code LOG_INFO(Logger& instance, std::ostream& messageStream);

    /**
     * @brief
     *
     * @param instance
     * @param messageStream
     * @return std::error_code
     */
    static std::error_code LOG_WARN(Logger& instance, std::ostream& messageStream);

    /**
     * @brief
     *
     * @param instance
     * @param messageStream
     * @return std::error_code
     */
    static std::error_code LOG_ERROR(Logger& instance, std::ostream& messageStream);

    /**
     * @brief
     *
     * @param instance
     * @param messageStream
     * @return std::error_code
     */
    static std::error_code LOG_FATAL(Logger& instance, std::ostream& messageStream);

    /**
     * @brief
     *
     * @param instance
     * @return Logger::LogLevel
     */
    static Logger::LogLevel LOG_GET_LEVEL(Logger& instance);

    /**
     * @brief
     *
     * @return std::ostream&
     */
    std::ostream& userGetMsgStream();

    /**
     * @brief
     *
     * @return std::error_code
     */
    std::error_code userStartLog();

    /**
     * @brief
     *
     * @return std::error_code
     */
    std::error_code userStopLog();

    /**
     * @brief
     *
     * @return std::error_code
     */
    std::error_code userResumeLog();

    /**
     * @brief
     *
     * @return std::error_code
     */
    std::error_code userSuppressLog();

    /**
     * @brief
     *
     * @param logTags
     * @return std::error_code
     */
    std::error_code userSetLogTags(unsigned char logTags);

    /**
     * @brief
     *
     * @param level
     * @return std::error_code
     */
    std::error_code userSetLogLevel(Logger::LogLevel level);

    /**
     * @brief
     *
     * @param properties
     * @return std::error_code
     */
    std::error_code userSetTimeStampProperties(unsigned char properties);

    /**
     * @brief
     *
     * @param level
     * @param msg
     * @return std::error_code
     */
    std::error_code userLog(Logger::LogLevel level, const std::ostream& msg);

    /**
     * @brief
     *
     * @return Logger::LogLevel
     */
    Logger::LogLevel userGetLogLevel();

private:
    // private static members

    /**
     * @brief
     *
     */
    static constexpr unsigned char ORANGE_WMARK_PERCENT = 33;

    /**
     * @brief
     *
     */
    static constexpr unsigned char RED_WMARK_PERCENT = 67;

    /**
     * @brief
     *
     */
    static constexpr unsigned short MIN_LOGS_PER_FILE = 100;

    /**
     * @brief
     *
     */
    static constexpr unsigned short MAX_LOGS_PER_FILE = 10000;

    /**
     * @brief
     *
     */
    static constexpr unsigned short LOG_MESSAGE_QUEUE_SIZE = 1024;

    /**
     * @brief
     *
     */
    static constexpr unsigned short LOG_MESSAGE_QUEUE_ORANGE_THRESHLD = (Logger::LOG_MESSAGE_QUEUE_SIZE * ORANGE_WMARK_PERCENT) / 100;

    /**
     * @brief
     *
     */
    static constexpr unsigned short LOG_MESSAGE_QUEUE_RED_THRESHLD = (Logger::LOG_MESSAGE_QUEUE_SIZE * RED_WMARK_PERCENT) / 100;

    /**
     * @brief
     *
     */
    static constexpr unsigned long GREEN_LOG_THREAD_PERIOD_US = 500000;

    /**
     * @brief
     *
     */
    static constexpr unsigned long ORANGE_LOG_THREAD_PERIOD_US = 1000;

    /**
     * @brief
     *
     */
    static constexpr unsigned long RED_LOG_THREAD_PERIOD_US = 10;

    /**
     * @brief
     *
     */
    static std::ostream nirvana;

    /**
     * @brief
     *
     */
    static const std::string logLevel2String[];

    /**
     * @brief Get the Time Str object
     *
     * @param now
     * @param properties
     * @return std::string
     */
    inline static std::string getTimeStr(boost::chrono::system_clock::time_point now, unsigned char properties);

    /**
     * @brief
     *
     */
    unsigned char logTags;

    /**
     * @brief
     *
     */
    unsigned char timeStampProps;

    /**
     * @brief
     *
     */
    unsigned short logsPerFile;

    /**
     * @brief
     *
     */
    unsigned short logFileCounter;

    /**
     * @brief
     *
     */
    unsigned long logInCounter;

    /**
     * @brief
     *
     */
    unsigned long logOutCounter;

    /**
     * @brief
     *
     */
    unsigned long logDiscardCounter;

    /**
     * @brief
     *
     */
    Logger::LogLevel logLevel;

    /**
     * @brief
     *
     */
    Logger::LogType logType;

    /**
     * @brief
     *
     */
    bool iniFileMode;

    /**
     * @brief
     *
     */
    bool logQMonEnabled;

    /**
     * @brief
     *
     */
    bool logQOverloadWait;

    /**
     * @brief
     *
     */
    bool loggerStarted;

    /**
     * @brief
     *
     */
    bool loggerSuppressed;

    /**
     * @brief
     *
     */
    std::ostringstream userMessageStream;

    /**
     * @brief
     *
     */
    std::ostringstream fullMessageStream;

    /**
     * @brief
     *
     */
    std::ostream* logOutChannel;

    /**
     * @brief
     *
     */
    boost::lockfree::spsc_queue<Logger::RawMessage, boost::lockfree::capacity<Logger::LOG_MESSAGE_QUEUE_SIZE>> logMessageOutputQueue;

    /**
     * @brief
     *
     */
    boost::thread logThreadHandle;

    /**
     * @brief
     *
     * @param configFilename
     * @return std::error_code
     */
    std::error_code parseConfigFile(const std::string& configFilename);

    /**
     * @brief Set the Log Tags object
     *
     * @param logTags
     */
    void setLogTags(unsigned char logTags);

    /**
     * @brief Set the Log Level object
     *
     * @param level
     */
    void setLogLevel(Logger::LogLevel level);

    /**
     * @brief Set the Time Stamp Properties object
     *
     * @param properties
     */
    void setTimeStampProperties(unsigned char properties);

    /**
     * @brief
     *
     * @param raw
     * @return std::string
     */
    std::string formatLogMessage(Logger::RawMessage raw);

    /**
     * @brief
     *
     */
    void logOutNextMessage();

    /**
     * @brief
     *
     */
    void logThread();

    /**
     * @brief Get the New Log File object
     *
     * @param fileCounter
     * @return std::ofstream*
     */
    inline static std::ofstream* getNewLogFile(unsigned short fileCounter);
};
