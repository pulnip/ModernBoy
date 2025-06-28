#ifndef MODERNBOY_RAW_RESOURCE_HPP
#define MODERNBOY_RAW_RESOURCE_HPP

#include <cstdint>
#include <vector>
#include "fwd.hpp"

namespace ModernBoy
{
    struct RawVertex{
        float position[3];
        float normal[3];
        float texcoord[2];
        float tangent[3];
    };

    struct RawMeshPart{
        Vertices vertices;
        Indices indices;

        TexPaths textures;

    public:
        RawMeshPart() = default;
        RawMeshPart(Vertices vertices, Indices indices,
            TexPaths textures)
        :vertices(vertices), indices(indices),
        textures(textures){}
        RawMeshPart(size_t numVertices, size_t numIndices,
            std::size_t numTextures)
        :vertices(numVertices), indices(numIndices),
        textures(numTextures){}
    };

    struct RawTexture{
        int width=0, height=0;
        int channels=0;
        std::vector<uint8_t> pixels;
    };
} // namespace ModernBoy

#endif // MODERNBOY_RAW_RESOURCE_HPP