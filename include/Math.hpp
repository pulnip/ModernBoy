#pragma once

#include <cmath>
#include <gsl-lite/gsl-lite.hpp>
#include <glm/glm.hpp>
#include "Concepts.hpp"

namespace ModernBoy{
    template<typename T> struct Line{ T point0, point1; };
    using iLine1=Line<int>;
    using Line1=Line<float>;
    using iLine2=Line<glm::ivec2>;
    using Line2=Line<glm::vec2>;
    using iLine3=Line<glm::ivec3>;
    using Line3=Line<glm::vec3>;

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

    bool in(const int x, const iLine1& range) noexcept;
    bool in(const float x, const Line1& range) noexcept;
    bool in(const glm::ivec2& x, const iLine2& range) noexcept;
    bool in(const glm::vec2& x, const Line2& range) noexcept;
    bool in(const glm::ivec3& x, const iLine3& range) noexcept;
    bool in(const glm::vec3& x, const Line3& range) noexcept;

    int wrap(int x, const iLine1& line) noexcept;
    int clamp(int x, const iLine1& line) noexcept;
    float wrap(float x, const Line1& line) noexcept;
    float clamp(float x, const Line1& line) noexcept;
    glm::ivec2 wrap(const glm::ivec2& x, const iLine2& line) noexcept;
    glm::ivec2 clamp(const glm::ivec2& x, const iLine2& line) noexcept;
    glm::vec2 wrap(const glm::vec2& x, const Line2& line) noexcept;
    glm::vec2 clamp(const glm::vec2& x, const Line2& line) noexcept;
    glm::ivec3 wrap(const glm::ivec3& x, const iLine3& line) noexcept;
    glm::ivec3 clamp(const glm::ivec3& x, const iLine3& line) noexcept;
    glm::vec3 wrap(const glm::vec3& x, const Line3& line) noexcept;
    glm::vec3 clamp(const glm::vec3& x, const Line3& line) noexcept;

    glm::vec2 floor(const glm::vec2& vec) noexcept;

    using Channel = uint8_t;
    // rgb in range [0, 256)
    using iRGB = glm::ivec3;
    // rgb in range [0, 1)
    using fRGB = glm::vec3;
    using fRGBA = glm::vec4;

    constexpr fRGB fBLACK(0.0f);
    constexpr fRGB fBLUE(0.0f, 0.0f, 1.0f);
    constexpr fRGB fWHITE(1.0f);
    constexpr fRGB fDUNE(0.2f);

    iRGB rgbcvt(const fRGB& color) noexcept;
    fRGB rgbcvt(const iRGB& color) noexcept;
    fRGBA toRGBA(const fRGB& color, float alpha=1.0f) noexcept;

    struct pos{
        using value_type=float;
        value_type x=0.0f, y=0.0f, z=0.0f;

        inline explicit operator glm::vec3() const noexcept{ return{x, y, z}; }
        inline constexpr pos& operator+=(const glm::vec3& v) noexcept{
            x+=v.x; y+=v.y; z+=v.z;
            return *this;
        }
    };
    constexpr bool operator==(const pos& lhs, const pos& rhs) noexcept{
        return lhs.x==rhs.x && lhs.y==rhs.y && lhs.z==rhs.z;
    }

    inline constexpr pos operator+(const pos& p, const glm::vec3& v) noexcept{
        return pos{p}+=v;
    }
    inline constexpr pos operator+(const glm::vec3& v, const pos& p) noexcept{
        return p+v;
    }
    inline constexpr glm::vec3 operator-(const pos& lhs, const pos& rhs) noexcept{
        return {lhs.x-rhs.x, lhs.y-rhs.y, lhs.z-rhs.z};
    }

    using UVPos = glm::vec2;
    using PixelPos = glm::ivec2;
    using WorldPos = glm::vec3;

    constexpr UVPos uvTopLeft{0.0f, 0.0f};
    constexpr UVPos uvTopRight{1.0f, 0.0f};
    constexpr UVPos uvBottomLeft{0.0f, 1.0f};
    constexpr UVPos uvBottomRight{1.0f, 1.0f};
}
