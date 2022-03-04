// Boost-Includes
// #include <boost/thread.hpp>
#include <boost/chrono.hpp>
// #include <boost/scoped_ptr.hpp>
// #include <boost/timer/timer.hpp>

// #include <boost/log/core.hpp>
// #include <boost/log/trivial.hpp>
// #include <boost/log/expressions.hpp>
// #include <boost/log/sinks/text_file_backend.hpp>
// #include <boost/log/utility/setup/file.hpp>
// #include <boost/log/utility/setup/common_attributes.hpp>
// #include <boost/log/utility/setup/settings_parser.hpp>
// #include <boost/log/utility/setup/from_stream.hpp>
// #include <boost/log/utility/setup/console.hpp>
// #include <boost/log/sources/severity_logger.hpp>
// #include <boost/log/sources/record_ostream.hpp>

// #include <boost/property_tree/ptree.hpp>
// #include <boost/property_tree/ini_parser.hpp>

#include <boost/container/vector.hpp>
#include <boost/format.hpp>

// Std-Includes
#include <iostream>
#include <ostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>

// Own Includes
#include "Prime.h"
#include "ScopeGuard.h"
#include "Logger.h"

// switches to activate / deactivate examples
#define PRIME_EXAMPLE 0
#define SCOPE_GUARD_EXAMPLE 0
#define BOOST_TIMER_EXAMPLE 0
#define LOGGER_EXAMPLE 1
#define BOOST_LOGGING_EXAMPLE 0
#define INI_FILE_PARSER 0


void handler();

int main(void)
{
    std::cout << "Hello, this is a C++ playground" << std::endl
              << std::endl;

#if PRIME_EXAMPLE
    unsigned long number = 987654321;

    // simple prime check of a number
    bool isPrime = Prime::checkForPrime(number);

    if (isPrime)
    {
        std::cout << number << " is a prime number." << std::endl;
    }
    else
    {
        std::cout << number << " is not a prime number." << std::endl;
    }

    // determine all prime numbers in a specified range
    unsigned long start = 0, end = 1000;
    boost::container::vector<unsigned long> primeList;

    if (Prime::getRangeOfPrimes(start, end, primeList))
    {
        std::cout << "Found " << primeList.size() << " prime numbers in the range [" << start << "," << end << "]: ";

        for (const unsigned long& i : primeList)
        {
            std::cout << i << " ";
        }
    }
    else
    {
        std::cout << "No prime numbers found in the range [" << start << "," << end << "]: ";
    }

    std::cout << std::endl;

    // factorize a number
    number = 999999999;

    if (Prime::primeFactorize(number, primeList))
    {
        std::cout << "Prime factors for " << number << " are: ";

        for (const unsigned long& i : primeList)
        {
            std::cout << i << " ";
        }

        std::cout << std::endl;
    }
    else
    {
        std::cout << "Could not factorize " << number << std::endl;
    }

    number = 10001;

    std::cout << "The " << number << ". prime number is " << Prime::getNthPrime(number) << std::endl;

    std::cout << "The next prime number right from " << number << " is " << Prime::getNextPrime(number) << std::endl;

    std::cout << std::endl;
#endif

#if SCOPE_GUARD_EXAMPLE
    std::cout << "RAII example, exception safe scope guard" << std::endl;

    try
    {
        // usage with helper macro (scope is created by macro)
        SUPPRESS_LOGGING(true, false, true,
                         {
                             std::cout << "Scoped output V1" << std::endl;
                             throw std::invalid_argument("received negative value");
                         })
    }
    catch (const std::invalid_argument& e)
    {
        std::cerr << "Std-Exception caught: " << e.what() << std::endl;
    }

    std::cout << std::endl;

    try
    {
        // usage with scoped pointer and curly block braces
        { // this is the start of the scope
            const boost::scoped_ptr<ScopeGuard> scopeGuard(new ScopeGuard(true, false, true));

            boost::container::vector<int> myvector(10);

            myvector.at(20) = 100; // vector::at throws an out-of-range

        } // this is the end of the scope
    }
    catch (const boost::container::out_of_range& e)
    {
        std::cerr << "Boost-Exception caught: " << e.what() << std::endl;
    }

    std::cout << std::endl;
#endif

#if BOOST_TIMER_EXAMPLE
    {
        // boost::asio::io_context io_context;
        // boost::asio::deadline_timer timer(io_context, boost::posix_time::seconds(10));
        // timer.async_wait(handler);
    }

    std::cout << std::endl;
#endif

#if BOOST_LOGGING_EXAMPLE
    // namespace logging = boost::log;
    // namespace sinks = boost::log::sinks;
    // namespace src = boost::log::sources;
    // namespace expr = boost::log::expressions;
    // namespace attrs = boost::log::attributes;
    // namespace keywords = boost::log::keywords;
    // namespace filters = boost::log::filters;

    // Read logging settings from a file
    // std::ifstream file("logging.ini", std::ifstream::in);
    // auto settings = boost::log::parse_settings(file);
    // boost::log::init_from_stream(file);

    boost::log::sources::severity_logger<boost::log::trivial::severity_level> consoleLogger;
    boost::log::sources::severity_logger<boost::log::trivial::severity_level> fileLogger;

    // Set console-logging settings
    boost::log::add_console_log(std::cout, boost::log::keywords::format = "[%TimeStamp%][%Severity%]: %Message%");

    // Set file-logging settings
    boost::log::add_file_log(
        boost::log::keywords::file_name = "sample_%N.log",
        boost::log::keywords::open_mode = std::ios_base::out | std::ios_base::app,
        boost::log::keywords::rotation_size = 10 * 1024 * 1024,
        boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
        boost::log::keywords::format = "[%TimeStamp%][%Severity%]: %Message%");

    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::info);
    boost::log::add_common_attributes();

    BOOST_LOG_SEV(consoleLogger, boost::log::trivial::trace) << "A trace severity message";
    BOOST_LOG_SEV(consoleLogger, boost::log::trivial::debug) << "A debug severity message";
    BOOST_LOG_SEV(consoleLogger, boost::log::trivial::info) << "An informational severity message";
    BOOST_LOG_SEV(consoleLogger, boost::log::trivial::warning) << "A warning severity message";
    BOOST_LOG_SEV(consoleLogger, boost::log::trivial::error) << "An error severity message";
    BOOST_LOG_SEV(consoleLogger, boost::log::trivial::fatal) << "A fatal severity message";

    BOOST_LOG_SEV(fileLogger, boost::log::trivial::trace) << "A trace severity message";
    BOOST_LOG_SEV(fileLogger, boost::log::trivial::debug) << "A debug severity message";
    BOOST_LOG_SEV(fileLogger, boost::log::trivial::info) << "An informational severity message";
    BOOST_LOG_SEV(fileLogger, boost::log::trivial::warning) << "A warning severity message";
    BOOST_LOG_SEV(fileLogger, boost::log::trivial::error) << "An error severity message";
    BOOST_LOG_SEV(fileLogger, boost::log::trivial::fatal) << "A fatal severity message";

    std::cout << std::endl;
#endif

#if LOGGER_EXAMPLE

    //Logger myLogger("logging.ini"); //(new Logger(std::clog));
    Logger myLogger(std::clog);

    //std::stringstream logStrm;

    myLogger.start();
    //myLogger2.start();
    // myLogger.boostLog(Logger::LogLevel::TRACE, logStrm << __BASENAME__ << " 1 This is a trace message");
    // myLogger.boostLog(Logger::LogLevel::FATAL, logStrm << __FILE__ << " 2 This is a fatal message");
    // myLogger.boostLog(Logger::LogLevel::DEBUG, logStrm << __LINE__ << " 3 This is a debug message");
    // myLogger.boostLog(Logger::LogLevel::WARN, logStrm << __FUNCTION__ << " 4 This is a warning message");
    // myLogger.boostLog(Logger::LogLevel::ERR, logStrm << __FILE_EXT__ << " 5 This is an error message");
    // myLogger.boostLog(Logger::LogLevel::INFO, logStrm << __LOCATION__ << " 6 This is a fatal message");

    std::cout << std::endl;

    myLogger.setLogLevel(Logger::LogLevel::INFO);

    myLogger.log(Logger::LogLevel::TRACE) << __BASENAME__ << " 1 This is a trace message" << std::endl;
    myLogger.log(Logger::LogLevel::FATAL) << __FILE__ << " 2 This is a fatal message" << std::endl;
    myLogger.suppress();
    myLogger.log(Logger::LogLevel::DEBUG) << __LINE__ << " 3 This is a debug message" << std::endl;
    myLogger.log(Logger::LogLevel::WARN) << __FUNCTION__ << " 4 This is a warning message" << std::endl;
    myLogger.resume();
    myLogger.log(Logger::LogLevel::ERR) << __FILE_EXT__ << " 5 This is an error message" << std::endl;
    myLogger.log(Logger::LogLevel::INFO) << __LOCATION__ << " 6 This is a fatal message" << std::endl;

    std::cout << std::endl;

    // myLogger2.setLogLevel(Logger::LogLevel::INFO);

    // myLogger2.log(Logger::LogLevel::TRACE) << __BASENAME__ << " 1 This is a trace message" << std::endl;
    // myLogger2.log(Logger::LogLevel::FATAL) << __FILE__ << " 2 This is a fatal message" << std::endl;
    // myLogger2.suppress();
    // myLogger2.log(Logger::LogLevel::DEBUG) << __LINE__ << " 3 This is a debug message" << std::endl;
    // myLogger2.log(Logger::LogLevel::WARN) << __FUNCTION__ << " 4 This is a warning message" << std::endl;
    // myLogger2.resume();
    // myLogger2.log(Logger::LogLevel::ERR) << __FILE_EXT__ << " 5 This is an error message" << std::endl;
    // myLogger2.log(Logger::LogLevel::INFO) << __LOCATION__ << " 6 This is a fatal message" << std::endl;

    std::cout << std::endl;

    Logger::LOG_SET_TAGS(myLogger, Logger::LogTag::COUNTER | Logger::LogTag::LEVEL | Logger::LogTag::TIME_STAMP);
    Logger::LOG_SET_LEVEL(myLogger, Logger::TRACE);
    Logger::LOG_TRACE(myLogger) << __BASENAME__ << " 1 This is a trace message" << std::endl;
    Logger::LOG_FATAL(myLogger) << __FILE__ << " 2 This is a fatal message" << std::endl;
    Logger::LOG_SUPPRESS(myLogger);
    Logger::LOG_DEBUG(myLogger) << __LINE__ << " 3 This is a debug message" << std::endl;
    Logger::LOG_WARN(myLogger) << __FUNCTION__ << " 4 This is a warning message" << std::endl;
    Logger::LOG_RESUME(myLogger);
    Logger::LOG_ERROR(myLogger) << __FILE_EXT__ << " 5 This is an error message" << std::endl;
    Logger::LOG_INFO(myLogger) << __LOCATION__ << " 6 This is a fatal message" << std::endl;

    myLogger.stop();

    std::cout << std::endl;
#endif

#if INI_FILE_PARSER
    std::chrono::system_clock::time_point today = std::chrono::system_clock::now();
    time_t in_time_t = std::chrono::system_clock::to_time_t(today);

    std::chrono::system_clock::duration duration = today.time_since_epoch();

    std::chrono::seconds durationSecs = std::chrono::duration_cast<std::chrono::seconds>(duration);
    std::chrono::milliseconds durationMiliSecs = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    std::chrono::microseconds durationMicroSecs = std::chrono::duration_cast<std::chrono::microseconds>(duration);
    std::chrono::nanoseconds durationNanosecs = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);

    std::chrono::milliseconds msCurrSec = durationMiliSecs - std::chrono::duration_cast<std::chrono::milliseconds>(durationSecs);
    std::chrono::microseconds usCurrSec = durationMicroSecs - std::chrono::duration_cast<std::chrono::microseconds>(durationMiliSecs);
    std::chrono::nanoseconds nsCurrSec = durationNanosecs - std::chrono::duration_cast<std::chrono::nanoseconds>(durationMicroSecs);

    //std::chrono::microseconds miliSecs = std::chrono::duration_cast<std::chrono::nanoseconds>(nsCurrSec);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d"); // Date
    ss << " " << std::put_time(std::localtime(&in_time_t), "%X"); // Time
    ss << "." << durationNanosecs.count();
    ss << "." << std::setw(3) << std::setfill('0') << msCurrSec.count(); // miliseconds
    ss << "." << std::setw(3) << std::setfill('0') << usCurrSec.count(); // microseconds
    ss << "." << std::setw(3) << std::setfill('0') << nsCurrSec.count(); // nanoseconds

    std::cout << ss.str() << std::endl;
#endif

    return 0;
}

void handler()
{
    std::cout << "Timer expired" << std::endl;
}