#include <algorithm>
#include <cassert>
#include <random>

#include "Math.hpp"

Vector2& Vector2::operator+=(const Vector2& other){
    x += other.x;
    y += other.y;
    return *this;
}
Vector2 operator+(const Vector2& lhs, const Vector2& rhs){
    return Vector2{lhs.x+rhs.x, lhs.y+rhs.y};
}

Vector2 operator-(const Vector2& lhs, const Vector2& rhs){
    return Vector2{lhs.x-rhs.x, lhs.y-rhs.y};
}

Vector2 operator*(float scalar, const Vector2& v){
    return Vector2{scalar*v.x, scalar*v.y};
}
Vector2 operator*(const Vector2& v, float scalar){
    return scalar*v;
}
Vector2 operator/(const Vector2& v, float scalar){
    return (1.0/scalar)*v;
}
bool operator<=(const Vector2& lhs, const Vector2& rhs){
    return lhs.x<=rhs.x && lhs.y<=rhs.y;
}

Vector2 abs(const Vector2& v) noexcept{
    return Vector2{ std::abs(v.x), std::abs(v.y) };
}

float size(const Vector2& v) noexcept{
    return std::sqrt(v.x*v.x + v.y*v.y);
}

Vector2 normalize(const Vector2& v) noexcept{
    return v/size(v);
}

Vector2 getForward(double radian) noexcept{
    return Vector2{
        static_cast<float>(std::cos(radian)),
        static_cast<float>(std::sin(radian))
    };
}

double getRotation(const Vector2& v) noexcept{
    return std::atan2(v.y, v.x);
}

double Math::ToDegree(double radian) noexcept{
    return 180.0/PI * radian;
}

double Math::ToRadian(double degree) noexcept{
    return PI/180.0 * degree;
}

bool Math::NearZero(double x){ return std::abs(x) < epsilon; }

int Math::wrap(int x, int low, int high){
    assert(low <= high);

    const auto r = (x-low) % (high-low);

    return (r >= 0) ? (r + low) : (r + high);
}

double Math::wrap(const double x, const double low, const double high){
    assert(low <= high);

    const auto r = std::fmod(x-low, high-low);

    return (r >= 0) ? (r + low) : (r + high);
}