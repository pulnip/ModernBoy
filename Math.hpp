#pragma once

#include <cmath>
#include <random>

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

Vector2 abs(const Vector2& v) noexcept;
float size(const Vector2& v) noexcept;
Vector2 normalize(const Vector2& v) noexcept;
// radian -> forward vector
Vector2 getForward(double radian) noexcept;
// forward vector -> radian
double getRotation(const Vector2& v) noexcept;

template<int start, int end>
auto getRandom() noexcept{
    // 시드값을 얻기 위해
    static std::random_device rd;
    // 난수 생성 엔진
    static std::mt19937 gen(rd());
    // [start, end) 균등 분포
    static std::uniform_int_distribution dis(start, end);

    return dis(gen);

}

namespace Math{
    constexpr auto PI=2*std::asin(1);
    constexpr auto epsilon=0.0001;

    double ToDegree(double radian) noexcept;
    double ToRadian(double degree) noexcept;
    template<typename T> T reflect(const T x, const T base) noexcept{
        return 2*base - x;
    }
    bool NearZero(double x);
    int wrap(int x, int low, int high);
    double wrap(const double x, const double low, const double high);
}