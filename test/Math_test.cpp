#include <gtest/gtest.h>
#include "Math.hpp"

using namespace glm;
using namespace ModernBoy;

constexpr Line2 WINDOW_RANGE{uvTopLeft, uvBottomRight};
constexpr Line fCOLOR_RANGE{fBLACK, fWHITE};

TEST(MathTest, lerp_test){
    static_assert(lerp({0, 1}, 0.4) == 0.4);
    static_assert(lerp({0, 1}, 1.2) == 1.2);
    static_assert(lerp<float>({2.0f, 10.0f}, 0.2f) == 3.6f);
    static_assert(lerp<float>({0.0f, 10.0f}, 2) == 20.0f);
    
    // floating-point precision problem
    EXPECT_DOUBLE_EQ(lerp({10.0, 33.0}, 0.23), 15.29);
    EXPECT_DOUBLE_EQ(lerp({5423.0, 45565.0}, 0.1234), 10376.5228);


    static_assert(lerp<vec3>({fBLACK, fWHITE}, 0.2f) == fDUNE);
    static_assert(lerp<vec3>(
        {
            {1.0f, 2.0f, 3.0f},
            {3.0f, 6.0f, 9.0f}
        }, 0.25f) == vec3{1.5f, 3.0f, 4.5f}
    );

    static_assert(lerp<mat4>({0.0f, 1.0f}, 0.1f) == mat4(0.1f));
}

TEST(MathTest, in_test){
    EXPECT_TRUE(in(3.4f, {1.0f, 10.0f}));
    EXPECT_FALSE(in(12.4f, {1.0f, 10.0f}));

    EXPECT_TRUE(in({0.3f, 0.7f}, WINDOW_RANGE));
    EXPECT_FALSE(in({0.3f, 1.1f}, WINDOW_RANGE));
    EXPECT_FALSE(in({-0.1f, 0.7f}, WINDOW_RANGE));
    EXPECT_FALSE(in({-0.1f, -0.1f}, WINDOW_RANGE));
    EXPECT_FALSE(in({1.1f, 1.1f}, WINDOW_RANGE));

    EXPECT_TRUE(in(fDUNE, fCOLOR_RANGE));
    EXPECT_FALSE(in(fWHITE, {fBLACK, fDUNE}));
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

TEST(MathTest, wrap2_test){
    constexpr vec2 origin1{0.7f, 1.3f};
    constexpr vec2 origin2{1.1f, 0.7f};
    constexpr vec2 origin3{1.2f, 1.2f};
    auto wrapped1=wrap(origin1, WINDOW_RANGE);
    auto wrapped2=wrap(origin2, WINDOW_RANGE);
    auto wrapped3=wrap(origin3, WINDOW_RANGE);

    EXPECT_TRUE(in(wrapped1, WINDOW_RANGE));
    EXPECT_TRUE(in(wrapped2, WINDOW_RANGE));
    EXPECT_TRUE(in(wrapped3, WINDOW_RANGE));

    EXPECT_FLOAT_EQ(origin1.x, wrapped1.x);
    EXPECT_FLOAT_EQ(wrapped1.y, 0.3f);
    EXPECT_FLOAT_EQ(wrapped2.x, 0.1f);
    EXPECT_FLOAT_EQ(origin2.y, wrapped2.y);
    EXPECT_FLOAT_EQ(wrapped3.x, 0.2f);
    EXPECT_FLOAT_EQ(wrapped3.y, 0.2f);
}

TEST(MathTest, clamp2_test){
    constexpr vec2 origin1{0.7f, 1.3f};
    constexpr vec2 origin2{1.1f, 0.7f};
    constexpr vec2 origin3{1.2f, 1.2f};
    auto clamped1=clamp(origin1, WINDOW_RANGE);
    auto clamped2=clamp(origin2, WINDOW_RANGE);
    auto clamped3=clamp(origin3, WINDOW_RANGE);

    EXPECT_FALSE(in(clamped1, WINDOW_RANGE));
    EXPECT_FALSE(in(clamped2, WINDOW_RANGE));
    EXPECT_FALSE(in(clamped3, WINDOW_RANGE));

    EXPECT_FLOAT_EQ(origin1.x, clamped1.x);
    EXPECT_FLOAT_EQ(clamped1.y, 1.0f);
    EXPECT_FLOAT_EQ(clamped2.x, 1.0f);
    EXPECT_FLOAT_EQ(origin2.y, clamped2.y);
    EXPECT_EQ(clamped3, uvBottomRight);
}
