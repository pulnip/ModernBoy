#include <algorithm>

#include "Vector2.hpp"

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
bool operator<=(const Vector2& lhs, const Vector2& rhs){
    return lhs.x<=rhs.x && lhs.y<=rhs.y;
}

Vector2 abs(const Vector2& v){
    return Vector2{ std::abs(v.x), std::abs(v.y) };
}

