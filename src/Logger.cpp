// own includes
#include "Logger.h"

// std includes
#include <ostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <ctime>
#include <ratio>
#include <iomanip>

// boost includes
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/date_time/local_time/local_time.hpp>
#include <boost/format.hpp>

Logger::Logger(std::ostream& strm) : boostLogger(new boost::log::sources::severity_logger<boost::log::trivial::severity_level>),
                                     logChannel(&strm),
                                     logCounter(0),
                                     logLevel(Logger::LogLevel::INFO),
                                     logTags(Logger::LogTag::ALL_TAGS_OFF)
{
    this->loggingSuppressed = true;
    boost::log::add_common_attributes();
    boost::log::add_console_log(*(this->logChannel), boost::log::keywords::format = "[%TimeStamp%][%Severity%]: %Message%");
}

Logger::Logger(const std::string& filename) : logCounter(0),
                                              logTags(Logger::LogTag::ALL_TAGS_OFF)
{
    std::error_condition error = this->parseConfigFile(filename);

    if (error.value() != 0)
    {
        std::cerr << "Falling back to default console logger" << std::endl;
        // fall back to a default console logger
        this->logChannel = &std::clog;
        this->logLevel = Logger::LogLevel::INFO;
        this->logTags = Logger::LogTag::ALL_TAGS_OFF;
    }
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

// static class members
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
    instance.suppress();
}

void Logger::LOG_SET_TAGS(Logger& instance, unsigned char logTags)
{
    instance.setLogTags(logTags);
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

std::string Logger::getCurrentTimeStr(unsigned char properties)
{
    if (Logger::TimeStampProperty::ALL_PROPS_OFF == properties)
    {
        return "";
    }

    // the current time
    std::chrono::system_clock::time_point today = std::chrono::system_clock::now();
    // the current time as epoch time
    std::chrono::system_clock::duration duration = today.time_since_epoch();

    // get duration in different units
    std::chrono::seconds durationSecs = std::chrono::duration_cast<std::chrono::seconds>(duration);
    std::chrono::milliseconds durationMiliSecs = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    std::chrono::microseconds durationMicroSecs = std::chrono::duration_cast<std::chrono::microseconds>(duration);
    std::chrono::nanoseconds durationNanosecs = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);

    // construct miliseconds, microseconds and nanoseconds
    std::chrono::milliseconds msCurrSec = durationMiliSecs - std::chrono::duration_cast<std::chrono::milliseconds>(durationSecs);
    std::chrono::microseconds usCurrSec = durationMicroSecs - std::chrono::duration_cast<std::chrono::microseconds>(durationMiliSecs);
    std::chrono::nanoseconds nsCurrSec = durationNanosecs - std::chrono::duration_cast<std::chrono::nanoseconds>(durationMicroSecs);

    // transform current time to time_t format
    time_t in_time_t = std::chrono::system_clock::to_time_t(today);

    std::stringstream ss;

    // return the time string like desired

    if (properties & Logger::TimeStampProperty::DATE)
    {
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d"); // Date
    }

    if (properties & Logger::TimeStampProperty::TIME)
    {
        ss << " " << std::put_time(std::localtime(&in_time_t), "%X"); // Time
    }

    if (properties & Logger::TimeStampProperty::MILISECS)
    {
        ss << "." << std::setw(3) << std::setfill('0') << msCurrSec.count(); // miliseconds
    }

    if (properties & Logger::TimeStampProperty::MICROSECS)
    {
        ss << "." << std::setw(3) << std::setfill('0') << usCurrSec.count(); // microseconds
    }

    if (properties & Logger::TimeStampProperty::NANOSECS)
    {
        ss << "." << std::setw(3) << std::setfill('0') << nsCurrSec.count(); // nanoseconds
    }

    return ss.str();
}

std::ostream Logger::nirvana(NULL);
std::string const Logger::logLevel2String[] = {"TRACE", "DEBUG", "INFO ", "WARN ", "ERROR", "FATAL"};

// instance members
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
    *(this->logChannel) << this->message.str();
}

std::ostream& Logger::log(Logger::LogLevel level)
{
    if (!this->loggingSuppressed && level >= this->logLevel)
    {
        *(this->logChannel) << std::string("\r");

        if (this->logTags & Logger::LogTag::COUNTER)
        {
            *(this->logChannel) << "[" << std::setw(5) << std::setfill('0') << ++this->logCounter << "]";
        }

        if (this->logTags & Logger::LogTag::TIME_STAMP)
        {
            *(this->logChannel) << "[" << this->getCurrentTimeStr(Logger::TimeStampProperty::ALL_PROPS_OFF) << "]";
        }

        if (this->logTags & Logger::LogTag::LEVEL)
        {
            *(this->logChannel) << "[" << Logger::logLevel2String[level] << "]";
        }

        if (this->logTags != Logger::LogTag::ALL_TAGS_OFF)
        {
            *(this->logChannel) << " ";
        }

        return *(this->logChannel);
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

void Logger::setLogTags(unsigned char logTags)
{
    this->logTags = logTags;
}

void Logger::setLogLevel(Logger::LogLevel level)
{
    this->logLevel = level;
}

Logger::LogLevel Logger::getLogLevel()
{
    return this->logLevel;
}

std::error_condition Logger::parseConfigFile(const std::string& filename)
{
    std::error_condition errCond(0, std::generic_category());
    std::ifstream ifs;

    ifs.open(filename.c_str(), std::ifstream::in);

    if (ifs.fail())
    {
        ifs.close();

        errCond = std::errc::no_such_file_or_directory;
    }
    else
    {
        boost::property_tree::ptree iniTree;

        try
        {
            boost::property_tree::read_ini(filename.c_str(), iniTree);
        }
        catch (const boost::property_tree::ptree_error& e)
        {
            std::cerr << "Configfile parse error: " << e.what() << std::endl;

            ifs.close();

            return std::errc::invalid_seek;
        }

        // [BasicSetup] processing
        try
        {
            std::string logType = iniTree.get<std::string>("BasicSetup.LogType");

            if ("console" == logType)
            {
                this->logType = Logger::LogType::CONSOLE;
            }
            else if ("file" == logType)
            {
                this->logType = Logger::LogType::FILE;
            }
            else
            {
                std::cerr << "Configfile parse error: No feasible BasicSetup.LogType found" << std::endl;

                ifs.close();

                return std::errc::invalid_seek;
            }
        }
        catch (const boost::property_tree::ptree_error& e)
        {
            std::cerr << "Configfile parse error: " << e.what() << std::endl;

            ifs.close();

            return std::errc::invalid_seek;
        }

        try
        {
            std::string logLevel = iniTree.get<std::string>("BasicSetup.LogLevel");

            if ("trace" == logLevel)
            {
                this->logLevel = Logger::LogLevel::TRACE;
            }
            else if ("debug" == logLevel)
            {
                this->logLevel = Logger::LogLevel::DEBUG;
            }
            else if ("info" == logLevel)
            {
                this->logLevel = Logger::LogLevel::INFO;
            }
            else if ("warning" == logLevel)
            {
                this->logLevel = Logger::LogLevel::WARN;
            }
            else if ("error" == logLevel)
            {
                this->logLevel = Logger::LogLevel::ERR;
            }
            else if ("fatal" == logLevel)
            {
                this->logLevel = Logger::LogLevel::FATAL;
            }
            else
            {
                std::cerr << "Configfile parse error: No feasible BasicSetup.LogLevel found. Defaulting to INFO" << std::endl;

                this->logLevel = Logger::LogLevel::INFO;
            }
        }
        catch (const boost::property_tree::ptree_error& e)
        {
            std::cerr << "Configfile parse error: No feasible BasicSetup.LogLevel found. Defaulting to INFO" << std::endl;

            this->logLevel = Logger::LogLevel::INFO;
        }

        // [ConsoleLog] processing
        if (this->logType == Logger::LogType::CONSOLE)
        {
            try
            {
                std::string logChannel = iniTree.get<std::string>("ConsoleLog.Channel");

                if ("stdLog" == logChannel)
                {
                    this->logChannel = &std::clog;
                }
                else if ("stdErr" == logChannel)
                {
                    this->logChannel = &std::cerr;
                }
                else if ("stdOut" == logChannel)
                {
                    this->logChannel = &std::cout;
                }
                else
                {
                    std::cerr << "Configfile parse error:  No feasible ConsoleLog.Channel found. Defaulting std::log " << std::endl;

                    this->logChannel = &std::clog;
                }
            }
            catch (const boost::property_tree::ptree_error& e)
            {
                std::cerr << "Configfile parse error:  No feasible ConsoleLog.Channel found. Defaulting std::log " << std::endl;

                this->logChannel = &std::clog;
            }
        }

        // [FileLog] processing
        else if (this->logType == Logger::LogType::FILE)
        {
        }
        else
        {
            std::cerr << "Configfile parse error:  No feasible ConsoleLog.Channel found. Defaulting std::log " << std::endl;

            this->logChannel = &std::clog;
        }

        // [LogTag] processing
        try
        {
            std::string logTagCounter = iniTree.get<std::string>("LogTag.Counter");

            if ("yes" == logTagCounter)
            {
                this->logTags = this->logTags | Logger::LogTag::COUNTER;
            }
            else if ("no" != logTagCounter)
            {
                std::cerr << "Configfile parse error:  No feasible LogTag.Counter found. Defaulting to OFF " << std::endl;

                this->logTags = this->logTags & ~Logger::LogTag::COUNTER;
            }
        }
        catch (const boost::property_tree::ptree_error& e)
        {
            std::cerr << "Configfile parse error:  No feasible LogTag.Counter found. Defaulting to OFF " << std::endl;

            this->logTags = Logger::LogTag::ALL_TAGS_OFF;
        }

        try
        {
            std::string logTagTimeStamp = iniTree.get<std::string>("LogTag.TimeStamp");

            if ("yes" == logTagTimeStamp)
            {
                this->logTags = this->logTags | Logger::LogTag::TIME_STAMP;
            }
            else if ("no" != logTagTimeStamp)
            {
                std::cerr << "Configfile parse error:  No feasible LogTag.TimeStamp found. Defaulting to OFF " << std::endl;

                this->logTags = this->logTags & ~Logger::LogTag::TIME_STAMP;
            }
        }
        catch (const boost::property_tree::ptree_error& e)
        {
            std::cerr << "Configfile parse error:  No feasible LogTag.TimeStamp found. Defaulting to OFF " << std::endl;

            this->logTags = Logger::LogTag::ALL_TAGS_OFF;
        }

        try
        {
            std::string logTagLevel = iniTree.get<std::string>("LogTag.Level");

            if ("yes" == logTagLevel)
            {
                this->logTags = this->logTags | Logger::LogTag::LEVEL;
            }
            else if ("no" != logTagLevel)
            {
                std::cerr << "Configfile parse error:  No feasible LogTag.Level found. Defaulting to OFF " << std::endl;

                this->logTags = this->logTags & ~Logger::LogTag::LEVEL;
            }
        }
        catch (const boost::property_tree::ptree_error& e)
        {
            std::cerr << "Configfile parse error:  No feasible LogTag.Level found. Defaulting to OFF " << std::endl;

            this->logTags = Logger::LogTag::ALL_TAGS_OFF;
        }
    }

    ifs.close();

    return errCond;
}