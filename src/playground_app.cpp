// Boost-Includes
#include <boost/chrono.hpp>
#include <boost/thread/thread.hpp>
#include <boost/container/vector.hpp>
#include <boost/format.hpp>
#include <boost/scoped_ptr.hpp>

// Std-Includes
#include <algorithm>
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
    const unsigned short Q_SIZE = 1024;
    unsigned short level = 0;

    unsigned char red_mark = 80;
    unsigned short max_ld = 19;
    unsigned short min_ld = 3;
    unsigned short slope = ((max_ld - min_ld) * 100) / red_mark;
    boost::chrono::nanoseconds t;

    for (level = 0; level <= Q_SIZE; level++)
    {
        boost::chrono::high_resolution_clock::time_point start = boost::chrono::high_resolution_clock::now();

        unsigned short s;

        if (level < Q_SIZE)
        {
            s = ((((slope * level) << 7) / Q_SIZE) >> 7);
            s = max_ld - s;
            s = std::clamp(static_cast<unsigned short>(s), min_ld, max_ld);
        }
        else
        {
            s = min_ld;
        }

        unsigned long period = 1 << s;

        boost::chrono::high_resolution_clock::time_point stop = boost::chrono::high_resolution_clock::now();

        t = boost::chrono::duration_cast<boost::chrono::nanoseconds>(stop - start);

        std::cout << "Level: " << std::setw(4) << level << " Shift: " << std::setw(2) << s << " Period: " << std::setw(6) << (period) << " CalcTime: " << std::setw(6) << t.count() << std::endl;
    }

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

    //std::chrono::system_clock::time_point start, stop;

    myLogger.userStartLog();

    std::cout << std::endl;

    myLogger.userSetLogTags({true, false, true, false});
    myLogger.userSetLogLevel(Logger::LogLevel::INFO);
    myLogger.userSetTimeStampResolution(Logger::TimeStampResolution::NANO);

    myLogger.userLog(Logger::LogLevel::TRACE, GMS(myLogger) << __LOCATION__ << " 1 This is a trace message");
    myLogger.userLog(Logger::LogLevel::FATAL, GMS(myLogger) << __LOCATION__ << " 2 This is a fatal message");
    myLogger.userSuppressLog();
    myLogger.userLog(Logger::LogLevel::DEBUG, GMS(myLogger) << __LOCATION__ << " 3 This is a debug message");
    myLogger.userLog(Logger::LogLevel::WARN, GMS(myLogger) << __LOCATION__ << " 4 This is a warning message");
    myLogger.userResumeLog();
    myLogger.userLog(Logger::LogLevel::ERR, GMS(myLogger) << __LOCATION__ << " 5 This is an error message");
    myLogger.userLog(Logger::LogLevel::INFO, GMS(myLogger) << __LOCATION__ << " 6 This is a info message");

    std::cout << std::endl;

    Logger::LOG_SET_TAGS(myLogger, {true, false, true, false});
    Logger::LOG_SET_LEVEL(myLogger, Logger::TRACE);
    Logger::LOG_SET_TIME_STAMP_RESOLUTION(myLogger, Logger::TimeStampResolution::NANO);

    Logger::LOG_TRACE(myLogger, GMS(myLogger) << __LOCATION__ << " 1 This is a trace message");
    Logger::LOG_FATAL(myLogger, GMS(myLogger) << __LOCATION__ << " 2 This is a fatal message");
    Logger::LOG_SUPPRESS(myLogger);
    Logger::LOG_DEBUG(myLogger, GMS(myLogger) << __LOCATION__ << " 3 This is a debug message");
    Logger::LOG_WARN(myLogger, GMS(myLogger) << __LOCATION__ << " 4 This is a warning message");
    Logger::LOG_RESUME(myLogger);
    Logger::LOG_ERROR(myLogger, GMS(myLogger) << __LOCATION__ << " 5 This is an error message");
    Logger::LOG_INFO(myLogger, GMS(myLogger) << __LOCATION__ << " 6 This is a fatal message");

    const unsigned short LOOP = 3000;

    boost::chrono::microseconds dur[LOOP];

    for (int i = 0; i < LOOP; i++)
    {
        boost::this_thread::sleep_for(boost::chrono::seconds(2));

        boost::chrono::high_resolution_clock::time_point start = boost::chrono::high_resolution_clock::now();
        Logger::LOG_INFO(myLogger, GMS(myLogger) << __LOCATION__ << " This is an info message " << i);
        boost::chrono::high_resolution_clock::time_point stop = boost::chrono::high_resolution_clock::now();
        dur[i] = boost::chrono::duration_cast<boost::chrono::microseconds>(stop - start);

        //std::cout << dur[i].count() << std::endl;
    }

    Logger::LOG_WARN(myLogger, GMS(myLogger) << __LOCATION__ << " 5 This is an error message");
    Logger::LOG_WARN(myLogger, GMS(myLogger) << __LOCATION__ << " 6 This is a fatal message");

    int avgDur(0);

    for (int i = 0; i < LOOP; i++)
    {
        avgDur = avgDur + dur[i].count();
    }

    avgDur = avgDur / LOOP;

    myLogger.userStopLog();

    std::cout << "\n\navgDur: " << avgDur << std::endl;

    //ofs.close();

    std::cout << std::endl;
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
