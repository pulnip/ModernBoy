#ifndef MODERNBOY_PHYSICS_HPP
#define MODERNBOY_PHYSICS_HPP

#include "common/type.hpp"

namespace ModernBoy
{
    Vec2 support(const Vec2 polygon[], int num_vert, const Vec2 v);
    Vec2 mink_dif(const Vec2 poly1[], int num_vert1,
        const Vec2 poly2[], int num_vert2, const Vec2 dir);
    Vec2 normal(const Vec2 v, const Vec2 dir);
    Vec2 barycenter(const Vec2 poly[], int num_vert);
    bool gjk(const Vec2 poly1[], int num_vert1,
        const Vec2 poly2[], int num_vert2);

    Vec3 support(const Vec3 polygon[], int num_vert, const Vec3 v);
    Vec3 mink_dif(const Vec3 poly1[], int num_vert1,
        const Vec3 poly2[], int num_vert2, const Vec3 dir);
    Vec3 dir_normal(const Vec3 p1, const Vec3 p2, const Vec3 p3, const Vec3 tgt_pt);
    Vec3 barycenter(const Vec3 poly[], int num_vert);
    bool gjk(const Vec3 poly1[], int num_vert1,
        const Vec3 poly2[], int num_vert2);
}

#endif // MODERNBOY_PHYSICS_HPP