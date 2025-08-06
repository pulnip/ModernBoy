#ifndef MODERNBOY_COMMON_TYPE_HPP
#define MODERNBOY_COMMON_TYPE_HPP

#include <cmath>
#include <cstddef>
#include <string>
#include <type_traits>
#include "common/alias.hpp"

namespace ModernBoy
{
    union Vec2{
        float v[2];
        struct{ float x, y; };
    }; static_assert(std::is_trivially_copyable_v<Vec2>);
    union Vec3{
        float v[3];
        struct{ float x, y, z; };
        struct{ float r, g, b; };
    }; static_assert(std::is_trivially_copyable_v<Vec3>);
    union Vec4{
        float v[4];
        struct{ float x, y, z, w; };
        struct{ float r, g, b, a; };
    }; static_assert(std::is_trivially_copyable_v<Vec4>);

    Vec2 operator+(Vec2, Vec2);
    Vec2 operator-(Vec2, Vec2);
    Vec2 operator-(Vec2);
    Vec2 operator/(Vec2, float);

    bool operator==(Vec2, Vec2);

    constexpr float dot(Vec2 lhs, Vec2 rhs){
        return lhs.x*rhs.x + lhs.y*rhs.y;
    }
    constexpr float norm_squared(Vec2 v){
        return dot(v, v);
    }
    inline float norm(Vec2 v){
        return std::sqrt(norm_squared(v));
    }
    inline Vec2 normalize(Vec2 v){
        return v / norm(v);
    }
    constexpr float cross(Vec2 lhs, Vec2 rhs){
        return lhs.x*rhs.y - lhs.y*rhs.x;
    }

    Vec3 zeros();
    Vec3 ones();

    constexpr Vec3 operator+(Vec3 lhs, Vec3 rhs){
        return {{lhs.x+rhs.x, lhs.y+rhs.y, lhs.z+rhs.z}};
    }
    constexpr Vec3& operator+=(Vec3& lhs, Vec3 rhs){
        lhs.x += rhs.x;
        lhs.y += rhs.y;
        lhs.z += rhs.z;
        return lhs;
    }
    constexpr Vec3 operator-(Vec3 v){
        return {{-v.x, -v.y, -v.z}};
    }
    constexpr Vec3 operator-(Vec3 lhs, Vec3 rhs){
        return {{lhs.x-rhs.x, lhs.y-rhs.y, lhs.z-rhs.z}};
    }
    constexpr Vec3& operator-=(Vec3& lhs, Vec3 rhs){
        lhs.x -= rhs.x;
        lhs.y -= rhs.y;
        lhs.z -= rhs.z;
        return lhs;
    }
    constexpr Vec3 operator*(Vec3 lhs, Vec3 rhs){
        return{
            .x = lhs.x * rhs.x,
            .y = lhs.y * rhs.y,
            .z = lhs.z * rhs.z
        };
    }
    constexpr Vec3 operator*(float f, Vec3 v){
        return {
            .x = f*v.x,
            .y = f*v.y,
            .z = f*v.z
        };
    }
    constexpr Vec3 operator*(Vec3 v, float f){
        return f*v;
    }
    constexpr Vec3& operator*=(Vec3& v, float f){
        v.x *= f;
        v.y *= f;
        v.z *= f;
        return v;
    }
    Vec3 operator/(Vec3, float);

    bool operator==(Vec3, Vec3);

    constexpr float dot(Vec3 lhs, Vec3 rhs){
        return lhs.x*rhs.x + lhs.y*rhs.y + lhs.z*rhs.z;
    }
    constexpr float norm_squared(Vec3 v){
        return dot(v, v);
    }
    inline float norm(Vec3 v){
        return std::sqrt(norm_squared(v));
    }
    inline Vec3 normalize(Vec3 v){
        return v / norm(v);
    }
    constexpr Vec3 cross(Vec3 lhs, Vec3 rhs){
        return {
            .x = lhs.y*rhs.z - lhs.z*rhs.y,
            .y = lhs.z*rhs.x - lhs.x*rhs.z,
            .z = lhs.x*rhs.y - lhs.y*rhs.x
        };
    }

    constexpr Vec4 unitQuat(){
        return {.x=0, .y=0, .z=0, .w=1};
    }
    constexpr Vec4 conjugate(Vec4 quat){
        return {
            .x = -quat.x,
            .y = -quat.y,
            .z = -quat.z,
            .w =  quat.w
        };
    }

    constexpr Vec4 operator*(Vec4 lhs, Vec4 rhs){
        return {
            .x = lhs.w*rhs.x + lhs.x*rhs.w + lhs.y*rhs.z - lhs.z*rhs.y,
            .y = lhs.w*rhs.y - lhs.x*rhs.z + lhs.y*rhs.w + lhs.z*rhs.x,
            .z = lhs.w*rhs.z + lhs.x*rhs.y - lhs.y*rhs.x + lhs.z*rhs.w,
            .w = lhs.w*rhs.w - lhs.x*rhs.x - lhs.y*rhs.y - lhs.z*rhs.z
        };
    }

    inline Vec4 rotateX(float theta) {
        float half = theta * 0.5f;
        return Vec4{
            .x = std::sinf(half),
            .y = 0.0f,
            .z = 0.0f,
            .w = std::cosf(half)
        };
    }
    inline Vec4 rotateY(float theta) {
        float half = theta * 0.5f;
        return Vec4{
            .x = 0.0f,
            .y = std::sinf(half),
            .z = 0.0f,
            .w = std::cosf(half)
        };
    }
    inline Vec4 rotateZ(float theta) {
        float half = theta * 0.5f;
        return Vec4{
            .x = 0.0f,
            .y = 0.0f,
            .z = std::sinf(half),
            .w = std::cosf(half)
        };
    }
    inline Vec4 yaw(Vec4 quat){
        float siny_cosp = 2*(quat.w*quat.y + quat.x*quat.z);
        float cosy_cosp = 1 - 2*(quat.y*quat.y + quat.x*quat.x);
        float theta = std::atan2(siny_cosp, cosy_cosp);
        return rotateY(theta);
    }
    constexpr Vec4 axisAngle(Vec3 axis, float radian){
        auto half = radian / 2;
        float s = std::sinf(half);
        return {
            .x = axis.x * s,
            .y = axis.y * s,
            .z = axis.z * s,
            .w = std::cosf(half)
        };
    }

    constexpr Vec3 right(Vec4 quat){
        auto e_x = Vec4{.x=1, .y=0, .z=0, .w=0};
        auto vec = quat * e_x * conjugate(quat);
        return {.x=vec.x, .y=vec.y, .z=vec.z };
    }
    constexpr Vec3 ground_right(Vec4 quat){
        auto e_x = Vec4{.x=1, .y=0, .z=0, .w=0};
        Vec4 y_quat = yaw(quat);
        auto vec = y_quat * e_x * conjugate(y_quat);
        return {.x=vec.x, .y=vec.y, .z=vec.z };
    }
    constexpr Vec3 up(Vec4 quat){
        auto e_y = Vec4{.x=0, .y=1, .z=0, .w=0};
        auto vec = quat * e_y * conjugate(quat);
        return {.x=vec.x, .y=vec.y, .z=vec.z };
    }
    constexpr Vec3 forward(Vec4 quat){
        auto e_z = Vec4{.x=0, .y=0, .z=1, .w=0};
        auto vec = quat * e_z * conjugate(quat);
        return {.x=vec.x, .y=vec.y, .z=vec.z };
    }
    constexpr Vec3 ground_forward(Vec4 quat){
        auto e_z = Vec4{.x=0, .y=0, .z=1, .w=0};
        Vec4 y_quat = yaw(quat);
        auto vec = y_quat * e_z * conjugate(y_quat);
        return {.x=vec.x, .y=vec.y, .z=vec.z };
    }

    bool operator==(Vec4, Vec4);

    #define DEFINE_TRANSFORM \
        Vec3 position = zeros(); \
        Vec4 rotation = unitQuat(); \
        Vec3 scale = ones()

    enum class CameraType{
        UNKNOWN     = -1,
        MainCamera  =  0,
        SubCamera   =  1,
    };
    CameraType cameraType(const std::string& text);

    enum class Projection{
        UNKNOWN     = -1,
        PERSPECTIVE =  0,
        ORTHOGRAPHIC=  1,
    };
    Projection projection(const std::string& text);

    enum class ResourceType{
        INVALID      = -1,
        MESH         = 0,
        TEXTURE      = 1,
        SHADER       = 2,
        MODULE       = 3,
        OBJECT       = 4,
        NUM_RESOURCE = 5,
    };

    struct ResourceHandle{
        ResourceType type;

        Index index;
    }; static_assert(std::is_trivially_copyable_v<ResourceHandle>);

    ResourceHandle invalidResourceHandle();
    bool isValid(ResourceHandle handle);
    bool operator==(const ResourceHandle& lhs, const ResourceHandle& rhs);
    bool operator!=(const ResourceHandle& lhs, const ResourceHandle& rhs);
    bool operator<(const ResourceHandle& lhs, const ResourceHandle& rhs);

    struct HandleV2{
        Index index;
        uint32_t generation;
    };
    inline bool operator==(HandleV2 lhs, HandleV2 rhs){
        return lhs.index==rhs.index && lhs.generation==rhs.generation;
    }
    inline bool operator!=(HandleV2 lhs, HandleV2 rhs){
        return !(lhs==rhs);
    }

    struct Ray{
        Vec3 point;
        Vec3 dir;
    };
} // namespace ModernBoy

#endif // MODERNBOY_COMMON_TYPE_HPP