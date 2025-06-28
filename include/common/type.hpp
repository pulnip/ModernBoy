#ifndef MODERNBOY_COMMON_TYPE_HPP
#define MODERNBOY_COMMON_TYPE_HPP

#include <cstddef>
#include <string>
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
    }; static_assert(std::is_pod_v<Camera>);

    Camera defaultCamera();

    struct NaiveHandle{
        size_t index;
        uint32_t generation;
    }; static_assert(std::is_pod_v<NaiveHandle>);

    NaiveHandle invalidHandle();
    bool isValid(NaiveHandle handle);
    bool operator==(const NaiveHandle& lhs, const NaiveHandle& rhs);

    enum class ResourceType{
        INVALID      = -1,
        MESH         = 0,
        SHADER       = 1,
        MODULE       = 2,
        NUM_RESOURCE = 3,
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