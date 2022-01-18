// prime.cc
#include <stdio.h>
#include <math.h>

#include "prime.h"

bool prime(unsigned int num)
{
    unsigned int i;
    bool isPrime;

    switch (num)
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

            unsigned int end = (unsigned int)(ceil(sqrt(num)));

            for (i = 2; i <= end; i++)
            {
                if ((num % i) == 0)
                {
                    isPrime = false;
                    break;
                }
            }

            break;
        }
    } // switch (num)

    return (isPrime);
}
