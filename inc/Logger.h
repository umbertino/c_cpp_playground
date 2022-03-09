#pragma once

// own includes

// std includes
#include <ostream>
#include <queue>
#include <sstream>
#include <thread>
#include <mutex>

// boost includes
#include <boost/filesystem/convenience.hpp>

// some convenience macros
#define __BASENAME__ (boost::filesystem::path(__FILE__).filename().string())
#define __FILE_EXT__ (boost::filesystem::path(__FILE__).filename().extension().string())
#define __LOCATION__ __BASENAME__ << ":" << __FUNCTION__ << ":" << __LINE__

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
        ALL_TAGS_OFF = 0x00,
        COUNTER = 0x01,
        TIME_STAMP = 0x02,
        LEVEL = 0x04,
        ALL_TAGS_ON = 0xFF
    } LogTag;

    typedef enum
    {
        CONSOLE,
        FILE
    } LogType;

    typedef enum
    {
        ALL_PROPS_OFF = 0x00,
        DATE = 0x01, //YYYY-MM-DD
        TIME = 0x02, //HH:MM:SS
        MILISECS = 0x04, // HH:MM:SS.mmm
        MICROSECS = 0x08, // HH:MM:SS.mmm.uuu
        NANOSECS = 0x10, // HH:MM:SS.mmm.uuu.nnn
        ALL_PROPS_ON = 0xFF
    } TimeStampProperty;

    // constructors and destructors
    Logger(std::ostream& strm);
    Logger(const std::string& configFilename);
    ~Logger();

    // static public class members
    static void LOG_START(Logger& instance);
    static void LOG_STOP(Logger& instance);
    static void LOG_RESUME(Logger& instance);
    static void LOG_SUPPRESS(Logger& instance);
    static void LOG_SET_TAGS(Logger& instance, unsigned char logTags);
    static void LOG_SET_LEVEL(Logger& instance, Logger::LogLevel level);
    static void LOG_SET_TIME_STAMP_PROPS(Logger& instance, unsigned char properties);
    static Logger::LogLevel LOG_GET_LEVEL(Logger& instance);

    static std::ostream& LOG_TRACE(Logger& instance);
    static std::ostream& LOG_DEBUG(Logger& instance);
    static std::ostream& LOG_INFO(Logger& instance);
    static std::ostream& LOG_WARN(Logger& instance);
    static std::ostream& LOG_ERROR(Logger& instance);
    static std::ostream& LOG_FATAL(Logger& instance);

    // public instance members
    std::ostream& log(Logger::LogLevel level);
    void start();
    void stop();
    void resume();
    void suppress();
    void setLogTags(unsigned char logTags);
    void setLogLevel(Logger::LogLevel level);
    void setTimeStampProperties(unsigned char properties);
    Logger::LogLevel getLogLevel();

private:
    // private static members
    static std::ostream nirvana;
    static unsigned short const MIN_LOGS_PER_FILE;
    static unsigned short const MAX_LOGS_PER_FILE;
    static std::string const logLevel2String[];

    inline static std::string getCurrentTimeStr(unsigned char properties);

    // private instance members
    unsigned char logTags;
    unsigned char timeStampProps;
    unsigned short logsPerFile;
    unsigned short logFileCounter;
    unsigned long logCounter;
    unsigned long logFlushCounter;
    Logger::LogLevel logLevel;
    Logger::LogType logType;
    bool loggerStarted;
    bool loggingSuppressed;
    std::ostream* logChannel;
    std::queue<std::ostringstream> logMessageQueue;
    std::thread logThreadHandle;
    std::mutex logMtx;

    std::error_condition parseConfigFile(const std::string& configFilename);
    void getNextLogMessageInQueue();
    void logThread();
    inline static std::ofstream* getNewLogFile(unsigned short fileCounter);
};