#pragma once

#include <cassert>
#include <cmath>
#include <directxtk/SimpleMath.h>
#include "Concepts.hpp"

namespace ModernBoy{
    template<typename T> struct Line{ T point0, point1; };
    using iLine1=Line<int>;
    using Line1=Line<float>;

    template<linear T=double, std::scalar S=double>
    constexpr T lerp(const Line<T>& line, S t) noexcept{
        const auto dir = line.point1-line.point0;
        return line.point0 + t*dir;
        // return line.point0*(S(1)-t) + line.point1*t;
    }
    template<linear T=double, std::scalar S=double>
    constexpr T lerp2(const Line<Line<T>>& line, S t0, S t1) noexcept{
        return lerp<T>(
            {
                lerp(line.point0, t0),
                lerp(line.point1, t0)
            }, t1
        );
    }

    template<Dim2 T>
    inline constexpr bool operator<(const T& lhs, const T& rhs){
        return lhs.x<rhs.x && lhs.y<rhs.y;
    }
    template<Dim2 T>
    inline constexpr bool operator>(const T& lhs, const T& rhs){
        return lhs.x>rhs.x && lhs.y>rhs.y;
    }
    template<Dim2 T>
    inline constexpr bool operator<=(const T& lhs, const T& rhs){
        return lhs.x<=rhs.x && lhs.y<=rhs.y;
    }
    template<Dim2 T>
    inline constexpr bool operator>=(const T& lhs, const T& rhs){
        return lhs.x>=rhs.x && lhs.y>=rhs.y;
    }

    template<Dim3 T>
    inline constexpr bool operator<(const T& lhs, const T& rhs){
        return lhs.x<rhs.x && lhs.y<rhs.y && lhs.z<rhs.z;
    }
    template<Dim3 T>
    inline constexpr bool operator>(const T& lhs, const T& rhs){
        return lhs.x>rhs.x && lhs.y>rhs.y && lhs.z>rhs.z;
    }
    template<Dim3 T>
    inline constexpr bool operator<=(const T& lhs, const T& rhs){
        return lhs.x<=rhs.x && lhs.y<=rhs.y && lhs.z<=rhs.z;
    }
    template<Dim3 T>
    inline constexpr bool operator>=(const T& lhs, const T& rhs){
        return lhs.x>=rhs.x && lhs.y>=rhs.y && lhs.z>=rhs.z;
    }

    template<std::totally_ordered T>
    constexpr bool in(const T& val, const Line<T>& range) noexcept{
        assert(range.point0 < range.point1);
        return range.point0 <= val && val < range.point1;
    }

    template<std::integral T>
    constexpr T mod(const T& dividend, const T& divisor) noexcept{
        return dividend % divisor;
    }
    template<std::floating_point T>
    T mod(const T& dividend, const T& divisor) noexcept{
        return std::fmod(dividend, divisor);
    }
    template<std::scalar T>
    constexpr T wrap(const T& x, const Line<T>& range) noexcept{
        assert(range.point0 <= range.point1);
        const auto xf = x - range.point0;
        const auto len = range.point1 - range.point0;
        const auto r = mod(xf, len);
        return r>=0 ? r+range.point0 : r+range.point1;
    }
    template<Dim2 T>
    constexpr T wrap(const T& val, const Line<T>& range) noexcept{
        return {
            wrap(val.x, {range.point0.x, range.point1.x}),
            wrap(val.y, {range.point0.y, range.point1.y})
        };
    }
    template<Dim3 T>
    constexpr T wrap(const T& val, const Line<T>& range) noexcept{
        return {
            wrap(val.x, {range.point0.x, range.point1.x}),
            wrap(val.y, {range.point0.y, range.point1.y}),
            wrap(val.z, {range.point0.z, range.point1.z})
        };
    }
    template<Dim4 T>
    constexpr T wrap(const T& val, const Line<T>& range) noexcept{
        return {
            wrap(val.x, {range.point0.x, range.point1.x}),
            wrap(val.y, {range.point0.y, range.point1.y}),
            wrap(val.z, {range.point0.z, range.point1.z}),
            wrap(val.w, {range.point0.w, range.point1.w})
        };
    }

    template<std::scalar T>
    constexpr T clamp(const T& val, const Line<T>& range) noexcept{
        assert(range.point0 <= range.point1);
        return std::min(std::max(val, range.point0), range.point1);
    }
    template<Dim2 T>
    constexpr T clamp(const T& val, const Line<T>& range) noexcept{
        return {
            clamp(val.x, {range.point0.x, range.point1.x}),
            clamp(val.y, {range.point0.y, range.point1.y})
        };
    }
    template<Dim3 T>
    constexpr T clamp(const T& val, const Line<T>& range) noexcept{
        return {
            clamp(val.x, {range.point0.x, range.point1.x}),
            clamp(val.y, {range.point0.y, range.point1.y}),
            clamp(val.z, {range.point0.z, range.point1.z})
        };
    }
    template<Dim4 T>
    constexpr T clamp(const T& val, const Line<T>& range) noexcept{
        return {
            clamp(val.x, {range.point0.x, range.point1.x}),
            clamp(val.y, {range.point0.y, range.point1.y}),
            clamp(val.z, {range.point0.z, range.point1.z}),
            clamp(val.w, {range.point0.w, range.point1.w})
        };
    }

    template<Dim2 T>
    constexpr T floor(const T& val) noexcept{
        return {std::floor(val.x), std::floor(val.y)};
    }

    using DirectX::SimpleMath::Color;
    constexpr Color DUNE{0.2f, 0.2f, 0.2f};

    using Channel = uint8_t;
    // rgb, rgba in range [0, 256)
    struct RGB{ Channel r, g, b; };
    struct RGBA{ Channel r, g, b, a; };
    constexpr RGB BLACK{0, 0, 0};
    constexpr RGB WHITE{255, 255, 255};

    RGBA toRGBA(const RGB& color, Channel alpha=255) noexcept;
    RGBA rgbcvt(const Color& color) noexcept;
    Color rgbcvt(const RGBA& color) noexcept;

    using DirectX::SimpleMath::Vector2;
    using DirectX::SimpleMath::Vector3;
    using DirectX::SimpleMath::Matrix;
    using DirectX::SimpleMath::Quaternion;

    using WorldPos=Vector3;
    using UVPos=Vector2;

    constexpr UVPos uvTopLeft{0, 0};
    constexpr UVPos uvTopRight{1, 0};
    constexpr UVPos uvBottomLeft{0, 1};
    constexpr UVPos uvBottomRight{1, 1};

    template<typename T=float>
    struct point2{
        T x=static_cast<T>(0), y=static_cast<T>(0);

        point2()=default;
        point2(const point2&)=default;
        point2(point2&&)=default;
        point2(const T& x, const T& y):x(x), y(y){}
        point2(const Vector2& v)
        :x(static_cast<T>(v.x)), y(static_cast<T>(v.y)){}
        ~point2()=default;
        point2& operator=(const point2&)=default;
        point2& operator=(point2&&)=default;

        inline operator Vector2() const noexcept{
            return{static_cast<float>(x), static_cast<float>(y)};
        }
        inline constexpr auto& operator+=(
            const Vector2& v
        ) noexcept{ x+=v.x; y+=v.y; return *this; }
    };
    template<typename T>
    constexpr bool operator==(const point2<T>& lhs, const point2<T>& rhs) noexcept{
        return lhs.x==rhs.x && lhs.y==rhs.y;
    }

    template<typename T>
    inline constexpr auto operator+(const point2<T>& p,
        const Vector2& v
    ) noexcept{ return point2{p}+=v; }
    template<typename T>
    inline constexpr auto operator+(const Vector3& v,
        const point2<T>& p
    ) noexcept{ return p+v; }
    template<typename T>
    inline constexpr Vector3 operator-(
        const point2<T>& lhs, const point2<T>& rhs
    ) noexcept{ return {lhs.x-rhs.x, lhs.y-rhs.y}; }

    using ipoint2=point2<int>;
    using PixelPos=ipoint2;

    struct pos{
        float x=0.0f, y=0.0f, z=0.0f;

        inline explicit operator Vector3() const noexcept{
            return {x, y, z};
        }
        inline constexpr pos& operator+=(
            const Vector3& v
        ) noexcept{ x+=v.x; y+=v.y; z+=v.z; return *this; }
    };
    constexpr bool operator==(const pos& lhs, const pos& rhs) noexcept{
        return lhs.x==rhs.x && lhs.y==rhs.y && lhs.z==rhs.z;
    }

    inline constexpr pos operator+(const pos& p, const Vector3& v
    ) noexcept{ return pos{p}+=v;}
    inline constexpr pos operator+(const Vector3& v, const pos& p
    ) noexcept{ return p+v; }
    inline constexpr Vector3 operator-(const pos& lhs, const pos& rhs
    ) noexcept{ return {lhs.x-rhs.x, lhs.y-rhs.y, lhs.z-rhs.z}; }
}
