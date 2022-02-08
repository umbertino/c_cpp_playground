#include <iostream>
#include <stdexcept>
#include "Prime.h"
#include "ScopeGuard.h"
#include <boost/scoped_ptr.hpp>
#include <boost/container/vector.hpp>

// switches to activate / deactivate examples
#define PRIME_EXAMPLE 1
#define SCOPE_GUARD_EXAMPLE 0

int main(void)
{
    std::cout << "Hello C++ playground" << std::endl;

    std::cout << std::endl;

#if PRIME_EXAMPLE
    unsigned int number = 18;

    bool isPrime = Prime::primeCheck(number);

    if (isPrime)
    {
        std::cout << number << " is a prime number!" << std::endl;
    }
    else
    {
        std::cout << number << " is not a prime number!" << std::endl;
    }

    unsigned long start = 0, end = 100;
    boost::container::vector<unsigned long> primeList;

    if (Prime::primeGetRange(start, end, primeList))
    {
        std::cout << "Found " << primeList.size() << " prime numbers in the range [" << start << "," << end << "]: ";

        for (auto i : primeList)
        {
            std::cout << i << " ";
        }
    }
    else
    {
        std::cout << "No prime numbers found in the range [" << start << "," << end << "]: ";
    }

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

    return 0;
}