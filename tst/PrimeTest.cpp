#include <gtest/gtest.h>
#include "Prime.h"

// Demonstrate some basic assertions.
TEST(PrimeTest, checkPrime)
{
    // Expect equality.
    EXPECT_TRUE(Prime::checkForPrime(3));
}