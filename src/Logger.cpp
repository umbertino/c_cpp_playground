#include "Logger.h"

#include <ostream>
//#include <iostream>
#include <sstream>
#include <string>
#include <chrono>
#include <ctime>
#include <ratio>
#include <iomanip>

#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/date_time/local_time/local_time.hpp>
#include <boost/format.hpp>


Logger::Logger(std::ostream& strm) :
    boostLogger(new boost::log::sources::severity_logger<boost::log::trivial::severity_level>),
    logChannel(strm),
    logCounter(0),
    logLevel(Logger::LogLevel::TRACE),
    logTags(Logger::LogTag::ALL_OFF)
{
    this->loggingSuppressed = true;
    boost::log::add_common_attributes();
    boost::log::add_console_log(this->logChannel, boost::log::keywords::format = "[%TimeStamp%][%Severity%]: %Message%");
}

// Logger::Logger(std::ofstream& strm) : boostLogger(new boost::log::sources::severity_logger<boost::log::trivial::severity_level>)
// {
//     this->loggingSuppressed = true;
//     boost::log::add_common_attributes();
//     boost::log::add_file_log(
//         boost::log::keywords::file_name = "sample_%N.log",
//         boost::log::keywords::open_mode = std::ios_base::out | std::ios_base::app,
//         boost::log::keywords::rotation_size = 10 * 1024 * 1024,
//         boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
//         boost::log::keywords::format = "[%TimeStamp%][%Severity%]: %Message%");
// }

Logger::~Logger()
{
}

// static class-methods
void Logger::LOG_START(Logger& instance)
{
    instance.start();
}

void Logger::LOG_STOP(Logger& instance)
{
    instance.stop();
}

void Logger::LOG_RESUME(Logger& instance)
{
    instance.resume();
}

void Logger::LOG_SUPPRESS(Logger& instance)
{
    instance.resume();
}

void Logger::LOG_SET_LEVEL(Logger& instance, Logger::LogLevel level)
{
    instance.setLogLevel(level);
}

Logger::LogLevel Logger::LOG_GET_LEVEL(Logger& instance)
{
    return instance.getLogLevel();
}

std::ostream& Logger::LOG_TRACE(Logger& instance)
{
    return instance.log(Logger::LogLevel::TRACE);
}

std::ostream& Logger::LOG_DEBUG(Logger& instance)
{
    return instance.log(Logger::LogLevel::DEBUG);
}

std::ostream& Logger::LOG_INFO(Logger& instance)
{
    return instance.log(Logger::LogLevel::INFO);
}

std::ostream& Logger::LOG_WARN(Logger& instance)
{
    return instance.log(Logger::LogLevel::WARN);
}

std::ostream& Logger::LOG_ERROR(Logger& instance)
{
    return instance.log(Logger::LogLevel::ERR);
}

std::ostream& Logger::LOG_FATAL(Logger& instance)
{
    return instance.log(Logger::LogLevel::FATAL);
}

// instance methods
void Logger::boostLog(Logger::LogLevel level, const std::ostream& messageStream)
{
    this->message << messageStream.rdbuf();

    if (!this->loggingSuppressed)
    {
        switch (level)
        {
            case Logger::TRACE:
            {
                BOOST_LOG_SEV(*(this->boostLogger), boost::log::trivial::trace) << this->message.str();
                break;
            }

            case Logger::INFO:
            {
                BOOST_LOG_SEV(*(this->boostLogger), boost::log::trivial::info) << this->message.str();
                break;
            }

            case Logger::DEBUG:
            {
                BOOST_LOG_SEV(*(this->boostLogger), boost::log::trivial::debug) << this->message.str();
                break;
            }

            case Logger::WARN:
            {
                BOOST_LOG_SEV(*(this->boostLogger), boost::log::trivial::warning) << this->message.str();
                break;
            }

            case Logger::ERR:
            {
                BOOST_LOG_SEV(*(this->boostLogger), boost::log::trivial::error) << this->message.str();
                break;
            }

            case Logger::FATAL:
            {
                BOOST_LOG_SEV(*(this->boostLogger), boost::log::trivial::fatal) << this->message.str();
                break;
            }

            default:
            {
                BOOST_LOG_SEV(*(this->boostLogger), boost::log::trivial::info) << this->message.str();
                break;
            }
        }
    }

    this->message.str(std::string("\r"));
    this->logChannel << this->message.str();
}

std::ostream& Logger::log(Logger::LogLevel level)
{
    if (!this->loggingSuppressed && level >= this->logLevel)
    {
        return this->logChannel << std::string("\r") << "[" << std::setw(5) << std::setfill(' ') << ++this->logCounter << "]" << "[" << this->getCurrentTimeStr() << "]" << "[" << Logger::logLevel2String[level] << "] ";
    }
    else
    {
        return Logger::nirvana;
    }
}


void Logger::start()
{
    this->loggingSuppressed = false;
}
void Logger::stop()
{
    this->loggingSuppressed = true;
}

void Logger::resume()
{
    this->start();
}

void Logger::suppress()
{
    this->stop();
}

void Logger::setLogLevel(Logger::LogLevel level)
{
    this->logLevel = level;
}

Logger::LogLevel Logger::getLogLevel()
{
    return this->logLevel;
}

std::string Logger::getCurrentTimeStr()
{
    std::chrono::system_clock::time_point today = std::chrono::system_clock::now();
    time_t in_time_t = std::chrono::system_clock::to_time_t(today);
    std::stringstream ss;
    auto const now = boost::posix_time::microsec_clock::universal_time();
    auto const t = now.time_of_day();
    boost::format formater("%06d");
    formater % (t.total_microseconds() % 1000000);
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X") << "." << formater.str();

    return ss.str();
}

std::ostream Logger::nirvana(NULL);
std::string const Logger::logLevel2String[] = {"TRACE", "DEBUG", "INFO ", "WARN ", "ERROR", "FATAL"};