#include <gtest/gtest.h>

#include "Prime.h"

TEST(PrimeTestsuite, ithPrime)
{
    EXPECT_EQ(Prime::getNthPrime(1), 2);
}
