/**
 * @file Logger.cpp
 * @author umbertino@mailbox.org
 * @brief Implementation of Logger-Class
 * @date 2022-03-14
 *
 */

// own includes
#include "Logger.h"

// std includes
#include <iostream>
#include <algorithm>

// boost includes
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/date_time/local_time/local_time.hpp>

// Initialization of static class members
std::ostream Logger::nirvana(nullptr);

Logger::Logger() : iniFileMode(false),
                   logQMonEnabled(false),
                   logQOverloadWait(true),
                   loggerStarted(false),
                   loggerSuppressed(false),
                   logOutChannel(&std::clog),
                   logLevel(Logger::LogLevel::INFO),
                   logTags(Logger::LogTag::ALL_TAGS_OFF | Logger::LogTag::COUNTER | Logger::LogTag::TIME_STAMP | Logger::LogTag::LEVEL),
                   timeStampProps(Logger::TimeStampProperty::ALL_PROPS_OFF | Logger::TimeStampProperty::DATE | Logger::TimeStampProperty::MILISECS)
{
}

Logger::Logger(const std::string& configFilename) : iniFileMode(true),
                                                    logQMonEnabled(false),
                                                    logQOverloadWait(true),
                                                    loggerStarted(false),
                                                    loggerSuppressed(false),
                                                    logOutChannel(nullptr),
                                                    logsPerFile(Logger::MAX_LOGS_PER_FILE),
                                                    logLevel(Logger::LogLevel::INFO),
                                                    logTags(Logger::LogTag::ALL_TAGS_OFF),
                                                    timeStampProps(Logger::TimeStampProperty::ALL_PROPS_OFF)
{
    std::error_code error = this->parseConfigFile(configFilename);

    if (error.value() != 0)
    {
        std::cerr << "Falling back to default console logger" << std::endl;

        // fall back to a default console logger
        this->iniFileMode = false;
        this->logOutChannel = &std::clog;
        this->logLevel = Logger::LogLevel::INFO;
        this->logTags = Logger::LogTag::ALL_TAGS_OFF | Logger::LogTag::COUNTER | Logger::LogTag::TIME_STAMP | Logger::LogTag::LEVEL;
        this->timeStampProps = Logger::TimeStampProperty::ALL_PROPS_OFF | Logger::TimeStampProperty::DATE | Logger::TimeStampProperty::MILISECS;
    }
}

Logger::~Logger()
{
    this->logOutChannel->flush();

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

std::error_code Logger::LOG_START(Logger& instance)
{
    return instance.userStartLog();
}

std::error_code Logger::LOG_STOP(Logger& instance)
{
    return instance.userStopLog();
}

std::error_code Logger::LOG_RESUME(Logger& instance)
{
    return instance.userResumeLog();
}

std::error_code Logger::LOG_SUPPRESS(Logger& instance)
{
    return instance.userSuppressLog();
}

std::error_code Logger::LOG_SET_TAGS(Logger& instance, unsigned char logTags)
{
    return instance.userSetLogTags(logTags);
}

std::error_code Logger::LOG_SET_LEVEL(Logger& instance, Logger::LogLevel level)
{
    return instance.userSetLogLevel(level);
}

std::error_code Logger::LOG_SET_TIME_STAMP_PROPS(Logger& instance, unsigned char properties)
{
    return instance.userSetTimeStampProperties(properties);
}

Logger::LogLevel Logger::LOG_GET_LEVEL(Logger& instance)
{
    return instance.userGetLogLevel();
}

std::error_code Logger::LOG_TRACE(Logger& instance, std::ostream& messageStream)
{
    return instance.userLog(Logger::LogLevel::TRACE, messageStream);
}

std::error_code Logger::LOG_DEBUG(Logger& instance, std::ostream& messageStream)
{
    return instance.userLog(Logger::LogLevel::DEBUG, messageStream);
}

std::error_code Logger::LOG_INFO(Logger& instance, std::ostream& messageStream)
{
    return instance.userLog(Logger::LogLevel::INFO, messageStream);
}

std::error_code Logger::LOG_WARN(Logger& instance, std::ostream& messageStream)
{
    return instance.userLog(Logger::LogLevel::WARN, messageStream);
}

std::error_code Logger::LOG_ERROR(Logger& instance, std::ostream& messageStream)
{
    return instance.userLog(Logger::LogLevel::ERR, messageStream);
}

std::error_code Logger::LOG_FATAL(Logger& instance, std::ostream& messageStream)
{
    return instance.userLog(Logger::LogLevel::FATAL, messageStream);
}

std::string Logger::getTimeStr(boost::chrono::system_clock::time_point now, unsigned char properties)
{
    if ((properties | Logger::TimeStampProperty::DATE) || (properties | Logger::TimeStampProperty::SECS))
    {
        // the current time as epoch time
        boost::chrono::system_clock::duration duration = now.time_since_epoch();

        // get duration in different units
        boost::chrono::nanoseconds durationNanosecs = boost::chrono::duration_cast<boost::chrono::nanoseconds>(duration);
        unsigned long curSubSecondNs = durationNanosecs.count() % 1000000000;

        // transform current time to time_t format
        time_t in_time_t = boost::chrono::system_clock::to_time_t(now);

        std::stringstream ss;

        // return the time string like desired
        if (properties & Logger::TimeStampProperty::DATE)
        {
            ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d"); // Date
        }

        if ((properties & Logger::TimeStampProperty::DATE) && (properties & Logger::TIME_MASK))
        {
            ss << " ";
        }

        if (properties & Logger::TIME_MASK)
        {
            ss << std::put_time(std::localtime(&in_time_t), "%X"); // Time

            if (properties & Logger::TimeStampProperty::NANOSECS)
            {
                ss << "." << std::setw(3) << std::setfill('0') << (curSubSecondNs / 1000000) // miliseconds
                   << "." << std::setw(3) << std::setfill('0') << (curSubSecondNs % 1000000) / 1000 // microseconds
                   << "." << std::setw(3) << std::setfill('0') << (curSubSecondNs % 1000000) % 1000; // nanoseconds
            }
            else if (properties & Logger::TimeStampProperty::MICROSECS)
            {
                ss << "." << std::setw(3) << std::setfill('0') << (curSubSecondNs / 1000000) // miliseconds
                   << "." << std::setw(3) << std::setfill('0') << (curSubSecondNs % 1000000) / 1000; // microseconds
            }
            else if (properties & Logger::TimeStampProperty::MILISECS)
            {
                ss << "." << std::setw(3) << std::setfill('0') << (curSubSecondNs / 1000000); // miliseconds
            }
        }

        return ss.str();
    }
    else
    {
        return "";
    }
}

std::ostream& Logger::userGetMsgStream()
{
    return this->userMessageStream;
}

std::error_code Logger::userLog(Logger::LogLevel level, const std::ostream& msg)
{
    if (this->loggerStarted)
    {
        std::error_code ec(0, std::generic_category());

        if (!this->loggerSuppressed && level >= this->logLevel)
        {
            // get the time stamp as soon as possible
            boost::chrono::system_clock::time_point now = boost::chrono::system_clock::now();
            this->logInCounter++;

            // get current free elements in qwuque
            size_t freeQElems = this->logMessageOutputQueue.write_available();

            // // check log overload policy
            if (this->logQOverloadWait)
            {
                // wait for a free place in the log-message queue: will block caller
                while (freeQElems == 0)
                {
                    freeQElems = this->logMessageOutputQueue.write_available();
                }
            }

            if (freeQElems > 0 || this->logQOverloadWait)
            {
                this->userMessageStream << std::endl;
                this->logMessageOutputQueue.push(Logger::RawMessage{this->logInCounter, level, now, this->userMessageStream.str()});
            }
            else
            {
                // discard this log-message: discards can be identified by missing counter values
                this->logDiscardCounter++;

                ec = std::make_error_code(std::errc::operation_canceled);
            }
        }

        // clear the stream for next usage
        this->userMessageStream.str(std::string("\r"));
        Logger::nirvana << this->userMessageStream.str();

        return ec;
    }
    else
    {
        return std::make_error_code(std::errc::operation_not_permitted);
    }
}

std::error_code Logger::userStartLog()
{
    this->logInCounter = 0;
    this->logDiscardCounter = 0;
    this->logFileCounter = 0;
    this->logOutCounter = 0;
    this->loggerStarted = true;
    this->logThreadHandle = boost::thread(&Logger::logThread, this);

    return std::error_code(0, std::generic_category());
}

std::error_code Logger::userStopLog()
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

        return e.code();
    }

    // write Log-Session Statistics to log-channel
    *(this->logOutChannel) << "\n\nLog-Session stopped at "
                           << Logger::getTimeStr(boost::chrono::system_clock::now(), this->timeStampProps) << std::endl
                           << "User Log-Attempts: " << std::setw(10) << this->logInCounter << std::endl
                           << "Successful Logs  : " << std::setw(10) << this->logOutCounter << std::endl
                           << "Discarded Logs   : " << std::setw(10) << this->logDiscardCounter << std::endl
                           << "Plausability     : " << std::setw(10) << ((this->logOutCounter + this->logDiscardCounter == this->logInCounter) ? "OK" : "NOK") << std::endl
                           << std::endl;

    return std::error_code(0, std::generic_category());
}

std::error_code Logger::userResumeLog()
{
    if (this->loggerStarted)
    {
        this->loggerSuppressed = false;
    }
    else
    {
        return std::make_error_code(std::errc::operation_not_permitted);
    }

    return std::error_code(0, std::generic_category());
}

std::error_code Logger::userSuppressLog()
{
    if (this->loggerStarted)
    {
        this->loggerSuppressed = true;
    }
    else
    {
        return std::make_error_code(std::errc::operation_not_permitted);
    }

    return std::error_code(0, std::generic_category());
}

std::error_code Logger::userSetLogTags(unsigned char logTags)
{
    if (this->iniFileMode)
    {
        return std::make_error_code(std::errc::operation_not_permitted);
    }
    else
    {
        this->setLogTags(logTags);

        return std::error_code(0, std::generic_category());
    }
}

std::error_code Logger::userSetLogLevel(Logger::LogLevel level)
{
    if (this->iniFileMode)
    {
        return std::make_error_code(std::errc::operation_not_permitted);
    }
    else
    {
        this->setLogLevel(level);

        return std::error_code(0, std::generic_category());
    }
}

std::error_code Logger::userSetTimeStampProperties(unsigned char properties)
{
    if (this->iniFileMode)
    {
        return std::make_error_code(std::errc::operation_not_permitted);
    }
    else
    {
        this->setTimeStampProperties(properties);

        return std::error_code(0, std::generic_category());
    }
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
    if (Logger::TimeStampProperty::NANOSECS & properties)
    {
        this->timeStampProps = (properties & ~Logger::TIME_MASK) | Logger::TimeStampProperty::NANOSECS;
    }
    else if (Logger::TimeStampProperty::MICROSECS & properties)
    {
        this->timeStampProps = (properties & ~Logger::TIME_MASK) | Logger::TimeStampProperty::MICROSECS;
    }
    else if (Logger::TimeStampProperty::MILISECS & properties)
    {
        this->timeStampProps = (properties & ~Logger::TIME_MASK) | Logger::TimeStampProperty::MILISECS;
    }
    else if (Logger::TimeStampProperty::SECS & properties)
    {
        this->timeStampProps = (properties & ~Logger::TIME_MASK) | Logger::TimeStampProperty::SECS;
    }
    else
    {
        this->timeStampProps = (properties & ~Logger::TIME_MASK);
    }
}

Logger::LogLevel Logger::userGetLogLevel()
{
    return this->logLevel;
}

std::string Logger::formatLogMessage(Logger::RawMessage raw)
{
    if (this->logTags & Logger::LogTag::COUNTER)
    {
        this->fullMessageStream << "[" << std::setw(5) << std::setfill('0') << raw.counter << "]";
    }

    if ((this->logTags & Logger::LogTag::TIME_STAMP) &&
        (this->timeStampProps != Logger::TimeStampProperty::ALL_PROPS_OFF))
    {
        std::string timeStr = this->getTimeStr(raw.now, this->timeStampProps);

        if (!timeStr.empty())
        {
            this->fullMessageStream << "[" << timeStr << "]";
        }
    }

    if (this->logTags & Logger::LogTag::LEVEL)
    {
        this->fullMessageStream << "[" << Logger::LOG_LEVEL_2_STRING[raw.level] << "]";
    }

    if (this->logTags != Logger::LogTag::ALL_TAGS_OFF)
    {
        this->fullMessageStream << " ";
    }

    this->fullMessageStream << raw.userMessage;

    std::string formattedMessage = this->fullMessageStream.str();

    this->fullMessageStream.str(std::string("\r"));
    Logger::nirvana << this->fullMessageStream.str();

    return formattedMessage;
}

unsigned short Logger::logOutNextMessage()
{
    unsigned short logsInQueue = this->logMessageOutputQueue.read_available();

    if (logsInQueue > 0)
    {
        this->logOutCounter++;

        // if we have file logging check, whether we need to create a new file
        // this is the case at the very beginning and when the max allowed
        // logs per file is reached
        if (this->logType == Logger::LogType::FILE)
        {
            if (((this->logOutCounter % this->logsPerFile) == 1))
            {
                if (!(this->logOutChannel == nullptr))
                {
                    // assure the rest is written to the current
                    // file before getting a new one
                    this->logOutChannel->flush();
                }

                this->logOutChannel = Logger::getNewLogFile(++this->logFileCounter);
            }
        }

        // userLog to channel, remove queue element and increment the log-out counter
        *(this->logOutChannel) << this->formatLogMessage(this->logMessageOutputQueue.front());
        this->logMessageOutputQueue.pop();
    }

    return logsInQueue;
}

std::error_code Logger::parseConfigFile(const std::string& configFilename)
{
    std::ifstream iniFs;

    iniFs.open(configFilename.c_str(), std::ifstream::in);

    if (iniFs.fail())
    {
        iniFs.close();

        return std::make_error_code(std::errc::no_such_file_or_directory);
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

            iniFs.close();

            return std::make_error_code(std::errc::invalid_seek);
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

                iniFs.close();

                return std::make_error_code(std::errc::invalid_seek);
            }
        }
        catch (const boost::property_tree::ptree_error& e)
        {
            std::cerr << "Configfile parse exception: " << e.what() << std::endl;

            iniFs.close();

            return std::make_error_code(std::errc::invalid_seek);
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

        // [BasicSetup.MessageQMon] processing
        try
        {
            std::string msgQMon = iniTree.get<std::string>("BasicSetup.MessageQMon");
            boost::to_lower(msgQMon);

            if ("on" == msgQMon)
            {
                this->logQMonEnabled = true;
            }
            else if ("off" != msgQMon)
            {
                std::cerr << "Configfile parse error:  No feasible value found for BasicSetup.MessageQMon. Defaulting to OFF " << std::endl;
            }
        }
        catch (const boost::property_tree::ptree_error& e)
        {
            std::cerr << "Configfile parse exception:  No BasicSetup.MessageQMon found. Defaulting to OFF " << std::endl;
        }

        // [BasicSetup.MessageQOverLoadPolicy] processing
        try
        {
            std::string msgQMonOvldPolicy = iniTree.get<std::string>("BasicSetup.MessageQOverLoadPolicy");
            boost::to_lower(msgQMonOvldPolicy);

            if ("discard" == msgQMonOvldPolicy)
            {
                this->logQOverloadWait = false;
            }
            else if ("wait" != msgQMonOvldPolicy)
            {
                std::cerr << "Configfile parse error:  No feasible value found for BasicSetup.MessageQOverLoadPolicy. Defaulting to WAIT " << std::endl;
            }
        }
        catch (const boost::property_tree::ptree_error& e)
        {
            std::cerr << "Configfile parse exception:  No BasicSetup.MessageQOverLoadPolicy found. Defaulting to OFF " << std::endl;
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
                    this->logOutChannel = &std::clog;
                }
                else if ("stderr" == logChannel)
                {
                    this->logOutChannel = &std::cerr;
                }
                else if ("stdout" == logChannel)
                {
                    this->logOutChannel = &std::cout;
                }
                else
                {
                    std::cerr << "Configfile parse error:  No feasible value found for ConsoleLog.Channel. Defaulting std::log " << std::endl;

                    this->logOutChannel = &std::clog;
                }
            }
            catch (const boost::property_tree::ptree_error& e)
            {
                std::cerr << "Configfile parse exception: No ConsoleLog.Channel found. Defaulting std::log " << std::endl;

                this->logOutChannel = &std::clog;
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
        }
        else
        {
            std::cerr << "Configfile parse error: No feasible value found for BasicSetup.LogType." << std::endl;

            return std::make_error_code(std::errc::invalid_seek);
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
                    timeStampProperties = timeStampProperties | Logger::TimeStampProperty::SECS;
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
            if (timeStampProperties & Logger::TimeStampProperty::SECS)
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

        iniFs.close();

        return std::error_code(0, std::generic_category());
    }
}

void Logger::logThread()
{
    // start values for thread control
    unsigned long newLogThreadPeriod = Logger::DEFAULT_THREAD_PERIOD_US;
    unsigned long prevLogThreadPeriod = Logger::DEFAULT_THREAD_PERIOD_US;
    unsigned short newLogsAtOnce = Logger::LOG_DEFAULT_LOGS_AT_ONCE;
    unsigned short prevLogsAtOnce = Logger::LOG_DEFAULT_LOGS_AT_ONCE;
    Logger::LogQueueStatus currQueueStatus;

    typedef enum
    {
        LOG,
        WAIT,
        EXIT
    } threadState;

    auto getLogQStat = [this]() -> Logger::LogQueueStatus
    {
        // get the current fill level of the message queue
        size_t queueFillLevel = this->logMessageOutputQueue.read_available();

        if (queueFillLevel < Logger::LOG_MESSAGE_QUEUE_ORANGE_THRESHLD)
        {
            return Logger::LogQueueStatus({queueFillLevel, Logger::LogQueueColor::GREEN});
        }
        else if ((queueFillLevel >= Logger::LOG_MESSAGE_QUEUE_ORANGE_THRESHLD) &&
                 (queueFillLevel < Logger::LOG_MESSAGE_QUEUE_RED_THRESHLD))
        {
            return Logger::LogQueueStatus({queueFillLevel, Logger::LogQueueColor::ORANGE});
        }
        else
        {
            return Logger::LogQueueStatus({queueFillLevel, Logger::LogQueueColor::RED});
        }
    };

    // variant 1 of an algorithm to control the logqueue consumer
    auto setThreadParams1 = [this, &currQueueStatus, &newLogThreadPeriod, &newLogsAtOnce]() -> threadState
    {
        // Condition 1
        // exit criteria for this thread,
        // logger has been stopped and queue is empty
        if (!this->loggerStarted && currQueueStatus.fillLevel == 0)
        {
            return EXIT;
        }

        // Condition 2
        // logger has been stopped but queue still contains
        // items, thus log out remaining messages in Queue
        if (!this->loggerStarted && currQueueStatus.fillLevel > 0)
        {
            // speed up logging out to terminate this thread quickly
            newLogThreadPeriod = Logger::LOG_RED_THREAD_PERIOD_US;
            newLogsAtOnce = 100;

            return LOG;
        }

        // Condition 3
        // logger is started but queue is empty, stay prepared
        if (this->loggerStarted && currQueueStatus.fillLevel == 0)
        {
            newLogThreadPeriod = Logger::LOG_ORANGE_THREAD_PERIOD_US;
            newLogsAtOnce = 10;

            return WAIT;
        }

        // Condition 4
        // logger is started and queue contains messages
        // Check Log-Queuestaus and adjust thread period and number of logs to log at once
        // This is only supported if Queue-Monitoring was activated
        if (this->logQMonEnabled && currQueueStatus.fillLevel > 0)
        {
            if (currQueueStatus.condition == Logger::LogQueueColor::GREEN)
            {
                if (newLogThreadPeriod < Logger::LOG_GREEN_THREAD_PERIOD_US - 100)
                {
                    // default period for log thread
                    newLogThreadPeriod = std::clamp(static_cast<unsigned long>(newLogThreadPeriod + 100), Logger::LOG_RED_THREAD_PERIOD_US, Logger::LOG_GREEN_THREAD_PERIOD_US);
                }

                // log one message per thread call or ramp down to 1
                newLogsAtOnce = std::clamp(static_cast<unsigned short>(newLogsAtOnce - 1), static_cast<unsigned short>(1), Logger::LOG_MESSAGE_QUEUE_SIZE);

                return LOG;
            }

            if (currQueueStatus.condition == Logger::LogQueueColor::ORANGE)
            {
                // decrease the period of log thread
                if (newLogThreadPeriod >= Logger::LOG_RED_THREAD_PERIOD_US + 100)
                {
                    newLogThreadPeriod = std::clamp(static_cast<unsigned long>(newLogThreadPeriod - 100), Logger::LOG_RED_THREAD_PERIOD_US, Logger::LOG_GREEN_THREAD_PERIOD_US);
                }
                else if (newLogThreadPeriod >= Logger::LOG_RED_THREAD_PERIOD_US + 10)
                {
                    newLogThreadPeriod = std::clamp(static_cast<unsigned long>(newLogThreadPeriod - 10), Logger::LOG_RED_THREAD_PERIOD_US, Logger::LOG_GREEN_THREAD_PERIOD_US);
                }
                else
                {
                    newLogThreadPeriod = std::clamp(static_cast<unsigned long>(newLogThreadPeriod - 1), Logger::LOG_RED_THREAD_PERIOD_US, Logger::LOG_GREEN_THREAD_PERIOD_US);
                }

                // increase the number of meaaages per thread call by one (ramp up)
                newLogsAtOnce = std::clamp(static_cast<unsigned short>(newLogsAtOnce + 1), static_cast<unsigned short>(1), Logger::LOG_MESSAGE_QUEUE_SIZE);

                return LOG;
            }

            if (currQueueStatus.condition == Logger::LogQueueColor::RED)
            {
                // decrease the period of log thread even more
                if (newLogThreadPeriod >= Logger::LOG_RED_THREAD_PERIOD_US + 1000)
                {
                    newLogThreadPeriod = std::clamp(static_cast<unsigned long>(newLogThreadPeriod - 1000), Logger::LOG_RED_THREAD_PERIOD_US, Logger::LOG_GREEN_THREAD_PERIOD_US);
                }
                else if (newLogThreadPeriod >= Logger::LOG_RED_THREAD_PERIOD_US + 100)
                {
                    newLogThreadPeriod = std::clamp(static_cast<unsigned long>(newLogThreadPeriod - 100), Logger::LOG_RED_THREAD_PERIOD_US, Logger::LOG_GREEN_THREAD_PERIOD_US);
                }
                else if (newLogThreadPeriod >= Logger::LOG_RED_THREAD_PERIOD_US + 10)
                {
                    newLogThreadPeriod = std::clamp(static_cast<unsigned long>(newLogThreadPeriod - 10), Logger::LOG_RED_THREAD_PERIOD_US, Logger::LOG_GREEN_THREAD_PERIOD_US);
                }
                else
                {
                    newLogThreadPeriod = std::clamp(static_cast<unsigned long>(newLogThreadPeriod - 1), Logger::LOG_RED_THREAD_PERIOD_US, Logger::LOG_GREEN_THREAD_PERIOD_US);
                }

                // increase the number of meaaages per thread call by 10 (ramp up faster)
                newLogsAtOnce = std::clamp(static_cast<unsigned short>(newLogsAtOnce + 10), static_cast<unsigned short>(1), Logger::LOG_MESSAGE_QUEUE_SIZE);

                return LOG;
            }

            return LOG;
        }

        // Condition 5
        // logger is started and queue contains messages
        // keep thread period an logs at once constant
        if (!this->logQMonEnabled && currQueueStatus.fillLevel > 0)
        {
            newLogThreadPeriod = Logger::LOG_ORANGE_THREAD_PERIOD_US;
            newLogsAtOnce = 10;

            return LOG;
        }

        return LOG;
    };

    // variant 2 of an algorithm to control the logqueue consumer
    auto setThreadParams2 = [this, &currQueueStatus, &newLogThreadPeriod, &prevLogThreadPeriod, &newLogsAtOnce, &prevLogsAtOnce]() -> threadState
    {
        // Condition 1
        // exit criteria for this thread,
        // logger has been stopped and queue is empty
        if (!this->loggerStarted && currQueueStatus.fillLevel == 0)
        {
            return EXIT;
        }

        // Condition 2
        // logger has been stopped but queue still contains
        // items, thus log out remaining messages in Queue
        if (!this->loggerStarted && currQueueStatus.fillLevel > 0)
        {
            // speed up logging out to terminate this thread quickly
            newLogThreadPeriod = Logger::LOG_MIN_THREAD_PERIOD_US;
            newLogsAtOnce = Logger::LOG_MAX_LOGS_AT_ONCE;

            return LOG;
        }

        // Condition 3
        // logger is started but queue is empty, stay prepared
        if (this->loggerStarted && currQueueStatus.fillLevel == 0)
        {
            newLogThreadPeriod = Logger::DEFAULT_THREAD_PERIOD_US;
            newLogsAtOnce = Logger::LOG_DEFAULT_LOGS_AT_ONCE;

            return WAIT;
        }

        // Condition 4
        // logger is started and queue contains messages
        // Check Log-Queuestaus and adjust thread period and number of logs to log at once
        // This is only supported if Queue-Monitoring was activated
        if (this->logQMonEnabled && currQueueStatus.fillLevel > 0)
        {
            // This is a linear function to determin a thread period in microseconds depending on the queue fill-level.
            // It calculates a shift value between in a way to calculate the period time as 2^shift
            // between Logger::LOG_MIN_THREAD_PERIOD_US and Logger::LOG_MAX_THREAD_PERIOD_US
            // The shortest period shall be reached at Logger::LOG_MESSAGE_RED_WMARK_PERCENT
            // In order to not loose precision we temporarily shift up by eight and down later after the division
            unsigned short slope = ((Logger::LOG_MAX_THREAD_PERIOD_LD - Logger::LOG_MIN_THREAD_PERIOD_LD) * 100) / Logger::LOG_MESSAGE_RED_WMARK_PERCENT;
            unsigned short shift = ((((slope * currQueueStatus.fillLevel) << 7) / Logger::LOG_MESSAGE_QUEUE_SIZE) >> 7);
            shift = Logger::LOG_MAX_THREAD_PERIOD_LD - shift;
            shift = std::clamp(static_cast<unsigned short>(shift), Logger::LOG_MIN_THREAD_PERIOD_LD, Logger::LOG_MAX_THREAD_PERIOD_LD);

            if (currQueueStatus.fillLevel >= Logger::LOG_MESSAGE_QUEUE_SIZE)
            {
                shift = Logger::LOG_MIN_THREAD_PERIOD_LD;
            }

            // finally the new log thread period is the average of the new and previous one
            newLogThreadPeriod = (std::clamp(static_cast<unsigned long>(1 << shift), Logger::LOG_MIN_THREAD_PERIOD_US, Logger::LOG_MAX_THREAD_PERIOD_US) + prevLogThreadPeriod) >> 1;
            prevLogThreadPeriod = newLogThreadPeriod;

            // This is a linear function to determine the number of log message to output at once from the queue
            // It takes the current queue fill level as input and returns a fourth of it. The return is clamped.
            newLogsAtOnce = (std::clamp(static_cast<unsigned short>(currQueueStatus.fillLevel >> 2), Logger::LOG_MIN_LOGS_AT_ONCE, Logger::LOG_MAX_LOGS_AT_ONCE) + prevLogsAtOnce) >> 1;
            prevLogsAtOnce = newLogsAtOnce;

            return LOG;
        }

        // Condition 5
        // logger is started and queue contains messages
        // keep thread period an logs at once constant
        if (!this->logQMonEnabled && currQueueStatus.fillLevel > 0)
        {
            newLogThreadPeriod = Logger::DEFAULT_THREAD_PERIOD_US;
            newLogsAtOnce = Logger::LOG_DEFAULT_LOGS_AT_ONCE;

            return LOG;
        }

        return LOG;
    };

    // Log thread loop
    while (true)
    {
        currQueueStatus = getLogQStat();

        threadState state = setThreadParams2();

        if (state == EXIT)
        {
            break;
        }

        //std::cout << "Queue-Fill: " << std::setw(4) << currQueueStatus.fillLevel << " Th.Period: " << std::setw(6) << newLogThreadPeriod << " LogsAtOnce: " << std::setw(4) << newLogsAtOnce << std::endl;

        if (state == WAIT)
        {
            continue;
        }

        for (int i = 0; i < newLogsAtOnce; i++)
        {
            if (this->logOutNextMessage() == 0)
            {
                break;
            }
        }

        boost::this_thread::sleep_for(boost::chrono::microseconds(newLogThreadPeriod));
    }
}

std::ofstream* Logger::getNewLogFile(unsigned short fileCounter)
{
    boost::chrono::system_clock::time_point now = boost::chrono::system_clock::now();
    std::string date = Logger::getTimeStr(now, Logger::TimeStampProperty::DATE);
    std::string time = Logger::getTimeStr(now, Logger::TimeStampProperty::SECS);
    boost::replace_all(time, ":", ".");
    std::stringstream fss;
    fss << date << "_" << time << "_" << std::setw(3) << std::setfill('0') << std::to_string(fileCounter) << ".log";
    std::string logFileName = fss.str();

    return new std::ofstream(logFileName, std::ofstream::out);
}
