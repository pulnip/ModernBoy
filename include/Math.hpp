#pragma once

#include <cmath>

namespace Math {
using Real = double;
using Int = int;

constexpr Real epsilon = 0.0001;
bool NearZero(const Real x) noexcept;

using Degree = Real;
using Radian = Real;
#if __INTELLISENSE__
constexpr Real PI = 3.14159265;
#else
constexpr Real PI = std::asin(1) * 2;
#endif

Degree toDegree(const Radian r) noexcept;
Radian toRadian(const Degree d) noexcept;

template <typename T>
T reflect(const T target, const T base) noexcept {
    return 2 * base - target;
}

Int wrap(const Int x, const Int low, const Int high) noexcept;
Real wrap(const Real x, const Real low, const Real high) noexcept;

Real random(const Real start, const Real end) noexcept;
} // namespace Math

struct Vector2 {
    float x = 0, y = 0;

  public:
    Vector2 &operator+=(const Vector2 &other) noexcept;

    static Vector2 abs(const Vector2 &v) noexcept;
    static Math::Real size(const Vector2 &v) noexcept;
    static Vector2 normalize(const Vector2 &v) noexcept;
    // radian -> forward vector
    static Vector2 forward(Math::Radian radian) noexcept;
    // forward vector -> radian
    static Math::Radian rotation(const Vector2 &v) noexcept;
};

Vector2 operator+(const Vector2 &lhs, const Vector2 &rhs) noexcept;
Vector2 operator-(const Vector2 &lhs, const Vector2 &rhs) noexcept;
Vector2 operator*(const Math::Real scalar, const Vector2 &v) noexcept;
Vector2 operator*(const Vector2 &v, const Math::Real scalar) noexcept;
Vector2 operator/(const Vector2 &v, const Math::Real scalar) noexcept;
bool operator<=(const Vector2 &lhs, const Vector2 &rhs) noexcept;
