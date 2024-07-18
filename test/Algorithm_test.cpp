#include <vector>
#include <gtest/gtest.h>
#include "Algorithm/base.hpp"

using namespace std;
using namespace ModernBoy;

TEST(AlgorithmTest, odd_test){
    EXPECT_EQ(odd(1), true);
    EXPECT_EQ(odd(43), true);

}

TEST(AlgorithmTest, half_test){
    EXPECT_EQ(half(42), 21);
    EXPECT_EQ(half(53), 26);
}

TEST(AlgorithmTest, multiply_test){
    EXPECT_EQ(multiply(6, 7), 42);
    EXPECT_EQ(multiply(3, 14), 42);
    EXPECT_EQ(multiply(2, 21), 42);
}

TEST(AlgorithmTest, prime_test){
    EXPECT_EQ(isPrime(2), true);
    EXPECT_EQ(isPrime(3), true);
    EXPECT_EQ(isPrime(4), false);
    EXPECT_EQ(isPrime(5), true);

    EXPECT_EQ(isPrime(49), false);
    EXPECT_EQ(isPrime(47), true);
    EXPECT_EQ(isPrime(45), false);
    EXPECT_EQ(isPrime(43), true);
    EXPECT_EQ(isPrime(41), true);
    EXPECT_EQ(isPrime(39), false);
    EXPECT_EQ(isPrime(37), true);
    EXPECT_EQ(isPrime(35), false);
    EXPECT_EQ(isPrime(33), false);
    EXPECT_EQ(isPrime(31), true);
    EXPECT_EQ(isPrime(29), true);
    EXPECT_EQ(isPrime(27), false);
    EXPECT_EQ(isPrime(25), false);

    EXPECT_EQ(isPrime(51), false);
    EXPECT_EQ(isPrime(53), true);
    EXPECT_EQ(isPrime(55), false);
    EXPECT_EQ(isPrime(57), false);
    EXPECT_EQ(isPrime(59), true);
    EXPECT_EQ(isPrime(61), true);
    EXPECT_EQ(isPrime(63), false);
    EXPECT_EQ(isPrime(65), false);
    EXPECT_EQ(isPrime(67), true);
    EXPECT_EQ(isPrime(69), false);
    EXPECT_EQ(isPrime(71), true);
    EXPECT_EQ(isPrime(73), true);
    EXPECT_EQ(isPrime(75), false);

    EXPECT_EQ(isPrime(101), true);
}

TEST(AlgorithmTest, gcd_test){
    EXPECT_EQ(gcd(24, 8), 8);
    EXPECT_EQ(gcd(12, 18), 6);
    EXPECT_EQ(gcd(15, 25), 5);
    EXPECT_EQ(gcd(120, 36), 12);
    EXPECT_EQ(gcd(625, 150), 25);
    EXPECT_EQ(gcd(1071, 462), 21);
    EXPECT_EQ(gcd(2304, 1440), 288);
    EXPECT_EQ(gcd(123456, 789012), 12);

    EXPECT_EQ(gcd(0, 5), 5);
    EXPECT_EQ(gcd(-24, 18), -6);
}
