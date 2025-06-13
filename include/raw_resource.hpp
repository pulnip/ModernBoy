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
}

#endif // MODERNBOY_RAW_RESOURCE_HPP