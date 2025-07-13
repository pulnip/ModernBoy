#ifndef MODERNBOY_PHYSICS_HPP
#define MODERNBOY_PHYSICS_HPP

#include "common/type.hpp"

namespace ModernBoy
{
    Vec2 support(Vec2 polygon[], int num_vert, Vec2 v);
    Vec2 mink_dif(Vec2 poly1[], int num_vert1,
        Vec2 poly2[], int num_vert2, Vec2 dir);
    Vec2 normal(Vec2 v, Vec2 dir);
    Vec2 barycenter(Vec2 poly[], int num_vert);
    bool gjk(Vec2 poly1[], int num_vert1,
        Vec2 poly2[], int num_vert2);
}

#endif // MODERNBOY_PHYSICS_HPP