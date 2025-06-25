#ifndef MODERNBOY_COMMON_TYPE_HPP
#define MODERNBOY_COMMON_TYPE_HPP

#include <cstddef>
#include <type_traits>

namespace ModernBoy
{
    union Vec3{
        float v[3];
        struct{ float x, y, z; };
        struct{ float r, g, b; };
    }; static_assert(std::is_pod_v<Vec3>);
    union Vec4{
        float v[4];
        struct{ float x, y, z, w; };
        struct{ float r, g, b, a; };
    }; static_assert(std::is_pod_v<Vec4>);

    Vec3 zeros();
    Vec3 ones();
    Vec4 unitQuat();

    struct Transform{
        Vec3 position;
        Vec4 rotation;
        Vec3 scale;
    }; static_assert(std::is_pod_v<Transform>);

    Transform identity();

    enum class Projection{
        PERSPECTIVE,
        ORTHOGRAPHIC
    };
    struct Camera{
        float fov;
        float nearPlane;
        float farPlane;
        Projection projection;
    }; static_assert(std::is_pod_v<Camera>);

    Camera defaultCamera();

    struct Handle{
        size_t index;
        uint32_t generation;
    }; static_assert(std::is_pod_v<Handle>);

    Handle invalidHandle();
    bool isValid(Handle handle);
    bool operator==(const Handle& lhs, const Handle& rhs);

    enum class ResourceType{
        INVALID      = -1,
        MESH         = 0,
        SHADER       = 1,
        NUM_RESOURCE = 2,
    };

    struct ResourceHandle{
        ResourceType type;

        size_t index;
        uint32_t generation;
    }; static_assert(std::is_pod_v<ResourceHandle>);

    ResourceHandle invalidResourceHandle();
    bool isValid(ResourceHandle handle);
    bool operator==(const ResourceHandle& lhs, const ResourceHandle& rhs);
} // namespace ModernBoy

#endif // MODERNBOY_COMMON_TYPE_HPP