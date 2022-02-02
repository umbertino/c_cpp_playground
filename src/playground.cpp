#include <iostream>
#include <stdexcept>
#include "prime.h"
#include "ScopeGuard.h"

int main(void)
{
    std::cout << "Hello C++" << std::endl;

    try
    {
        SUPPRESS_LOGGING(
            std::cout << "Scoped output" << std::endl;
            throw std::invalid_argument("received negative value");)
    }
    catch (...)
    {
        std::cerr << "Exception caught" << std::endl;
    }

    unsigned int number = 18;

    bool isPrime = prime(number);

    if (isPrime)
    {
        std::cout << number << " is a prime number!" << std::endl;
    }
    else
    {
        std::cout << number << " is not a prime number!" << std::endl;
    }

    return 0;
}