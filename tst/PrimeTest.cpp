#include <gtest/gtest.h>
#include "Prime.h"


TEST(PrimeTest, checkPrime)
{
    EXPECT_FALSE(Prime::checkForPrime(1));
    EXPECT_FALSE(Prime::checkForPrime(0));
    EXPECT_TRUE(Prime::checkForPrime(2));
    EXPECT_TRUE(Prime::checkForPrime(3));
    EXPECT_TRUE(Prime::checkForPrime(823));
    EXPECT_TRUE(Prime::checkForPrime(5647));
    EXPECT_TRUE(Prime::checkForPrime(11833));
    EXPECT_TRUE(Prime::checkForPrime(28631));
    EXPECT_TRUE(Prime::checkForPrime(45989));
    EXPECT_TRUE(Prime::checkForPrime(104729));
    EXPECT_TRUE(Prime::checkForPrime(6274687));
    EXPECT_TRUE(Prime::checkForPrime(88891853));
}
