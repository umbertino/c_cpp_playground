// Boost-Includes
#include <boost/chrono.hpp>
#include <boost/container/vector.hpp>
#include <boost/format.hpp>
#include <boost/scoped_ptr.hpp>

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
#define LOGGER_EXAMPLE 1

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

#if LOGGER_EXAMPLE

    Logger myLogger("logging.ini"); //(new Logger(std::clog));
    //Logger myLogger(std::clog);

    std::ofstream ofs("test.txt", std::ofstream::out);

    //Logger myLogger(ofs);

    myLogger.start();

    std::cout << std::endl;

    //myLogger.setLogTags(Logger::LogTag::COUNTER | Logger::LogTag::TIME_STAMP);
    //myLogger.setLogLevel(Logger::LogLevel::INFO);
    //myLogger.setTimeStamp(Logger::TimeStampProperty::DATE | Logger::TimeStampProperty::NANOSECS);

    myLogger.log(Logger::LogLevel::TRACE) << __BASENAME__ << " 1 This is a trace message";
    myLogger.log(Logger::LogLevel::FATAL) << __FILE__ << " 2 This is a fatal message";
    myLogger.suppress();
    myLogger.log(Logger::LogLevel::DEBUG) << __LINE__ << " 3 This is a debug message";
    myLogger.log(Logger::LogLevel::WARN) << __FUNCTION__ << " 4 This is a warning message";
    myLogger.resume();
    myLogger.log(Logger::LogLevel::ERR) << __FILE_EXT__ << " 5 This is an error message";
    myLogger.log(Logger::LogLevel::INFO) << __LOCATION__ << " 6 This is a info message";

    std::cout << std::endl;

    //Logger::LOG_SET_TAGS(myLogger, Logger::LogTag::COUNTER | Logger::LogTag::TIME_STAMP | Logger::LogTag::LEVEL);
    //Logger::LOG_SET_LEVEL(myLogger, Logger::TRACE);
    //Logger::LOG_SET_TIME_STAMP(myLogger, Logger::TimeStampProperty::TIME | Logger::TimeStampProperty::MICROSECS);

    Logger::LOG_TRACE(myLogger) << __BASENAME__ << " 1 This is a trace message";
    Logger::LOG_FATAL(myLogger) << __FILE__ << " 2 This is a fatal message";
    Logger::LOG_SUPPRESS(myLogger);
    Logger::LOG_DEBUG(myLogger) << __LINE__ << " 3 This is a debug message";
    Logger::LOG_WARN(myLogger) << __FUNCTION__ << " 4 This is a warning message";
    Logger::LOG_RESUME(myLogger);
    Logger::LOG_ERROR(myLogger) << __FILE_EXT__ << " 5 This is an error message";
    Logger::LOG_INFO(myLogger) << __LOCATION__ << " 6 This is a fatal message";

    myLogger.stop();

    ofs.close();

    std::cout << std::endl;
#endif

    return 0;
}
