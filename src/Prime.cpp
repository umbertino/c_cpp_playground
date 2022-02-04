// prime.cc
#include <iostream>
#include <math.h>
#include "Prime.h"

using namespace boost::container;

Prime::Prime()
{
}

Prime::~Prime()
{
}

bool Prime::primeCheck(unsigned long number)
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

            // we do not need to check up to number, just until the square root of number
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

bool Prime::primeGetRange(unsigned long from, unsigned long to, vector<unsigned long>& listOfPrimes)
{
    listOfPrimes.clear();

    for (auto i = from; i <= to; i++)
    {
        if (primeCheck(i))
        {
            listOfPrimes.push_back(i);
        }
    }

    if (!listOfPrimes.empty())
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Prime::primeFactorize(unsigned long number, vector<unsigned long>& primeFactors)
{
    primeFactors.clear();

    return false;
}
