#include <cassert>
#include <gsl-lite/gsl-lite.hpp>
#include "Math.hpp"

using namespace gsl;
using namespace glm;
using namespace ModernBoy;

float ModernBoy::lerp(const Line1& line, float t) noexcept{
    return line.point0*(1.0f-t) + line.point1*t;
}

vec3 ModernBoy::lerp(const Line& line, float t) noexcept{
    return line.point0*(1.0f-t) + line.point1*t;
}

vec3 ModernBoy::lerp2(
    const Line& line0, const Line& line1,
    float t0, float t1
) noexcept{
    return lerp({lerp(line0, t0), lerp(line1, t0)}, t1);
}

bool ModernBoy::in(const int x, const iLine1& range) noexcept{
    assert(range.point0 < range.point1);
    return range.point0 <= x && x < range.point1;
}

bool ModernBoy::in(const float x, const Line1& range) noexcept{
    assert(range.point0 < range.point1);
    return range.point0 <= x && x < range.point1;
}

bool ModernBoy::in(const ivec2& x, const iLine2& range) noexcept{
    const auto in_x=in(x.x, {range.point0.x, range.point1.x}); 
    const auto in_y=in(x.y, {range.point0.y, range.point1.y});
    return in_x && in_y; 
}

bool ModernBoy::in(const vec2& x, const Line2& range) noexcept{
    const auto in_x=in(x.x, {range.point0.x, range.point1.x}); 
    const auto in_y=in(x.y, {range.point0.y, range.point1.y});
    return in_x && in_y; 
}

bool ModernBoy::in(const ivec3& x, const iLine& range) noexcept{
    const auto in_x=in(x.x, {range.point0.x, range.point1.x}); 
    const auto in_y=in(x.y, {range.point0.y, range.point1.y});
    const auto in_z=in(x.z, {range.point0.z, range.point1.z});
    return in_x && in_y && in_z; 
}

bool ModernBoy::in(const vec3& x, const Line& range) noexcept{
    const auto in_x=in(x.x, {range.point0.x, range.point1.x}); 
    const auto in_y=in(x.y, {range.point0.y, range.point1.y});
    const auto in_z=in(x.z, {range.point0.z, range.point1.z});
    return in_x && in_y && in_z; 
}

int ModernBoy::wrap(int x, const iLine1& line) noexcept{
    assert(line.point0 <= line.point1);
    const auto xf = x - line.point0;
    const auto len = line.point1 - line.point0;
    const auto r = xf % len;
    return (r >= 0) ? r+line.point0 : r+line.point1;
}

int ModernBoy::clamp(int x, const iLine1& line) noexcept{
    assert(line.point0 <= line.point1);
    return min(max(x, line.point0), line.point1);
}

float ModernBoy::wrap(float x, const Line1& line) noexcept{
    assert(line.point0 <= line.point1);
    const auto xf = x - line.point0;
    const auto len = line.point1 - line.point0;
    const auto r = fmodf(xf, len);
    return (r >= 0) ? r+line.point0 : r+line.point1;
}

float ModernBoy::clamp(float x, const Line1& line) noexcept{
    assert(line.point0 <= line.point1);
    return min(max(x, line.point0), line.point1);
}

ivec2 ModernBoy::wrap(const ivec2& x, const iLine2& line) noexcept{
    return {
        wrap(x.x, {line.point0.x, line.point1.x}),
        wrap(x.y, {line.point0.y, line.point1.y})
    };
}

vec2 ModernBoy::floor(const vec2& vec) noexcept{
    return {std::floor(vec.x), std::floor(vec.y)};
}

iRGB ModernBoy::rgbcvt(const fRGB& color) noexcept{
    assert(in(color, {vec3(0.0f), vec3(1.0f)}));
    return narrow_cast<iRGB>(256.0f * color);
}

fRGB ModernBoy::rgbcvt(const iRGB& color) noexcept{
    return static_cast<fRGB>(color) / 256.0f;
}
