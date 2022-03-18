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
#include <queue>
#include <sstream>
#include <thread>
#include <mutex>

// boost includes
#include <boost/lockfree/queue.hpp>
#include <boost/lockfree/spsc_queue.hpp>
#include <boost/thread/thread.hpp>
#include <boost/filesystem/convenience.hpp>

// some convenience macros
#define __BASENAME__ (boost::filesystem::path(__FILE__).filename().string())
#define __FILE_EXT__ (boost::filesystem::path(__FILE__).filename().extension().string())
#define __LOCATION__ __BASENAME__ << ":" << __FUNCTION__ << ":" << __LINE__
#define GMS(inst) inst.getMsgStream()

class Logger
{
public:
    typedef enum
    {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERR,
        FATAL
    } LogLevel;

    typedef enum
    {
        ALL_TAGS_OFF = 0b00000000,
        COUNTER = 0b00000001,
        TIME_STAMP = 0b00000010,
        LEVEL = 0b00000100,
        ALL_TAGS_ON = 0x11111111
    } LogTag;

    typedef enum
    {
        CONSOLE,
        FILE
    } LogType;

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

    typedef enum
    {
        DATE_MASK = 0b00000001,
        TIME_MASK = 0b00011110
    } Masks;

    typedef struct
    {
        unsigned long counter;
        Logger::LogLevel level;
        std::chrono::system_clock::time_point now;
        std::string userMessage;
    } RawMessage;

    typedef enum
    {
        GREEN,
        ORANGE,
        RED
    } LogQueueColor;

    typedef struct
    {
        size_t fillLevel;
        Logger::LogQueueColor condition;
    } LogQueueStatus;

    // constructors and destructors
    Logger(std::ostream& strm);
    Logger(const std::string& configFilename);
    ~Logger();

    // static public class members
    static void LOG_START(Logger& instance);
    static void LOG_STOP(Logger& instance);
    static std::error_code LOG_RESUME(Logger& instance);
    static std::error_code LOG_SUPPRESS(Logger& instance);
    static std::error_code LOG_SET_TAGS(Logger& instance, unsigned char logTags);
    static std::error_code LOG_SET_LEVEL(Logger& instance, Logger::LogLevel level);
    static std::error_code LOG_SET_TIME_STAMP_PROPS(Logger& instance, unsigned char properties);
    static Logger::LogLevel LOG_GET_LEVEL(Logger& instance);

    static std::error_code LOG_TRACE(Logger& instance, std::ostream& messageStream);
    static std::error_code LOG_DEBUG(Logger& instance, std::ostream& messageStream);
    static std::error_code LOG_INFO(Logger& instance, std::ostream& messageStream);
    static std::error_code LOG_WARN(Logger& instance, std::ostream& messageStream);
    static std::error_code LOG_ERROR(Logger& instance, std::ostream& messageStream);
    static std::error_code LOG_FATAL(Logger& instance, std::ostream& messageStream);

    // public instance members
    std::ostream& getMsgStream();
    std::error_code userLog(Logger::LogLevel level, const std::ostream& msg);
    void start();
    std::error_code stop();
    std::error_code resume();
    std::error_code suppress();
    std::error_code userSetLogTags(unsigned char logTags);
    std::error_code userSetLogLevel(Logger::LogLevel level);
    std::error_code userSetTimeStampProperties(unsigned char properties);
    Logger::LogLevel getLogLevel();

private:
    // private static members
    static std::ostream nirvana;
    static unsigned short const MIN_LOGS_PER_FILE;
    static unsigned short const MAX_LOGS_PER_FILE;
    static unsigned long const GREEN_LOG_THREAD_PERIOD_US;
    static unsigned long const ORANGE_LOG_THREAD_PERIOD_US;
    static unsigned long const RED_LOG_THREAD_PERIOD_US;
    static unsigned char const ORANGE_WMARK_PERCENT;
    static unsigned char const RED_WMARK_PERCENT;
    static std::string const logLevel2String[];
#define LOG_MESSAGE_Q_SIZE 1024

    inline static std::string getTimeStr(std::chrono::system_clock::time_point now, unsigned char properties);

    // private instance members
    unsigned char logTags;
    unsigned char timeStampProps;
    unsigned short logsPerFile;
    unsigned short logFileCounter;
    unsigned long logInCounter;
    unsigned long logOutCounter;
    unsigned long logDiscardCounter;
    Logger::LogLevel logLevel;
    Logger::LogType logType;
    Logger::LogQueueStatus logQueueStatus;
    bool iniFileMode;
    bool logQMonEnabled;
    bool logQOverloadWait;
    bool loggerStarted;
    bool loggingSuppressed;
    std::ostringstream userMessageStream;
    std::ostringstream fullMessageStream;
    std::ostream* logChannel;
    //std::queue<std::string> logMessageOutputQueue;
    boost::lockfree::spsc_queue<Logger::RawMessage, boost::lockfree::capacity<LOG_MESSAGE_Q_SIZE>> logMessageOutputQueue;
    boost::thread logThreadHandle;
    boost::thread logQueueMonThreadHandle;
    unsigned long logThreadPeriod;
    //boost::condition_variable logCv;
    //boost::mutex logMtx;

    std::error_code parseConfigFile(const std::string& configFilename);
    void setLogTags(unsigned char logTags);
    void setLogLevel(Logger::LogLevel level);
    void setTimeStampProperties(unsigned char properties);
    std::string formatLogMessage(Logger::RawMessage raw);
    void logNextMessage();
    void logThread();
    void logQueueMonThread();
    inline static std::ofstream* getNewLogFile(unsigned short fileCounter);
};