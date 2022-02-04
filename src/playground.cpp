#include <iostream>
#include <stdexcept>
#include <vector>
#include "Prime.h"
#include "ScopeGuard.h"
#include <boost/scoped_ptr.hpp>

// switches to activate / deactivate examples
#define BASIC_EXAMPLE 0
#define SCOPE_GUARD_EXAMPLE 1

int main(void)
{
    std::cout << "Hello C++ playground" << std::endl;

    std::cout << std::endl;

#if BASIC_EXAMPLE
    unsigned int number = 18;

    bool isPrime = Prime::prime(number);

    if (isPrime)
    {
        std::cout << number << " is a prime number!" << std::endl;
    }
    else
    {
        std::cout << number << " is not a prime number!" << std::endl;
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
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }

    std::cout << std::endl;

    try
    {
        // usage with scoped pointer and curly block braces
        { // this is the start of the scope
            const boost::scoped_ptr<ScopeGuard> scopeGuard(new ScopeGuard(true, false, true));

            std::vector<int> myvector(10);

            myvector.at(20) = 100; // vector::at throws an out-of-range

        } // this is the end of the scope
    }
    catch (const std::out_of_range& e)
    {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }

    std::cout << std::endl;
#endif

    return 0;
}