#pragma once

#include <math.h>
#include <boost/container/vector.hpp>

using namespace boost::container;

class Prime
{
private:
    Prime();
    ~Prime();

public:
    static bool primeCheck(unsigned long number);
    static bool primeGetRange(unsigned long from, unsigned long to, vector<unsigned long>& listOfPrimes);
    static bool primeFactorize(unsigned long number, vector<unsigned long>& primeFactors);
};
