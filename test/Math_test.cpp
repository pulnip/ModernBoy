#include <gtest/gtest.h>
#include "Math.hpp"

using namespace glm;
using namespace ModernBoy;

TEST(MathTest, lerpf_test){
    EXPECT_FLOAT_EQ(lerp({0.0f, 10.0f}, 0.5f), 5.0f);
    EXPECT_FLOAT_EQ(lerp({0.0f, 5.0f}, 0.2f), 1.0f);
    EXPECT_FLOAT_EQ(lerp({2.0f, 10.0f}, 0.2f), 3.6f);
    EXPECT_FLOAT_EQ(lerp({10.0f, 33.0f}, 0.23f), 15.29f);
    EXPECT_FLOAT_EQ(lerp({5423.0f, 45565.0f}, 0.1234f), 10376.5228f);
}

TEST(MathTest, lerpv_test){
    EXPECT_EQ(lerp({fBLACK, fWHITE}, 0.2f), fDUNE);

    auto case2 = lerp({{1.0f, 2.0f, 3.0f}, {3.0f, 6.0f, 9.0f}}, 0.25f);
    vec3 expected2{1.5f, 3.0f, 4.5f};
    EXPECT_EQ(case2, expected2);
}

TEST(MathTest, in_test){
    EXPECT_EQ(in(3.4f, {1.0f, 10.0f}), true);
    EXPECT_EQ(in(12.4f, {1.0f, 10.0f}), false);
    EXPECT_EQ(in(fDUNE, {fBLACK, fWHITE}), true);
    EXPECT_EQ(in(fWHITE, {fBLACK, fDUNE}), false);
}

TEST(MathTest, wrap_test){
    EXPECT_EQ(wrap(2, {10, 50}), 42);
    EXPECT_EQ(wrap(15, {32, 64}), 47);
    EXPECT_FLOAT_EQ(wrap(6.3f, {1.0f, 5.0f}), 2.3f);
    EXPECT_FLOAT_EQ(wrap(6.3f, {12.4f, 20.3f}), 14.2f);
}

TEST(MathTest, clamp_test){
    EXPECT_EQ(clamp(2, {10, 50}), 10);
    EXPECT_EQ(clamp(91, {32, 64}), 64);
    EXPECT_EQ(clamp(47, {32, 64}), 47);
    EXPECT_FLOAT_EQ(clamp(6.3f, {1.0f, 5.0f}), 5.0f);
    EXPECT_FLOAT_EQ(clamp(6.3f, {12.4f, 20.3f}), 12.4f);
    EXPECT_FLOAT_EQ(clamp(34.5f, {12.3f, 45.6f}), 34.5f);
}
