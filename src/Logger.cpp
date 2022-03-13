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
Logger::Logger(std::ostream& strm) : iniFileMode(false),
                                     loggerStarted(false),
                                     loggingSuppressed(false),
                                     logChannel(&strm),
                                     logsPerFile(Logger::MAX_LOGS_PER_FILE),
                                     logLevel(Logger::LogLevel::INFO),
                                     logTags(Logger::LogTag::ALL_TAGS_OFF),
                                     timeStampProps(Logger::TimeStampProperty::ALL_PROPS_OFF)
{
}

Logger::Logger(const std::string& configFilename) : iniFileMode(true),
                                                    loggerStarted(false),
                                                    loggingSuppressed(false),
                                                    logChannel(nullptr),
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

std::error_condition Logger::LOG_RESUME(Logger& instance)
{
    return instance.resume();
}

std::error_condition Logger::LOG_SUPPRESS(Logger& instance)
{
    return instance.suppress();
}

std::error_condition Logger::LOG_SET_TAGS(Logger& instance, unsigned char logTags)
{
    return instance.userSetLogTags(logTags);
}

std::error_condition Logger::LOG_SET_LEVEL(Logger& instance, Logger::LogLevel level)
{
    return instance.userSetLogLevel(level);
}

std::error_condition Logger::LOG_SET_TIME_STAMP_PROPS(Logger& instance, unsigned char properties)
{
    return instance.userSetTimeStampProperties(properties);
}

Logger::LogLevel Logger::LOG_GET_LEVEL(Logger& instance)
{
    return instance.getLogLevel();
}

std::error_condition Logger::LOG_TRACE(Logger& instance, std::ostream& messageStream)
{
    return instance.log(Logger::LogLevel::TRACE, messageStream);
}

std::error_condition Logger::LOG_DEBUG(Logger& instance, std::ostream& messageStream)
{
    return instance.log(Logger::LogLevel::DEBUG, messageStream);
}

std::error_condition Logger::LOG_INFO(Logger& instance, std::ostream& messageStream)
{
    return instance.log(Logger::LogLevel::INFO, messageStream);
}

std::error_condition Logger::LOG_WARN(Logger& instance, std::ostream& messageStream)
{
    return instance.log(Logger::LogLevel::WARN, messageStream);
}

std::error_condition Logger::LOG_ERROR(Logger& instance, std::ostream& messageStream)
{
    return instance.log(Logger::LogLevel::ERR, messageStream);
}

std::error_condition Logger::LOG_FATAL(Logger& instance, std::ostream& messageStream)
{
    return instance.log(Logger::LogLevel::FATAL, messageStream);
}

std::string Logger::getCurrentTimeStr(unsigned char properties)
{
    if ((properties | Logger::TimeStampProperty::DATE) || (properties | Logger::TimeStampProperty::SECS))
    {
        // the current time
        std::chrono::system_clock::time_point today = std::chrono::system_clock::now();
        // the current time as epoch time
        std::chrono::system_clock::duration duration = today.time_since_epoch();

        // get duration in different units
        std::chrono::nanoseconds durationNanosecs = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
        unsigned long curSubSecondNs = durationNanosecs.count() % 1000000000;

        // transform current time to time_t format
        time_t in_time_t = std::chrono::system_clock::to_time_t(today);

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

std::ostream Logger::nirvana(NULL);
unsigned short const Logger::MIN_LOGS_PER_FILE = 100;
unsigned short const Logger::MAX_LOGS_PER_FILE = 10000;
std::string const Logger::logLevel2String[] = {"TRACE", "DEBUG", "INFO ", "WARN ", "ERROR", "FATAL"};

// instance members
std::ostream& Logger::getMsgStream()
{
    return this->userMessageStream;
}

/**
 * @brief Provides the logging metod for the user.
 *
 * @param level The log message category of type Logger::LogLevel
 * @param msg The log message as a stream
 */
std::error_condition Logger::log(Logger::LogLevel level, const std::ostream& msg)
{
    std::error_condition errCond(0, std::generic_category());

    if (this->loggerStarted)
    {
        if (!this->loggingSuppressed && level >= this->logLevel)
        {
            std::lock_guard<std::mutex> lock(this->logMtx);
            this->logInCounter++;

            if (this->logTags & Logger::LogTag::COUNTER)
            {
                this->fullMessageStream << "[" << std::setw(5) << std::setfill('0') << this->logInCounter << "]";
            }

            if ((this->logTags & Logger::LogTag::TIME_STAMP) &&
                (this->timeStampProps != Logger::TimeStampProperty::ALL_PROPS_OFF))
            {
                std::string timeStr = this->getCurrentTimeStr(this->timeStampProps);

                if (!timeStr.empty())
                {
                    this->fullMessageStream << "[" << timeStr << "]";
                }
            }

            if (this->logTags & Logger::LogTag::LEVEL)
            {
                this->fullMessageStream << "[" << Logger::logLevel2String[level] << "]";
            }

            if (this->logTags != Logger::LogTag::ALL_TAGS_OFF)
            {
                this->fullMessageStream << " ";
            }

            this->fullMessageStream << this->userMessageStream.str() << std::endl;

            this->logMessageOutputQueue.emplace(this->fullMessageStream.str());

            // discard the streams
            this->fullMessageStream.str(std::string("\r"));
            Logger::nirvana << this->fullMessageStream.str();
            this->userMessageStream.str(std::string("\r"));
            Logger::nirvana << this->userMessageStream.str();
        }
    }
    else
    {
        errCond = std::errc::operation_not_permitted;
    }

    return errCond;
}

void Logger::start()
{
    this->logInCounter = 0;
    this->logFileCounter = 0;
    this->logOutCounter = 1;
    this->loggerStarted = true;
    this->logThreadHandle = std::thread(&Logger::logThread, this);
}

std::error_condition Logger::stop()
{
    std::error_condition errCond(0, std::generic_category());

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

        errCond = std::errc::operation_canceled;
    }

    return errCond;
}

std::error_condition Logger::resume()
{
    std::error_condition errCond(0, std::generic_category());

    if (this->loggerStarted)
    {
        this->loggingSuppressed = false;
    }
    else
    {
        errCond = std::errc::operation_not_permitted;
    }

    return errCond;
}

std::error_condition Logger::suppress()
{
    std::error_condition errCond(0, std::generic_category());

    if (this->loggerStarted)
    {
        this->loggingSuppressed = true;
    }
    else
    {
        errCond = std::errc::operation_not_permitted;
    }

    return errCond;
}

std::error_condition Logger::userSetLogTags(unsigned char logTags)
{
    std::error_condition errCond(0, std::generic_category());

    if (this->iniFileMode)
    {
        errCond = std::errc::operation_not_permitted;
    }
    else
    {
        this->setLogTags(logTags);
    }

    return errCond;
}

std::error_condition Logger::userSetLogLevel(Logger::LogLevel level)
{
    std::error_condition errCond(0, std::generic_category());

    if (this->iniFileMode)
    {
        errCond = std::errc::operation_not_permitted;
    }
    else
    {
        this->setLogLevel(level);
    }

    return errCond;
}

std::error_condition Logger::userSetTimeStampProperties(unsigned char properties)
{
    std::error_condition errCond(0, std::generic_category());

    if (this->iniFileMode)
    {
        errCond = std::errc::operation_not_permitted;
    }
    else
    {
        this->setTimeStampProperties(properties);
    }

    return errCond;
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

void Logger::logMessagesInOutputQueue()
{
    // log next message in queue to logChannel
    while (!this->logMessageOutputQueue.empty())
    {
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

        // log to channel, remove queue element and increment the log-out counter
        *(this->logChannel) << this->logMessageOutputQueue.front();
        this->logMessageOutputQueue.pop();
        this->logOutCounter++;
    }
}

std::error_condition Logger::parseConfigFile(const std::string& configFilename)
{
    std::error_condition errCond(0, std::generic_category());
    std::ifstream iniFs;

    iniFs.open(configFilename.c_str(), std::ifstream::in);

    if (iniFs.fail())
    {
        iniFs.close();

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

            iniFs.close();

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

                iniFs.close();

                return std::errc::invalid_seek;
            }
        }
        catch (const boost::property_tree::ptree_error& e)
        {
            std::cerr << "Configfile parse exception: " << e.what() << std::endl;

            iniFs.close();

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

            // this->logChannel = this->getNewLogFile(++this->logFileCounter);
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
    }

    return errCond;
}

void Logger::logThread()
{
    while (this->loggerStarted || !this->logMessageOutputQueue.empty())
    {
        std::this_thread::sleep_for(std::chrono::microseconds(1000));
        std::lock_guard<std::mutex> lock(this->logMtx);
        this->logMessagesInOutputQueue();
    }
}

std::ofstream* Logger::getNewLogFile(unsigned short fileCounter)
{
    std::string date = Logger::getCurrentTimeStr(Logger::TimeStampProperty::DATE);
    std::string time = Logger::getCurrentTimeStr(Logger::TimeStampProperty::SECS);
    boost::replace_all(time, ":", ".");
    std::stringstream fss;
    fss << date << "_" << time << "_" << std::setw(3) << std::setfill('0') << std::to_string(fileCounter) << ".log";
    std::string logFileName = fss.str();

    return new std::ofstream(logFileName, std::ofstream::out);
}