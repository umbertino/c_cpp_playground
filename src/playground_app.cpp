// Boost-Includes
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/timer/timer.hpp>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/settings_parser.hpp>
#include <boost/log/utility/setup/from_stream.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>

#include <boost/container/vector.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_context.hpp>

// Std-Includes
#include <iostream>
#include <ostream>
#include <fstream>
#include <stdexcept>
#include <string>

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

    Logger* myLogger(new Logger(std::clog));
    std::stringstream logStrm;

    myLogger->start();

    myLogger->log(Logger::logLevel::TRACE, logStrm << __BASENAME__ << " 1 This is a trace message");
    myLogger->log(Logger::logLevel::FATAL, logStrm << __FILE__ << " 2 This is a fatal message");
    myLogger->log(Logger::logLevel::DEBUG, logStrm << __LINE__ << " 3 This is a debug message");
    myLogger->log(Logger::logLevel::WARN, logStrm << __FUNCTION__ << " 4 This is a warning message");
    myLogger->log(Logger::logLevel::ERR, logStrm << __FILE_EXT__ << " 5 This is an error message");
    // myLogger->log(Logger::logLevel::INFO, logStrm << __LOCATION__ << " 6 This is a fatal message");

    std::cout << std::endl;

    myLogger->suppress();

    myLogger->trace(logStrm << __BASENAME__ << " 7 This is a trace message");
    myLogger->fatal(logStrm << __FILE__ << " 8 This is a trace message");
    myLogger->debug(logStrm << __LINE__ << " 9 This is a debug message");
    myLogger->warn(logStrm << __FUNCTION__ << " 10 This is a warning message");

    myLogger->resume();

    myLogger->error(logStrm << __FILE_EXT__ << " 11 This is an error message");
    //myLogger->info(logStrm << __LOCATION__ << " 12 This is a fatal message");

    myLogger->stop();

    std::cout << std::endl;
#endif

    return 0;
}

void handler()
{
    std::cout << "Timer expired" << std::endl;
}