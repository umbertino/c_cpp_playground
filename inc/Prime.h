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
    static bool prime(unsigned int number);
    static bool factorize(unsigned long number, vector<unsigned long>& primeFactors);
};

