#pragma once

#include <cmath>

struct Vector2{
    float x=0, y=0;
public:
    Vector2& operator+=(const Vector2& other);
};

Vector2 operator+(const Vector2& lhs, const Vector2& rhs);
Vector2 operator-(const Vector2& lhs, const Vector2& rhs);
Vector2 operator*(float scalar, const Vector2& v);
Vector2 operator*(const Vector2& v, float scalar);
Vector2 operator/(const Vector2& v, float scalar);
bool operator<=(const Vector2& lhs, const Vector2& rhs);

Vector2 abs(const Vector2& v);

namespace Math{
    double ToDegree(double radian);
    template<typename T> T reflect(const T x, const T base) noexcept{
        return 2*base - x;
    }
}