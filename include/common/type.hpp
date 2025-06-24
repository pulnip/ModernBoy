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
        float fov = 100.0f;
        float nearPlane = 0.1f;
        float farPlane = 100.0f;
        Projection projection = Projection::PERSPECTIVE;
    };

        struct Handle{
        size_t index = size_t(-1);
        uint32_t generation = 0;

        static constexpr Handle invalid(){ return Handle{}; }

        bool isValid() const{ return index != UINT32_MAX; }
        bool operator==(const Handle&) const = default;
    };

    enum class ResourceType{
        INVALID      = -1,
        MESH         = 0,
        SHADER       = 1,
        NUM_RESOURCE = 2,
    };

    struct ResourceHandle{
        ResourceType type = ResourceType::INVALID;

        size_t index = size_t(-1);
        uint32_t generation = 0;

        static constexpr ResourceHandle invalid(){ return ResourceHandle{}; }

        bool isValid() const{ return index != size_t(-1); }
        bool operator==(const ResourceHandle&) const = default;
    };

} // namespace ModernBoy

#endif // MODERNBOY_COMMON_TYPE_HPP