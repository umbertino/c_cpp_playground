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
std::ostream Logger::nirvana(NULL);
const std::string Logger::logLevel2String[] = {"TRACE", "DEBUG", "INFO ", "WARN ", "ERROR", "FATAL"};

// constructors and destructors
Logger::Logger(std::ostream& strm) : iniFileMode(false),
                                     logQMonEnabled(false),
                                     logQOverloadWait(true),
                                     loggerStarted(false),
                                     loggingSuppressed(false),
                                     logChannel(&strm),
                                     logsPerFile(Logger::MAX_LOGS_PER_FILE),
                                     logLevel(Logger::LogLevel::INFO),
                                     logTags(Logger::LogTag::ALL_TAGS_OFF),
                                     timeStampProps(Logger::TimeStampProperty::ALL_PROPS_OFF),
                                     logThreadPeriod(Logger::GREEN_LOG_THREAD_PERIOD_US)
{
}

Logger::Logger(const std::string& configFilename) : iniFileMode(true),
                                                    logQMonEnabled(false),
                                                    logQOverloadWait(true),
                                                    loggerStarted(false),
                                                    loggingSuppressed(false),
                                                    logChannel(nullptr),
                                                    logsPerFile(Logger::MAX_LOGS_PER_FILE),
                                                    logLevel(Logger::LogLevel::INFO),
                                                    logTags(Logger::LogTag::ALL_TAGS_OFF),
                                                    timeStampProps(Logger::TimeStampProperty::ALL_PROPS_OFF),
                                                    logThreadPeriod(Logger::GREEN_LOG_THREAD_PERIOD_US)
{
    std::error_code error = this->parseConfigFile(configFilename);

    if (error.value() != 0)
    {
        std::cerr << "Falling back to default console logger" << std::endl;
        // fall back to a default console logger
        this->iniFileMode = false;
        this->logChannel = &std::clog;
        this->logLevel = Logger::LogLevel::INFO;
        this->logTags = Logger::LogTag::ALL_TAGS_OFF;
        this->timeStampProps = Logger::TimeStampProperty::ALL_PROPS_OFF;
    }
}

Logger::~Logger()
{
    this->logChannel->flush();

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

std::error_code Logger::LOG_RESUME(Logger& instance)
{
    return instance.resume();
}

std::error_code Logger::LOG_SUPPRESS(Logger& instance)
{
    return instance.suppress();
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
    return instance.getLogLevel();
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
        //auto start = std::chrono::high_resolution_clock::now();
        // the current time
        //std::chrono::system_clock::time_point today = std::chrono::system_clock::now();
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
        // auto stop = std::chrono::high_resolution_clock::now();
        // std::chrono::duration<int, std::micro> dur = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        // unsigned long dura = dur.count();
        // std::cout << dura << std::endl;

        return ss.str();
    }
    else
    {
        return "";
    }
}

// instance members
std::ostream& Logger::getMsgStream()
{
    return this->userMessageStream;
}

/**
 * @brief Provides the logging metod for the user.
 *
 * @param level The userLog message category of type Logger::LogLevel
 * @param msg The userLog message as a stream
 */
std::error_code Logger::userLog(Logger::LogLevel level, const std::ostream& msg)
{
    if (this->loggerStarted)
    {
        std::error_code ec(0, std::generic_category());

        if (!this->loggingSuppressed && level >= this->logLevel)
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

        // discard the stream
        this->userMessageStream.str(std::string("\r"));
        Logger::nirvana << this->userMessageStream.str();

        return ec;
    }
    else
    {
        return std::make_error_code(std::errc::operation_not_permitted);
    }
}

void Logger::start()
{
    this->logInCounter = 0;
    this->logDiscardCounter = 0;
    this->logFileCounter = 0;
    this->logOutCounter = 0;
    this->loggerStarted = true;
    this->logThreadHandle = boost::thread(&Logger::logThread, this);
}

std::error_code Logger::stop()
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
    *(this->logChannel) << "\n\nLog-Session stopped at "
                        << Logger::getTimeStr(boost::chrono::system_clock::now(), this->timeStampProps) << std::endl
                        << "User Log-Attempts: " << std::setw(10) << this->logInCounter << std::endl
                        << "Successful Logs  : " << std::setw(10) << this->logOutCounter << std::endl
                        << "Discarded Logs   : " << std::setw(10) << this->logDiscardCounter << std::endl
                        << "Plausability     : " << std::setw(10) << ((this->logOutCounter + this->logDiscardCounter == this->logInCounter) ? "OK" : "NOK") << std::endl
                        //<< "Q Write-Avail    : " << std::setw(10) << this->logMessageOutputQueue.read_available() << std::endl
                        << std::endl;

    return std::error_code(0, std::generic_category());
}

std::error_code Logger::resume()
{
    if (this->loggerStarted)
    {
        this->loggingSuppressed = false;
    }
    else
    {
        return std::make_error_code(std::errc::operation_not_permitted);
    }

    return std::error_code(0, std::generic_category());
}

std::error_code Logger::suppress()
{
    if (this->loggerStarted)
    {
        this->loggingSuppressed = true;
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

Logger::LogLevel Logger::getLogLevel()
{
    return this->logLevel;
}

Logger::LogQueueStatus Logger::getLogQueueStatus()
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
        this->fullMessageStream << "[" << Logger::logLevel2String[raw.level] << "]";
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

void Logger::logNextMessage()
{
    if (this->logMessageOutputQueue.read_available() > 0)
    {
        this->logOutCounter++;

        // if we have file logging check, whether we need to create a new file
        // this is the case at the very beginning and when the max allowed
        // logs per file is reached
        if (this->logType == Logger::LogType::FILE)
        {
            if (((this->logOutCounter % this->logsPerFile) == 1))
            {
                if (!(this->logChannel == nullptr))
                {
                    // assure the rest is written to the current
                    // file before getting a new one
                    this->logChannel->flush();
                }

                this->logChannel = Logger::getNewLogFile(++this->logFileCounter);
            }
        }

        // userLog to channel, remove queue element and increment the log-out counter
        *(this->logChannel) << this->formatLogMessage(this->logMessageOutputQueue.front());
        this->logMessageOutputQueue.pop();
    }
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

            // this->logChannel = this->getNewLogFile(++this->logFileCounter);
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
    // start values
    unsigned long logThreadPeriod = Logger::ORANGE_LOG_THREAD_PERIOD_US;
    unsigned short logsAtOnce = 10;
    Logger::LogQueueStatus queueStatus;

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

    auto setThreadParams = [this, &queueStatus, &logThreadPeriod, &logsAtOnce]() -> threadState
    {
        // Condition 1
        // exit criteria for this thread,
        // logger has been stopped and queue is empty
        if (!this->loggerStarted && queueStatus.fillLevel == 0)
        {
            return EXIT;
        }

        // Condition 2
        // logger has been stopped but queue still contains
        // items, thus log out remaining messages in Queue
        if (!this->loggerStarted && queueStatus.fillLevel > 0)
        {
            // speed up logging out to terminate this thread quickly
            logThreadPeriod = Logger::RED_LOG_THREAD_PERIOD_US;
            logsAtOnce = 100;

            return LOG;
        }

        // Condition 3
        // logger is started but queue is empty, stay prepared
        if (this->loggerStarted && queueStatus.fillLevel == 0)
        {
            logThreadPeriod = Logger::ORANGE_LOG_THREAD_PERIOD_US;
            logsAtOnce = 10;

            return WAIT;
        }

        // Condition 4
        // logger is started and queue contains messages
        // Check Log-Queuestaus and adjust thread period and number of logs to log at once
        // This is only supported if Queue-Monitoring was activated
        if (this->logQMonEnabled && queueStatus.fillLevel > 0)
        {
            if (queueStatus.condition == Logger::LogQueueColor::GREEN)
            {
                if (logThreadPeriod < Logger::GREEN_LOG_THREAD_PERIOD_US - 100)
                {
                    // default period for log thread
                    logThreadPeriod = std::clamp(static_cast<unsigned long>(logThreadPeriod + 100), Logger::RED_LOG_THREAD_PERIOD_US, Logger::GREEN_LOG_THREAD_PERIOD_US);
                }

                // log one message per thread call or ramp down to 1
                logsAtOnce = std::clamp(static_cast<unsigned short>(logsAtOnce - 1), static_cast<unsigned short>(1), Logger::LOG_MESSAGE_QUEUE_SIZE);

                return LOG;
            }

            if (queueStatus.condition == Logger::LogQueueColor::ORANGE)
            {
                // decrease the period of log thread
                if (logThreadPeriod >= Logger::RED_LOG_THREAD_PERIOD_US + 100)
                {
                    logThreadPeriod = std::clamp(static_cast<unsigned long>(logThreadPeriod - 100), Logger::RED_LOG_THREAD_PERIOD_US, Logger::GREEN_LOG_THREAD_PERIOD_US);
                }
                else if (logThreadPeriod >= Logger::RED_LOG_THREAD_PERIOD_US + 10)
                {
                    logThreadPeriod = std::clamp(static_cast<unsigned long>(logThreadPeriod - 10), Logger::RED_LOG_THREAD_PERIOD_US, Logger::GREEN_LOG_THREAD_PERIOD_US);
                }
                else
                {
                    logThreadPeriod = std::clamp(static_cast<unsigned long>(logThreadPeriod - 1), Logger::RED_LOG_THREAD_PERIOD_US, Logger::GREEN_LOG_THREAD_PERIOD_US);
                }

                // increase the number of meaaages per thread call by one (ramp up)
                logsAtOnce = std::clamp(static_cast<unsigned short>(logsAtOnce + 1), static_cast<unsigned short>(1), Logger::LOG_MESSAGE_QUEUE_SIZE);

                return LOG;
            }

            if (queueStatus.condition == Logger::LogQueueColor::RED)
            {
                // decrease the period of log thread even more
                if (logThreadPeriod >= Logger::RED_LOG_THREAD_PERIOD_US + 1000)
                {
                    logThreadPeriod = std::clamp(static_cast<unsigned long>(logThreadPeriod - 1000), Logger::RED_LOG_THREAD_PERIOD_US, Logger::GREEN_LOG_THREAD_PERIOD_US);
                }
                else if (logThreadPeriod >= Logger::RED_LOG_THREAD_PERIOD_US + 100)
                {
                    logThreadPeriod = std::clamp(static_cast<unsigned long>(logThreadPeriod - 100), Logger::RED_LOG_THREAD_PERIOD_US, Logger::GREEN_LOG_THREAD_PERIOD_US);
                }
                else if (logThreadPeriod >= Logger::RED_LOG_THREAD_PERIOD_US + 10)
                {
                    logThreadPeriod = std::clamp(static_cast<unsigned long>(logThreadPeriod - 10), Logger::RED_LOG_THREAD_PERIOD_US, Logger::GREEN_LOG_THREAD_PERIOD_US);
                }
                else
                {
                    logThreadPeriod = std::clamp(static_cast<unsigned long>(logThreadPeriod - 1), Logger::RED_LOG_THREAD_PERIOD_US, Logger::GREEN_LOG_THREAD_PERIOD_US);
                }

                // increase the number of meaaages per thread call by 10 (ramp up faster)
                logsAtOnce = std::clamp(static_cast<unsigned short>(logsAtOnce + 10), static_cast<unsigned short>(1), Logger::LOG_MESSAGE_QUEUE_SIZE);

                return LOG;
            }

            return LOG;
        }

        // Condition 5
        // logger is started and queue contains messages
        // keep thread period an logs at once constant
        if (!this->logQMonEnabled && queueStatus.fillLevel > 0)
        {
            logThreadPeriod = Logger::ORANGE_LOG_THREAD_PERIOD_US;
            logsAtOnce = 10;

            return LOG;
        }

        return LOG;
    };

    while (true)
    {
        queueStatus = getLogQStat();

        threadState state = setThreadParams();

        this->logThreadPeriod = logThreadPeriod;

        if (state == EXIT)
        {
            break;
        }

        if (state == WAIT)
        {
            continue;
        }

        // log messages accordingly
        for (int i = 0; i < logsAtOnce; i++)
        {
            this->logNextMessage();
        }

        boost::this_thread::sleep_for(boost::chrono::microseconds(logThreadPeriod));
    }
}

std::ofstream* Logger::getNewLogFile(unsigned short fileCounter)
{
    std::string date = Logger::getTimeStr(boost::chrono::system_clock::now(), Logger::TimeStampProperty::DATE);
    std::string time = Logger::getTimeStr(boost::chrono::system_clock::now(), Logger::TimeStampProperty::SECS);
    boost::replace_all(time, ":", ".");
    std::stringstream fss;
    fss << date << "_" << time << "_" << std::setw(3) << std::setfill('0') << std::to_string(fileCounter) << ".log";
    std::string logFileName = fss.str();

    return new std::ofstream(logFileName, std::ofstream::out);
}