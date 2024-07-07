#pragma once

#include <concepts>
#include <cmath>
#include <glm/glm.hpp>

namespace ModernBoy{
    template <typename T>
    concept arithmetic = std::integral<T> || std::floating_point<T>;

    struct iLine1{ int point0, point1; };
    struct Line1{ float point0, point1; };
    struct iLine2{ glm::ivec2 point0, point1; };
    struct Line2{ glm::vec3 point0, point1; };
    struct iLine{ glm::ivec3 point0, point1; };
    struct Line{ glm::vec3 point0, point1; };

    float lerp(const Line1& line, float t) noexcept;
    glm::vec3 lerp(const Line& line, float t) noexcept;
    glm::vec3 lerp2(const Line& line0, const Line& line1, float t0, float t1) noexcept;

    bool in(const int x, const iLine1& range) noexcept;
    bool in(const float x, const Line1& range) noexcept;
    bool in(const glm::ivec2& x, const iLine2& range) noexcept;
    bool in(const glm::vec2& x, const Line2& range) noexcept;
    bool in(const glm::ivec3& x, const iLine& range) noexcept;
    bool in(const glm::vec3& x, const Line& range) noexcept;

    int wrap(int x, const iLine1& line) noexcept;
    int clamp(int x, const iLine1& line) noexcept;
    float wrap(float x, const Line1& line) noexcept;
    float clamp(float x, const Line1& line) noexcept;
    glm::ivec2 wrap(const glm::ivec2& x, const iLine2& line) noexcept;

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

    using UVPos = glm::vec2;
    using PixelPos = glm::ivec2;
    using WorldPos = glm::vec3;

    constexpr UVPos uvTopLeft{0.0f, 0.0f};
    constexpr UVPos uvTopRight{1.0f, 0.0f};
    constexpr UVPos uvBottomLeft{0.0f, 1.0f};
    constexpr UVPos uvBottomRight{1.0f, 1.0f};
}
