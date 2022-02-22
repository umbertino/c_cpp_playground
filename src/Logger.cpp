#include "Logger.h"

#include <ostream>

#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/sources/severity_logger.hpp>

#define STRINGIZE_DETAIL(x) #x
#define STRINGIZE(x) STRINGIZE_DETAIL(x)

Logger::Logger(std::ostream& strm) : boostLogger(new boost::log::sources::severity_logger<boost::log::trivial::severity_level>)
{
    boost::log::add_common_attributes();
    boost::log::add_console_log(strm, boost::log::keywords::format = "[%TimeStamp%][%Severity%]: %Message%");
}

Logger::Logger(std::ofstream& strm) : boostLogger(new boost::log::sources::severity_logger<boost::log::trivial::severity_level>)
{
    boost::log::add_common_attributes();
    boost::log::add_file_log(
        boost::log::keywords::file_name = "sample_%N.log",
        boost::log::keywords::open_mode = std::ios_base::out | std::ios_base::app,
        boost::log::keywords::rotation_size = 10 * 1024 * 1024,
        boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
        boost::log::keywords::format = "[%TimeStamp%][%Severity%]: %Message%");
}

Logger::~Logger()
{
}

void Logger::log(Logger::logLevel level, const std::string& message)
{
    switch (level)
    {
        case Logger::TRACE:
        {
            BOOST_LOG_SEV(*(this->boostLogger), boost::log::trivial::trace) << message;
            break;
        }
        case Logger::DEBUG:
        {
            BOOST_LOG_SEV(*(this->boostLogger), boost::log::trivial::debug) << message;
            break;
        }

        case Logger::INFO:
        {
            BOOST_LOG_SEV(*(this->boostLogger), boost::log::trivial::info) << message;
            break;
        }

        case Logger::WARN:
        {
            BOOST_LOG_SEV(*(this->boostLogger), boost::log::trivial::warning) << message;
            break;
        }
        case Logger::FATAL:
        {
            BOOST_LOG_SEV(*(this->boostLogger), boost::log::trivial::trace) << message;
            break;
        }

        default:
        {
            BOOST_LOG_SEV(*(this->boostLogger), boost::log::trivial::info) << message;
            break;
        }
    }
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