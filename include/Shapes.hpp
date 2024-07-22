#pragma once

#include "Math.hpp"
#include "Object.hpp"

namespace ModernBoy{
    class Sphere: public Object{
      public:
        WorldPos center;
        float radius;

        Sphere(
            const WorldPos& center,
            const float radius,
            const fRGB& color = fBLUE)
        : Object(fDUNE, color, fWHITE)
        , center(center), radius(radius){}

        Hit shootRay(const Ray& ray) const override;
    };

    class Triangle: public Object{
      public:
        WorldPos v0, v1, v2;
        UVPos uv0, uv1, uv2;

        Triangle(
            const WorldPos& v0,
            const WorldPos& v1,
            const WorldPos& v2,
            const UVPos& uv0 = uvTopLeft,
            const UVPos& uv1 = uvTopRight,
            const UVPos& uv2 = uvBottomLeft)
        : Object(fWHITE, fBLACK, fBLACK)
        , v0(v0), v1(v1), v2(v2)
        , uv0(uv0), uv1(uv1), uv2(uv2){}

        Hit shootRay(const Ray& ray) const override;
    };

    class Square: public Object{
      public:
        WorldPos v0, v1, v2, v3;
        UVPos uv0, uv1, uv2, uv3;

        Square(
            const WorldPos& v0,
            const WorldPos& v1,
            const WorldPos& v2,
            const WorldPos& v3,
            const UVPos& uv0 = uvTopLeft,
            const UVPos& uv1 = uvTopRight,
            const UVPos& uv2 = uvBottomRight,
            const UVPos& uv3 = uvBottomLeft)
        : Object(fWHITE, fBLACK, fBLACK)
        , v0(v0), v1(v1), v2(v2), v3(v3)
        , uv0(uv0), uv1(uv1), uv2(uv2), uv3(uv3){}

        Hit shootRay(const Ray& ray) const override;

        Triangle getTopRight() const;
        Triangle getBottomLeft() const;
    };
}
