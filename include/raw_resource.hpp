#ifndef MODERNBOY_RAW_RESOURCE_HPP
#define MODERNBOY_RAW_RESOURCE_HPP

#include <cstdint>
#include <vector>

struct Vector3{ float x, y, z; };
struct Vector4{ float x, y, z, w; };
static_assert(sizeof(Vector3)==sizeof(float[3]));
static_assert(sizeof(Vector4)==sizeof(float[4]));

struct Transform{
    union{
        Vector3 position;
        float pos[3] = {0.0f, 0.0f, 0.0f};
    }; 
    union{
        Vector4 rotation;
        float rot[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    }; 
    union{
        Vector3 scale;
        float scl[3] = {1.0f, 1.0f, 1.0f};
    };
};
static_assert(sizeof(Transform)==sizeof(float[10]));

namespace ModernBoy
{
    struct RawVertex{
        float position[3];
        float normal[3];
        float texcoord[2];
        float tangent[3];
    };

    using Vertices = std::vector<RawVertex>;
    using Indices = std::vector<uint32_t>;
    // File Name of Texture
    using Texture = std::string;
    using Textures = std::vector<Texture>;

    struct RawMesh{
        Vertices vertices;
        Indices indices;

        Textures textures;

    public:
        RawMesh() = default;
        RawMesh(Vertices vertices, Indices indices,
            Textures textures)
        :vertices(vertices), indices(indices),
        textures(textures){}
        RawMesh(size_t numVertices, size_t numIndices,
            std::size_t numTextures)
        :vertices(numVertices), indices(numIndices),
        textures(numTextures){}
    };

    using RawMeshes = std::vector<RawMesh>;

    struct RawTexture{
        int width=0, height=0;
        int channels=0;
        std::vector<uint8_t> pixels;
    };
    using RawTextures = std::vector<RawTexture>;

    Vector3 zeros();
    Vector3 ones();
    Vector4 unitQuaternion();

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
} // namespace ModernBoy

#endif // MODERNBOY_RAW_RESOURCE_HPP