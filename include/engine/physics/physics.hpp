#ifndef MODERNBOY_PHYSICS_HPP
#define MODERNBOY_PHYSICS_HPP

#include <array>
#include <cassert>
#include "core/math/type.hpp"

namespace ModernBoy
{
    bool sphereCollision(Vec3 pos1, float radius1, Vec3 pos2, float radius2);

    constexpr bool overlap(
        float m1, float M1,
        float m2, float M2,
        float epsilon = 0
    ){
        assert(epsilon >= 0);
        if(m1 > m2){
            std::swap(m1, m2);
            std::swap(M1, M2);
        }

        if(M1 < m2 - epsilon)
            return false;
        return true;
    }
    inline bool obb(const Vec3 center1, const Vec3 axis11, const Vec3 axis12, const Vec3 axis13,
        const Vec3 center2, const Vec3 axis21, const Vec3 axis22, const Vec3 axis23
    ){
        auto axes = std::array{
            axis11, axis12, axis13, axis21, axis22, axis23,
            cross(axis11, axis21), cross(axis11, axis22), cross(axis11, axis23),
            cross(axis12, axis21), cross(axis12, axis22), cross(axis12, axis23),
            cross(axis13, axis21), cross(axis13, axis22), cross(axis13, axis23),
        };

        for(auto axis: axes){
            auto proj_c1 = dot(axis, center1);
            auto r1 = (std::abs(dot(axis, axis11)) +
                       std::abs(dot(axis, axis12)) +
                       std::abs(dot(axis, axis13))) / 2;
            auto proj_c2 = dot(axis, center2);
            auto r2 = (std::abs(dot(axis, axis21)) +
                       std::abs(dot(axis, axis22)) +
                       std::abs(dot(axis, axis23))) / 2;

            if(!overlap(
                proj_c1-r1, proj_c1+r1,
                proj_c2-r2, proj_c2+r2,
                1e-6f
            ))
                return false;
        }
        return true;
    }

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

    struct RaycastHit{
        Vec3 point;
        float distance;
        ID collider;
    };

    inline bool raycastSphere(const Ray& ray, Vec3 pos, float radius, RaycastHit& hit){
        auto d = pos - ray.point;

        auto dist2 = norm_squared(d);
        auto dProj = dot(d, ray.dir);

        auto cos2 = dProj*dProj/dist2;
        auto sin2 = 1 - cos2;
        auto ray_dist2 = dist2*sin2;

        auto radius2 = radius*radius;

        if(ray_dist2 > radius2)
            return false;
        hit.distance = std::sqrt(dist2*cos2) - std::sqrt(radius2-ray_dist2);
        hit.point = ray.point + hit.distance*ray.dir;

        return true;
    }
}

#endif // MODERNBOY_PHYSICS_HPP