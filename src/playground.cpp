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
                throw std::invalid_argument( "received negative value" );
        )
    }
    catch(const std::invalid_argument& e)
    {
        std::cerr << e.what() << '\n';
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