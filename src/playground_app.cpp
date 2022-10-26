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
#include <memory>
#include <queue>
#include <thread>
#include <future>

// Own Includes
#include "Prime.h"
#include "ScopeGuard.h"
#include "CustomErrorCodes.h"
#include "MemoryPool.h"

// switches to activate / deactivate examples
#define SCRATCH_PAD 0
#define PRIME_EXAMPLE 0
#define SCOPE_GUARD_EXAMPLE 0
#define FUTURE_PROMISE_EXAMPLE 0
#define MEMORY_POOL_EXAMPLE 1

std::ostream* logOutChannel;

std::ostream& print()
{
    logOutChannel = &std::cout;

    return *logOutChannel;
}

int main(void)
{
    std::cout << "Hello, this is a C++ playground" << std::endl
              << std::endl;

#if SCRATCH_PAD

    std::ostringstream myStream;

    std::cout << "stream size is: " << myStream.rdbuf()->in_avail() << std::endl;

    if (myStream.str().empty())
    {
        std::cout << "myStream is empty" << std::endl;
    }

    myStream << "Hello";

    std::cout << "stream size is: " << myStream.rdbuf()->in_avail() << std::endl;

    if (!myStream.str().empty())
    {
        std::cout << "myStream is not empty" << std::endl;
    }

    myStream.flush();

    std::cout << "stream size is: " << myStream.rdbuf()->in_avail() << std::endl;

    if (!myStream.str().empty())
    {
        std::cout << "myStream is not empty" << std::endl;
    }

    myStream.str("");

    std::cout << "stream size is: " << myStream.rdbuf()->in_avail() << std::endl;

    if (myStream.str().empty())
    {
        std::cout << "myStream is empty" << std::endl;
    }

    std::error_code ec = FlightsErrc::InvertedDates;

    std::cout << "Error-Category: " << ec.category().name() << std::endl;
    std::cout << "Error-message : " << ec.message() << std::endl;
    std::cout << "Error-Value   : " << ec.value() << std::endl;

    print() << "Hello" << std::endl;
    print() << "Du da!" << std::endl;
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

#if FUTURE_PROMISE_EXAMPLE
    std::atomic<bool> magic1Ready(false);
    std::atomic<bool> magic2Ready(false);
    std::promise<int> prom;
    std::future<int> fut = prom.get_future();

    auto magic1 = std::async(std::launch::async, [&]()
                             {
                                 std::this_thread::sleep_for(std::chrono::seconds(5));
                                 magic1Ready.store(true);
                                 return 42;
                             });

    auto magic2 = std::async(std::launch::async, [&]()
                             {
                                 std::this_thread::sleep_for(std::chrono::seconds(8));
                                 prom.set_value(10);
                                 magic2Ready.store(true);
                             });

    std::cout << "Waiting";

    while (!magic1Ready.load() || !magic2Ready.load())
    {
        std::cout << ".";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << std::endl;

    try
    {
        std::cout << magic1.get() << std::endl;
        std::cout << fut.get() << std::endl;

        if (fut.valid() && magic1.valid())
        { // cannot consume any more, raises exception
            std::cout << magic1.get() << std::endl;
            std::cout << fut.get() << std::endl;
        }
    }
    catch (std::exception& ex)
    {
        std::cout << "Exception caught: " << ex.what();
    }

#endif

#if MEMORY_POOL_EXAMPLE
    MemPoolLib::calibratable<std::uint16_t> a;
    MemPoolLib::calibratable<std::uint8_t> b;
    MemPoolLib::calibratable<std::uint8_t> c;
    MemPoolLib::calibratable<std::uint32_t> d;
    MemPoolLib::calibratable<std::uint64_t> e;

    a.set(16);
    b.set(8);
    c.set(8);
    d.set(32);
    e.set(64);

    std::cout << "RefPageStartAddress: " << static_cast<void*>(MemPoolLib::getRefPageStartAddress()) << std::endl;

    std::cout << std::endl;

    std::uint8_t x;
    std::uint16_t y;
    std::uint32_t z;

    std::cout << +a.get() << " " << +b.get() << " " << +c.get() << " " << +d.get() << " " << +e.get() << std::endl;

#endif

    return 0;
}
