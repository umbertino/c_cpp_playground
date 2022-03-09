// own includes
#include "Logger.h"

// std includes
#include <chrono>
#include <algorithm>

// boost includes
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/date_time/local_time/local_time.hpp>

// constructors and destructors
Logger::Logger(std::ostream& strm) : loggerStarted(false),
                                     loggingSuppressed(false),
                                     logChannel(&strm),
                                     logsPerFile(Logger::MAX_LOGS_PER_FILE),
                                     logLevel(Logger::LogLevel::INFO),
                                     logTags(Logger::LogTag::ALL_TAGS_OFF),
                                     timeStampProps(Logger::TimeStampProperty::ALL_PROPS_OFF)
{
}

Logger::Logger(const std::string& configFilename) : loggerStarted(false),
                                                    loggingSuppressed(false),
                                                    logsPerFile(Logger::MAX_LOGS_PER_FILE),
                                                    logLevel(Logger::LogLevel::INFO),
                                                    logTags(Logger::LogTag::ALL_TAGS_OFF),
                                                    timeStampProps(Logger::TimeStampProperty::ALL_PROPS_OFF)
{
    std::error_condition error = this->parseConfigFile(configFilename);

    if (error.value() != 0)
    {
        std::cerr << "Falling back to default console logger" << std::endl;
        // fall back to a default console logger
        this->logChannel = &std::clog;
        this->logLevel = Logger::LogLevel::INFO;
        this->logTags = Logger::LogTag::ALL_TAGS_OFF;
        this->timeStampProps = Logger::TimeStampProperty::ALL_PROPS_OFF;
    }
}

Logger::~Logger()
{
    this->loggerStarted = false;

    try
    {
        if (this->logThreadHandle.joinable())
        {
            this->logThreadHandle.join();
        }
    }
    catch (std::system_error& e)
    {
        std::cerr << "Exception: " << e.what() << " " << e.code();
    }
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

void Logger::LOG_SET_TIME_STAMP_PROPS(Logger& instance, unsigned char properties)
{
    instance.setTimeStampProperties(properties);
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
    if ((properties | Logger::TimeStampProperty::DATE) || (properties | Logger::TimeStampProperty::TIME))
    {
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

        if ((properties & Logger::TimeStampProperty::TIME))
        {
            if (properties & Logger::TimeStampProperty::DATE)
            {
                ss << " ";
            }

            ss << std::put_time(std::localtime(&in_time_t), "%X"); // Time

            if (properties & Logger::TimeStampProperty::NANOSECS)
            {
                ss << "." << std::setw(3) << std::setfill('0') << msCurrSec.count() // miliseconds
                   << "." << std::setw(3) << std::setfill('0') << usCurrSec.count() // microseconds
                   << "." << std::setw(3) << std::setfill('0') << nsCurrSec.count(); // nanoseconds

                return ss.str();
            }

            if (properties & Logger::TimeStampProperty::MICROSECS)
            {
                ss << "." << std::setw(3) << std::setfill('0') << msCurrSec.count() // miliseconds
                   << "." << std::setw(3) << std::setfill('0') << usCurrSec.count(); // microseconds

                return ss.str();
            }

            if (properties & Logger::TimeStampProperty::MILISECS)
            {
                ss << "." << std::setw(3) << std::setfill('0') << msCurrSec.count(); // miliseconds

                return ss.str();
            }
        }

        return ss.str();
    }
    else
    {
        return "";
    }
}

std::ostream Logger::nirvana(NULL);
unsigned short const Logger::MIN_LOGS_PER_FILE = 100;
unsigned short const Logger::MAX_LOGS_PER_FILE = 10000;
std::string const Logger::logLevel2String[] = {"TRACE", "DEBUG", "INFO ", "WARN ", "ERROR", "FATAL"};

// instance members
std::ostream& Logger::log(Logger::LogLevel level)
{
    std::this_thread::sleep_for(std::chrono::microseconds(1));

    std::lock_guard<std::mutex> lock(this->logMtx);

    if (this->loggerStarted && !this->loggingSuppressed && level >= this->logLevel)
    {
        this->logMessageQueue.emplace(std::ostringstream(""));
        this->logMessageQueue.back() << std::string("\n");
        this->logCounter++;

        if (this->logTags & Logger::LogTag::COUNTER)
        {
            this->logMessageQueue.back() << "[" << std::setw(5) << std::setfill('0') << this->logCounter << "]";
        }

        if ((this->logTags & Logger::LogTag::TIME_STAMP) &&
            (this->timeStampProps != Logger::TimeStampProperty::ALL_PROPS_OFF))
        {
            std::string timeStr = this->getCurrentTimeStr(this->timeStampProps);

            if (!timeStr.empty())
            {
                this->logMessageQueue.back() << "[" << timeStr << "]";
            }
        }

        if (this->logTags & Logger::LogTag::LEVEL)
        {
            this->logMessageQueue.back() << "[" << Logger::logLevel2String[level] << "]";
        }

        if (this->logTags != Logger::LogTag::ALL_TAGS_OFF)
        {
            this->logMessageQueue.back() << " ";
        }

        return this->logMessageQueue.back();
    }
    else
    {
        return Logger::nirvana;
    }
}

void Logger::start()
{
    this->logCounter = 0;
    this->logFileCounter = 0;
    this->logFlushCounter = 1;
    this->loggerStarted = true;
    this->logThreadHandle = std::thread(&Logger::logThread, this);
}

void Logger::stop()
{
    this->logMessageQueue.emplace(""); // assures to log last log message
    this->loggerStarted = false;

    try
    {
        if (this->logThreadHandle.joinable())
        {
            this->logThreadHandle.join();
        }
    }
    catch (std::system_error& e)
    {
        std::cerr << "Exception: " << e.what() << " " << e.code();
    }
}

void Logger::resume()
{
    this->loggingSuppressed = false;
}

void Logger::suppress()
{
    this->loggingSuppressed = true;
}

void Logger::setLogTags(unsigned char logTags)
{
    this->logTags = logTags;
}

void Logger::setLogLevel(Logger::LogLevel level)
{
    this->logLevel = level;
}

void Logger::setTimeStampProperties(unsigned char properties)
{
    this->timeStampProps = properties;

    // adjust time properties in case of missing but
    // mandatory properties, e.g. if microseconds shall be
    // set make sure that general time and milliseconds are set, too
    if (Logger::TimeStampProperty::MILISECS & properties)
    {
        this->timeStampProps = this->timeStampProps |
                               Logger::TimeStampProperty::TIME;
    }

    if (Logger::TimeStampProperty::MICROSECS & properties)
    {
        this->timeStampProps = this->timeStampProps |
                               Logger::TimeStampProperty::TIME |
                               Logger::TimeStampProperty::MILISECS;
    }

    if (Logger::TimeStampProperty::NANOSECS & properties)
    {
        this->timeStampProps = this->timeStampProps |
                               Logger::TimeStampProperty::TIME |
                               Logger::TimeStampProperty::MILISECS |
                               Logger::TimeStampProperty::MICROSECS;
    }
}

Logger::LogLevel Logger::getLogLevel()
{
    return this->logLevel;
}

void Logger::getNextLogMessageInQueue()
{
    // log next message in queue to logChannel
    if (this->logMessageQueue.size() > 1)
    {
        // if we have file logging check, whether we need to create a new file
        // this is the case when the max allowed logs per file is reached
        if (this->logType == Logger::LogType::FILE)
        {
            // init a new logfile the very first time and every time the max number of log-events per file is reached
            if (((this->logFlushCounter % this->logsPerFile) == 1))
            {
                this->logChannel = Logger::getNewLogFile(++this->logFileCounter);
            }
        }

        // log to channel and remove queue element
        *(this->logChannel) << this->logMessageQueue.front().str();

        this->logMessageQueue.pop();
        this->logFlushCounter++;
    }
}

std::error_condition Logger::parseConfigFile(const std::string& configFilename)
{
    std::error_condition errCond(0, std::generic_category());
    std::ifstream ifs;

    ifs.open(configFilename.c_str(), std::ifstream::in);

    if (ifs.fail())
    {
        ifs.close();

        errCond = std::errc::no_such_file_or_directory;
    }
    else
    {
        boost::property_tree::ptree iniTree;

        // read ini/config file
        try
        {
            boost::property_tree::read_ini(configFilename.c_str(), iniTree);
        }
        catch (const boost::property_tree::ptree_error& e)
        {
            std::cerr << "ConfigFile parse exception: " << e.what() << std::endl;

            ifs.close();

            return std::errc::invalid_seek;
        }

        // [BasicSetup.LogType] processing
        try
        {
            std::string logType = iniTree.get<std::string>("BasicSetup.LogType");
            boost::to_lower(logType);

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
                std::cerr << "Configfile parse error: No feasible value found for BasicSetup.LogType" << std::endl;

                ifs.close();

                return std::errc::invalid_seek;
            }
        }
        catch (const boost::property_tree::ptree_error& e)
        {
            std::cerr << "Configfile parse exception: " << e.what() << std::endl;

            ifs.close();

            return std::errc::invalid_seek;
        }

        // [BasicSetup.LogLevel] processing
        try
        {
            std::string logLevel = iniTree.get<std::string>("BasicSetup.LogLevel");
            boost::to_lower(logLevel);

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
                std::cerr << "Configfile parse error: No feasible value found for BasicSetup.LogLevel. Defaulting to INFO" << std::endl;

                this->logLevel = Logger::LogLevel::INFO;
            }
        }
        catch (const boost::property_tree::ptree_error& e)
        {
            std::cerr << "Configfile parse exception: No BasicSetup.LogLevel found. Defaulting to INFO" << std::endl;

            this->logLevel = Logger::LogLevel::INFO;
        }

        // [ConsoleLog] processing
        if (this->logType == Logger::LogType::CONSOLE)
        {
            try
            {
                std::string logChannel = iniTree.get<std::string>("ConsoleLog.Channel");
                boost::to_lower(logChannel);

                if ("stdlog" == logChannel)
                {
                    this->logChannel = &std::clog;
                }
                else if ("stderr" == logChannel)
                {
                    this->logChannel = &std::cerr;
                }
                else if ("stdout" == logChannel)
                {
                    this->logChannel = &std::cout;
                }
                else
                {
                    std::cerr << "Configfile parse error:  No feasible value found for ConsoleLog.Channel. Defaulting std::log " << std::endl;

                    this->logChannel = &std::clog;
                }
            }
            catch (const boost::property_tree::ptree_error& e)
            {
                std::cerr << "Configfile parse exception: No ConsoleLog.Channel found. Defaulting std::log " << std::endl;

                this->logChannel = &std::clog;
            }
        }
        // [FileLog] processing
        else if (this->logType == Logger::LogType::FILE)
        {
            try
            {
                unsigned short logsPerFile = iniTree.get<unsigned short>("FileLog.MaxLogsPerFile");

                this->logsPerFile = std::clamp(logsPerFile, Logger::MIN_LOGS_PER_FILE, Logger::MAX_LOGS_PER_FILE);

                if ((logsPerFile > Logger::MAX_LOGS_PER_FILE) || (logsPerFile < Logger::MIN_LOGS_PER_FILE))
                {
                    std::cerr << "Configfile parse error: No feasible value found for FileLog.MaxLogsPerFile. Defaulting to " << this->logsPerFile << std::endl;
                }
            }
            catch (const boost::property_tree::ptree_error& e)
            {
                std::cerr << "Configfile parse exception: No FileLog.MaxLogsPerFile. Defaulting to " << Logger::MAX_LOGS_PER_FILE << std::endl;

                this->logsPerFile = Logger::MAX_LOGS_PER_FILE;
            }

            //this->logChannel = this->getNewLogFile(++this->logFileCounter);
        }
        else
        {
            std::cerr << "Configfile parse error: No feasible value found for BasicSetup.LogType." << std::endl;

            return std::errc::invalid_seek;
        }

        // [LogTag.Counter] processing
        unsigned char logTags = Logger::LogTag::ALL_TAGS_OFF;

        try
        {
            std::string logTagCounter = iniTree.get<std::string>("LogTag.Counter");
            boost::to_lower(logTagCounter);

            if ("on" == logTagCounter)
            {
                logTags = logTags | Logger::LogTag::COUNTER;
            }
            else if ("off" != logTagCounter)
            {
                std::cerr << "Configfile parse error:  No feasible value found for LogTag.Counter. Defaulting to OFF " << std::endl;
            }
        }
        catch (const boost::property_tree::ptree_error& e)
        {
            std::cerr << "Configfile parse exception:  No LogTag.Counter found. Defaulting to OFF " << std::endl;
        }

        // [LogTag.TimeStamp] processing
        try
        {
            std::string logTagTimeStamp = iniTree.get<std::string>("LogTag.TimeStamp");
            boost::to_lower(logTagTimeStamp);

            if ("on" == logTagTimeStamp)
            {
                logTags = logTags | Logger::LogTag::TIME_STAMP;
            }
            else if ("off" != logTagTimeStamp)
            {
                std::cerr << "Configfile parse error: No feasible value found for LogTag.TimeStamp. Defaulting to OFF " << std::endl;
            }
        }
        catch (const boost::property_tree::ptree_error& e)
        {
            std::cerr << "Configfile parse exception: No LogTag.TimeStamp found. Defaulting to OFF " << std::endl;

            this->logTags = this->logTags & ~Logger::LogTag::TIME_STAMP;
        }

        // [LogTag.Level] processing
        try
        {
            std::string logTagLevel = iniTree.get<std::string>("LogTag.Level");
            boost::to_lower(logTagLevel);

            if ("on" == logTagLevel)
            {
                logTags = logTags | Logger::LogTag::LEVEL;
            }
            else if ("off" != logTagLevel)
            {
                std::cerr << "Configfile parse error: No feasible value found for LogTag.Level. Defaulting to OFF " << std::endl;
            }
        }
        catch (const boost::property_tree::ptree_error& e)
        {
            std::cerr << "Configfile parse exception: No feasible LogTag.Level found. Defaulting to OFF " << std::endl;
        }

        this->setLogTags(logTags);

        if (this->logTags & Logger::LogTag::TIME_STAMP)
        {
            // [TimeStampFormat] processing
            unsigned char timeStampProperties = Logger::TimeStampProperty::ALL_PROPS_OFF;

            // [TimeStampFormat.Date] processing
            try
            {
                std::string timeStampDate = iniTree.get<std::string>("TimeStampFormat.Date");
                boost::to_lower(timeStampDate);

                if ("on" == timeStampDate)
                {
                    timeStampProperties = timeStampProperties | Logger::TimeStampProperty::DATE;
                }
                else if ("off" != timeStampDate)
                {
                    std::cerr << "Configfile parse error: No feasible TimeStampFormat.Date found. Defaulting to OFF " << std::endl;
                }
            }
            catch (const boost::property_tree::ptree_error& e)
            {
                std::cerr << "Configfile parse exception: No feasible TimeStampFormat.Date found. Defaulting to OFF " << std::endl;
            }

            // [TimeStampFormat.Time] processing
            try
            {
                std::string timeStampTime = iniTree.get<std::string>("TimeStampFormat.Time");
                boost::to_lower(timeStampTime);

                if ("on" == timeStampTime)
                {
                    timeStampProperties = timeStampProperties | Logger::TimeStampProperty::TIME;
                }
                else if ("off" != timeStampTime)
                {
                    std::cerr << "Configfile parse error: No feasible TimeStampTime.Date found. Defaulting to OFF " << std::endl;
                }
            }
            catch (const boost::property_tree::ptree_error& e)
            {
                std::cerr << "Configfile parse exception: No TimeStampFormat.Time found. Defaulting to OFF " << std::endl;
            }

            // this is only needed if Time proprty is set
            if (timeStampProperties & Logger::TimeStampProperty::TIME)
            {
                // [TimeStampFormat.TimeResolution] processing
                try
                {
                    std::string timeResolution = iniTree.get<std::string>("TimeStampFormat.TimeResolution");
                    boost::to_lower(timeResolution);

                    if ("secs" == timeResolution)
                    {
                        // nothing to do, already covered by Logger::TimeStampProperty::TIME
                    }
                    else if ("milis" == timeResolution)
                    {
                        timeStampProperties = timeStampProperties | Logger::TimeStampProperty::MILISECS;
                    }
                    else if ("micros" == timeResolution)
                    {
                        timeStampProperties = timeStampProperties | Logger::TimeStampProperty::MICROSECS;
                    }
                    else if ("nanos" == timeResolution)
                    {
                        timeStampProperties = timeStampProperties | Logger::TimeStampProperty::NANOSECS;
                    }
                    else
                    {
                        std::cerr << "Configfile parse error: No feasible value found for TimeStampFormat.Resolution. Defaulting to seconds " << std::endl;
                    }
                }
                catch (const boost::property_tree::ptree_error& e)
                {
                    std::cerr << "Configfile parse exception: No TimeStampFormat.Resolution found. Defaulting to seconds " << std::endl;
                }
            }

            this->setTimeStampProperties(timeStampProperties);
        }

        ifs.close();
    }

    return errCond;
}

void Logger::logThread()
{
    while (this->loggerStarted || this->logMessageQueue.size() > 1)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(1));
        std::lock_guard<std::mutex> lock(this->logMtx);
        this->getNextLogMessageInQueue();
    }
}

std::ofstream* Logger::getNewLogFile(unsigned short fileCounter)
{
    std::string date = Logger::getCurrentTimeStr(Logger::TimeStampProperty::DATE);
    std::string time = Logger::getCurrentTimeStr(Logger::TimeStampProperty::TIME);
    boost::replace_all(time, ":", ".");
    std::stringstream fss;
    fss << date << "_" << time << "_" << std::setw(3) << std::setfill('0') << std::to_string(fileCounter) << ".log";
    std::string logFileName = fss.str();

    return new std::ofstream(logFileName, std::ofstream::out);
}