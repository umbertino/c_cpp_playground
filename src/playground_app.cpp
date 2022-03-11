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
#include <queue>
#include <thread>

// Own Includes
#include "Prime.h"
#include "ScopeGuard.h"
#include "Logger.h"

// switches to activate / deactivate examples
#define SCRATCH_PAD 0
#define PRIME_EXAMPLE 0
#define SCOPE_GUARD_EXAMPLE 0
#define LOGGER_EXAMPLE 1
#define QUEUE_EXAMPLE 0

int main(void)
{
    std::cout << "Hello, this is a C++ playground" << std::endl
              << std::endl;

#if SCRATCH_PAD
    std::chrono::nanoseconds epochDur = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());

    //std::cout << std::put_time(std::localtime(&secsSinceEpoch), "%H:%M") << std::endl;

    std::cout << epochDur.count();
    std::cout << std::endl;
    unsigned long nanoSecs = epochDur.count() % 1000000000;
    std::cout << std::setw(3) << std::setfill('0') << (nanoSecs / 1000000) << "."
              << std::setw(3) << std::setfill('0') << (nanoSecs % 1000000) / 1000 << "."
              << std::setw(3) << std::setfill('0') << (nanoSecs % 1000000) % 1000;

    std::cout << std::endl;
#endif

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

    //std::ofstream ofs("test.txt", std::ofstream::out);

    //Logger myLogger(ofs);

    std::chrono::duration<int, std::micro> dur[1000];
    //std::chrono::system_clock::time_point start, stop;

    myLogger.start();

    std::cout << std::endl;

    // myLogger.setLogTags(Logger::LogTag::COUNTER | Logger::LogTag::TIME_STAMP);
    // myLogger.setLogLevel(Logger::LogLevel::INFO);
    // myLogger.setTimeStampProperties(Logger::TimeStampProperty::DATE | Logger::TimeStampProperty::NANOSECS);

    myLogger.log(Logger::LogLevel::TRACE, GMS(myLogger) << __LOCATION__ << " 1 This is a trace message");
    myLogger.log(Logger::LogLevel::FATAL, GMS(myLogger) << __LOCATION__ << " 2 This is a fatal message");
    myLogger.suppress();
    myLogger.log(Logger::LogLevel::DEBUG, GMS(myLogger) << __LOCATION__ << " 3 This is a debug message");
    myLogger.log(Logger::LogLevel::WARN, GMS(myLogger) << __LOCATION__ << " 4 This is a warning message");
    myLogger.resume();
    myLogger.log(Logger::LogLevel::ERR, GMS(myLogger) << __LOCATION__ << " 5 This is an error message");
    myLogger.log(Logger::LogLevel::INFO, GMS(myLogger) << __LOCATION__ << " 6 This is a info message");

    std::cout << std::endl;

    //Logger::LOG_SET_TAGS(myLogger, Logger::LogTag::COUNTER | Logger::LogTag::TIME_STAMP | Logger::LogTag::LEVEL);
    //Logger::LOG_SET_LEVEL(myLogger, Logger::TRACE);
    //Logger::LOG_SET_TIME_STAMP(myLogger, Logger::TimeStampProperty::TIME | Logger::TimeStampProperty::MICROSECS);

    Logger::LOG_TRACE(myLogger, GMS(myLogger) << __LOCATION__ << " 1 This is a trace message");
    Logger::LOG_FATAL(myLogger, GMS(myLogger) << __LOCATION__ << " 2 This is a fatal message");
    Logger::LOG_SUPPRESS(myLogger);
    Logger::LOG_DEBUG(myLogger, GMS(myLogger) << __LOCATION__ << " 3 This is a debug message");
    Logger::LOG_WARN(myLogger, GMS(myLogger) << __LOCATION__ << " 4 This is a warning message");
    Logger::LOG_RESUME(myLogger);
    Logger::LOG_ERROR(myLogger, GMS(myLogger) << __LOCATION__ << " 5 This is an error message");
    Logger::LOG_INFO(myLogger, GMS(myLogger) << __LOCATION__ << " 6 This is a fatal message");

    for (int i = 0; i < 1000; i++)
    {
        //std::this_thread::sleep_for(std::chrono::microseconds(1));
        auto start = std::chrono::high_resolution_clock::now();
        Logger::LOG_INFO(myLogger, GMS(myLogger) << __LOCATION__ << " This is a fatal message " << i);
        auto stop = std::chrono::high_resolution_clock::now();

        dur[i] = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    }

    Logger::LOG_WARN(myLogger, GMS(myLogger) << __LOCATION__ << " 5 This is an error message");
    Logger::LOG_WARN(myLogger, GMS(myLogger) << __LOCATION__ << " 6 This is a fatal message");

    myLogger.stop();

    float avgDur(0);
    const unsigned short LOOP = 1000;

    for (int i = 0; i < LOOP; i++)
    {
        //std::cout << std::setw(4) << i << " " << dur[i].count() << "us" << std::endl;

        avgDur = avgDur + dur[i].count();
    }

    avgDur = avgDur / LOOP;

    std::cout << "avgDur: " << avgDur << std::endl;

    //ofs.close();

    std::cout
        << std::endl;
#endif

#if QUEUE_EXAMPLE
    std::queue<std::ostringstream> mq;

    mq.emplace(std::ostringstream(""));
    mq.emplace(std::ostringstream("world "));

    std::cout << mq.front().str() << std::endl;

    mq.front() << "gone mad ";

    std::cout << mq.front().str() << std::endl;

    mq.front() << "and crazy";

    std::cout << mq.front().str() << std::endl;

    mq.front() << " for all people";

    std::cout << mq.front().str() << std::endl;

    std::cout << std::endl;
#endif

    return 0;
}
