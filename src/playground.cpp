#include <iostream>
#include "prime.h"

int main(void)
{
    std::cout << "Hello C++" << std::endl;

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