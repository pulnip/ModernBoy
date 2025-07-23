#ifndef MODERNBOY_COMMON_TYPE_HPP
#define MODERNBOY_COMMON_TYPE_HPP

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

    float dot(Vec2, Vec2);
    float cross(Vec2, Vec2);

    Vec3 zeros();
    Vec3 ones();

    Vec3 operator+(Vec3, Vec3);
    Vec3& operator+=(Vec3&, Vec3);
    Vec3 operator-(Vec3, Vec3);
    Vec3 operator-(Vec3);
    Vec3 operator*(Vec3, float);
    Vec3& operator*=(Vec3&, float);
    Vec3 operator/(Vec3, float);
    Vec3 cross(Vec3, Vec3);

    bool operator==(Vec3, Vec3);

    float dot(Vec3, Vec3);
    float norm_squared(Vec3);

    Vec4 unitQuat();

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

    struct Camera{
        CameraType type;
        float fov;
        float nearPlane;
        float farPlane;
        Projection projection;
    }; static_assert(std::is_trivially_copyable_v<Camera>);

    Camera defaultCamera();

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
} // namespace ModernBoy

#endif // MODERNBOY_COMMON_TYPE_HPP