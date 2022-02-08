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
            unsigned long end = (unsigned long)(ceil(sqrt(number)));

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
    if (Prime::primeCheck(number))
    {
        // a prime number cannot be factorized.
        return false;
    }

    // determine prime deviders in range from 2 to square root of number to factorize
    unsigned long start = 2, end = (unsigned long)(ceil(sqrt(number)));
    vector<unsigned long> primeDeviders;

    if (Prime::primeGetRange(start, end, primeDeviders))
    {
        // okay, there are potential prime deviders for the number to factorize
        // first clear the vector we want to add our prime factors
        primeFactors.clear();

        // now reverse iterate the primeDeviders vector and make test divisions
        for (auto i = end; i >= start; i--)
        {
        }
    }
    else
    {
        return false;
    }
}
