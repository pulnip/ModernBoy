#include <algorithm>
#include <cassert>
#include <random>

#include "Math.hpp"

// Math namespace
namespace Math{
    bool NearZero(double x) noexcept {
        return std::abs(x) < epsilon;
    }

    Real toDegree(Real radian) noexcept {
        return 180.0 / PI * radian;
    }
    Real toRadian(Real degree) noexcept {
        return PI / 180.0 * degree;
    }

    Int wrap(const Int x, const Int low, const Int high) noexcept {
        assert(low <= high);

        const auto r = (x - low) % (high - low);

        return (r >= 0) ? (r + low) : (r + high);
    }

    Real wrap(const Real x, const Real low, const Real high) noexcept {
        assert(low <= high);
        const auto r = std::fmod(x - low, high - low);
        return (r >= 0) ? (r + low) : (r + high);
    }

    Real random(const Real start, const Real end) noexcept {
        // 난수 생성 엔진
        static std::mt19937 gen(
            // 시드값을 얻기 위해
            (std::random_device())());
        // [start, end) 균등 분포
        std::uniform_real_distribution dis(start, end);

        return dis(gen);
    }
}
// Vector2 class

Vector2& Vector2::operator+=(const Vector2 &other) noexcept {
    x += other.x;
    y += other.y;
    return *this;
}
Vector2& Vector2::operator-=(const Vector2 &other) noexcept {
    x -= other.x;
    y -= other.y;
    return *this;
}

Vector2 Vector2::abs(const Vector2 &v) noexcept {
    return Vector2{std::abs(v.x), std::abs(v.y)};
}
Math::Real Vector2::size(const Vector2 &v) noexcept {
    return std::sqrt(v.x * v.x + v.y * v.y);
}
Vector2 Vector2::normalize(const Vector2 &v) noexcept {
    return v / size(v);
}
Vector2 Vector2::forward(Math::Real radian) noexcept {
    return Vector2{
        static_cast<float>(std::cos(radian)),
        static_cast<float>(std::sin(radian))};
}
Math::Real Vector2::rotation(const Vector2 &v) noexcept {
    return std::atan2(v.y, v.x);
}

Vector2 operator+(const Vector2 &lhs, const Vector2 &rhs) noexcept {
    return Vector2{lhs.x + rhs.x, lhs.y + rhs.y};
}
Vector2 operator-(const Vector2 &lhs, const Vector2 &rhs) noexcept {
    return Vector2{lhs.x - rhs.x, lhs.y - rhs.y};
}
Vector2 operator*(Math::Real scalar, const Vector2 &v) noexcept {
    return Vector2{
        static_cast<float>(scalar * v.x),
        static_cast<float>(scalar * v.y)};
}
Vector2 operator*(const Vector2 &v, Math::Real scalar) noexcept {
    return scalar * v;
}
Math::Real operator*(const Vector2& lhs, const Vector2& rhs) noexcept{
    return lhs.x*rhs.x + lhs.y*rhs.y;
}
Vector2 operator/(const Vector2 &v, Math::Real scalar) noexcept {
    assert(!Math::NearZero(scalar) && "Near Zero!");
    return (1.0 / scalar) * v;
}
bool operator<=(const Vector2 &lhs, const Vector2 &rhs) noexcept {
    return lhs.x <= rhs.x && lhs.y <= rhs.y;
}
