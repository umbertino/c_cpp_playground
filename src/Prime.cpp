// prime.cc
#include <stdio.h>
#include <math.h>

#include "Prime.h"

Prime::Prime()
{
}

Prime::~Prime()
{
}

bool Prime::prime(unsigned int number)
{
    unsigned int i;
    bool isPrime;

    switch (number)
    {
        case 0:
        {
            isPrime = false;
            break;
        }

        case 1:
        {
            isPrime = false;
            break;
        }

        case 2:
        {
            isPrime = true;
            break;
        }

        default:
        {
            isPrime = true;

            unsigned int end = (unsigned int)(ceil(sqrt(number)));

            for (i = 2; i <= end; i++)
            {
                if ((number % i) == 0)
                {
                    isPrime = false;
                    break;
                }
            }

            break;
        }
    } // switch (number)

    return (isPrime);
}

bool Prime::factorize(unsigned long number, vector<unsigned long>& primeFactors)
{
    return false;
}
