#define GLM_ENABLE_EXPERIMENTAL

#include <vector>
#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include "Algorithm/base.hpp"

using namespace std;
using namespace glm;
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

TEST(MathTest, pow_trivial_test){
    EXPECT_EQ(pow(2, 1), 2);
    EXPECT_EQ(pow(2, 2), 4);
    EXPECT_EQ(pow(2, 3), 8);
    EXPECT_EQ(pow(2, 4), 16);
    EXPECT_EQ(pow(2, 5), 32);
    EXPECT_EQ(pow(3, 5), 243);
}

TEST(MathTest, pow_mat_test){
    constexpr mat2 fib1{{1, 1}, {1, 0}};
    constexpr mat2 fib2=fib1*fib1;
    constexpr mat2 fib3=fib2*fib1;
    constexpr mat2 fib4=fib3*fib1;

    EXPECT_EQ(pow(fib1, 1), fib1);
    EXPECT_EQ(pow(fib1, 2), fib2);
    EXPECT_EQ(pow(fib1, 3), fib3);
    EXPECT_EQ(pow(fib1, 4), fib4);
    EXPECT_EQ(pow(fib2, 2), fib4);
}
