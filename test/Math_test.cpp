#include <gtest/gtest.h>
#include <directxmath/DirectXColors.h>
#include "Math.hpp"

using namespace DirectX::SimpleMath;
using namespace DirectX::Colors;
using namespace ModernBoy;

constexpr Line WINDOW_RANGE{uvTopLeft, uvBottomRight};
const Line<Color> COLOR_RANGE{Black.v, White.v};

TEST(MathTest, lerp_test){
    static_assert(lerp({0, 1}, 0.4) == 0.4);
    static_assert(lerp({0, 1}, 1.2) == 1.2);
    static_assert(lerp<float>({2.0f, 10.0f}, 0.2f) == 3.6f);
    static_assert(lerp<float>({0.0f, 10.0f}, 2) == 20.0f);
    
    // floating-point precision problem
    EXPECT_DOUBLE_EQ(lerp({10.0, 33.0}, 0.23), 15.29);
    EXPECT_DOUBLE_EQ(lerp({5423.0, 45565.0}, 0.1234), 10376.5228);
}

TEST(MathTest, lerp2_test){
    constexpr Line top{uvTopLeft, uvTopRight};
    constexpr Line bottom{uvBottomLeft, uvBottomRight};
    constexpr Line uv{top, bottom};

    constexpr Vector2 expected1{0.2f, 0.3f};
    auto case1 = lerp2(uv, expected1.x, expected1.y);

    EXPECT_EQ(expected1, case1);
}

TEST(MathTest, in_test){
    EXPECT_TRUE(in(3.4f, {1.0f, 10.0f}));
    EXPECT_FALSE(in(12.4f, {1.0f, 10.0f}));

    EXPECT_TRUE(in({0.3f, 0.7f}, WINDOW_RANGE));
    EXPECT_FALSE(in({0.3f, 1.1f}, WINDOW_RANGE));
    EXPECT_FALSE(in({-0.1f, 0.7f}, WINDOW_RANGE));
    EXPECT_FALSE(in({-0.1f, -0.1f}, WINDOW_RANGE));
    EXPECT_FALSE(in({1.1f, 1.1f}, WINDOW_RANGE));

    EXPECT_TRUE(in(DUNE, COLOR_RANGE));
    EXPECT_FALSE(in<Color>(White.v, {Black.v, DUNE}));
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
    constexpr Vector2 origin1{0.7f, 1.3f};
    constexpr Vector2 origin2{1.1f, 0.7f};
    constexpr Vector2 origin3{1.2f, 1.2f};
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
    constexpr Vector2 origin1{0.7f, 1.3f};
    constexpr Vector2 origin2{1.1f, 0.7f};
    constexpr Vector2 origin3{1.2f, 1.2f};
    constexpr auto clamped1=clamp(origin1, WINDOW_RANGE);
    constexpr auto clamped2=clamp(origin2, WINDOW_RANGE);
    constexpr auto clamped3=clamp(origin3, WINDOW_RANGE);

    static_assert(!in(clamped1, WINDOW_RANGE));
    static_assert(!in(clamped2, WINDOW_RANGE));
    static_assert(!in(clamped3, WINDOW_RANGE));

    static_assert(origin1.x == clamped1.x);
    static_assert(clamped1.y == 1.0f);
    static_assert(clamped2.x == 1.0f);
    static_assert(origin2.y == clamped2.y);
    EXPECT_EQ(clamped3, uvBottomRight);
}

struct fake_pos{
    float x=0.0f, y=0.0f, z=0.0f;
};
// constexpr bool operator==(const fake_pos& lhs, const fake_pos& rhs) noexcept{
//     return lhs.x==rhs.x && lhs.y==rhs.y && lhs.z==rhs.z;
// }
inline constexpr fake_pos operator+(const Vector3& v, const fake_pos& p) noexcept{
    return{.x=p.x+v.x, .y=p.y+v.y, .z=p.z+v.z};
}
inline constexpr fake_pos operator+(const fake_pos& p, const Vector3& v) noexcept{
    return v+p;
}

TEST(MathTest, affine_test){
    static_assert(affine_additive<Vector3, pos>);
    static_assert(!affine_additive<Vector3, fake_pos>);
}
