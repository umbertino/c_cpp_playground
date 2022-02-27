#include "Logger.h"

#include <ostream>
#include <sstream>
#include <string>

#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/sources/severity_logger.hpp>

Logger::Logger(std::ostream& strm) : boostLogger(new boost::log::sources::severity_logger<boost::log::trivial::severity_level>), channel(strm)
{
    this->loggingSuppressed = true;
    boost::log::add_common_attributes();
    boost::log::add_console_log(this->channel, boost::log::keywords::format = "[%TimeStamp%][%Severity%]: %Message%");
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

void Logger::log(Logger::logLevel level, const std::ostream& messageStream)
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
    this->channel << this->message.str();
}

void Logger::trace(const std::ostream& messageStream)
{
    this->log(Logger::logLevel::TRACE, messageStream);
}

void Logger::info(const std::ostream& messageStream)
{
    this->log(Logger::logLevel::INFO, messageStream);
}

void Logger::debug(const std::ostream& messageStream)
{
    this->log(Logger::logLevel::DEBUG, messageStream);
}

void Logger::warn(const std::ostream& messageStream)
{
    this->log(Logger::logLevel::WARN, messageStream);
}

void Logger::error(const std::ostream& messageStream)
{
    this->log(Logger::logLevel::ERR, messageStream);
}

void Logger::fatal(const std::ostream& messageStream)
{
    this->log(Logger::logLevel::FATAL, messageStream);
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

// const boost::container::map<Logger::logLevel, boost::log::trivial::severity_level> levelTranslator =
//     {
//         {Logger::TRACE, boost::log::trivial::trace},
//         {Logger::DEBUG, boost::log::trivial::debug},
//         {Logger::INFO, boost::log::trivial::info},
//         {Logger::WARN, boost::log::trivial::warning},
//         {Logger::ERR, boost::log::trivial::error},
//         {Logger::FATAL, boost::log::trivial::fatal},
// };