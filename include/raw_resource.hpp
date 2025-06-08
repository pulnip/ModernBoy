#ifndef MODERNBOY_RAW_RESOURCE_HPP
#define MODERNBOY_RAW_RESOURCE_HPP

#include <cstdint>
#include <vector>

namespace ModernBoy
{
    struct RawVertex{
        float position[3];
        float normal[3];
        float texcoord[2];
        float tangent[3];
    };

    struct RawMesh{
        std::vector<RawVertex> vertices;
        std::vector<uint32_t> indices;

    public:
        RawMesh() = default;
        RawMesh(size_t numVertices, size_t numIndices)
        :vertices(numVertices), indices(numIndices){}
    };

    using RawMeshes = std::vector<RawMesh>;

    struct RawTexture{
        int width=0, height=0;
        int channels=0;
        std::vector<uint8_t> pixels;
    };
}

#endif // MODERNBOY_RAW_RESOURCE_HPP