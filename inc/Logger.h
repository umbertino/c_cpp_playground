#pragma once

// own includes

// std includes
#include <ostream>
#include <sstream>

// boost includes
#include <boost/scoped_ptr.hpp>
#include <boost/log/trivial.hpp>
#include <boost/filesystem/convenience.hpp>

// some convenience macros
#define __BASENAME__ (boost::filesystem::path(__FILE__).filename().string())
#define __FILE_EXT__ (boost::filesystem::path(__FILE__).filename().extension().string())
#define __LOCATION__ __BASENAME__ << ":" <<__FUNCTION__ <<":" << __LINE__

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
        ALL_OFF = 0x00,
        LOG_COUNTER = 0x01,
        TIME_STAMP = 0x02,
        LOG_LEVEL = 0x04
    } LogTag;

    Logger(std::ostream& strm);
    //Logger(std::ofstream& strm);
    ~Logger();

    // static public class members
    static void LOG_START(Logger& instance);
    static void LOG_STOP(Logger& instance);
    static void LOG_RESUME(Logger& instance);
    static void LOG_SUPPRESS(Logger& instance);
    static void LOG_SET_TAGS(Logger& instance, unsigned char logTags);
    static void LOG_SET_LEVEL(Logger& instance, Logger::LogLevel level);
    static Logger::LogLevel LOG_GET_LEVEL(Logger& instance);
    static std::ostream& LOG_TRACE(Logger& instance);
    static std::ostream& LOG_DEBUG(Logger& instance);
    static std::ostream& LOG_INFO(Logger& instance);
    static std::ostream& LOG_WARN(Logger& instance);
    static std::ostream& LOG_ERROR(Logger& instance);
    static std::ostream& LOG_FATAL(Logger& instance);

    // public instance members
    void boostLog(Logger::LogLevel level, const std::ostream& messageStream);
    std::ostream& log(Logger::LogLevel level);

    void start();
    void stop();
    void resume();
    void suppress();

    void setLogTags(unsigned char logTags);
    void setLogLevel(Logger::LogLevel level);
    Logger::LogLevel getLogLevel();

private:
    // private static members
    static std::ostream nirvana;
    static std::string const logLevel2String[];
    inline static std::string getCurrentTimeStr();

    // private instance members
    unsigned char logTags;
    unsigned long logCounter;
    Logger::LogLevel logLevel;
    bool loggingSuppressed;
    std::ostream& logChannel;
    std::stringstream message;
    boost::scoped_ptr<boost::log::sources::severity_logger<boost::log::trivial::severity_level>> boostLogger;
};