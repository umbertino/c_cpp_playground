// Boost-Includes
#include <boost/chrono.hpp>
#include <boost/thread/thread.hpp>
#include <boost/container/vector.hpp>
#include <boost/format.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/core/typeinfo.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

// Std-Includes
#include <algorithm>
#include <iostream>
#include <ostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <chrono>
#include <ctime>
#include <cstddef>
#include <iomanip>
#include <memory>
#include <queue>
#include <thread>
#include <future>
#include <array>
#include <condition_variable>
#include <thread>
#include <pthread.h>

// Own Includes
#include "Prime.h"





int main(void)
{
    std::cout << "Hello, this is a C++ playground" << std::endl
              << std::endl;

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
    return 0;
}
