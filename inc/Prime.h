#pragma once

#include <math.h>
#include <boost/container/vector.hpp>
#include <boost/container/set.hpp>

using namespace boost::container;

class Prime
{
private:
    Prime();
    ~Prime();
    static unsigned long SIZE_OF_PRIME_TABLE;
    static set<unsigned long> primeTable;

public:
    static bool checkForPrime(unsigned long number);
    static bool getRangeOfPrimes(unsigned long from, unsigned long to, vector<unsigned long>& listOfPrimes);
    static bool primeFactorize(unsigned long number, vector<unsigned long>& primeFactors);
    static unsigned long getNextPrime(unsigned long start);
    static unsigned long getPrevPrime(unsigned long start);
    static unsigned long getNthPrime(unsigned long n);
};
