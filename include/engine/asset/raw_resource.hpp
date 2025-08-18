#ifndef MODERNBOY_ASSET_RAWRESOURCE_HPP
#define MODERNBOY_ASSET_RAWRESOURCE_HPP

#include "core/math/type.hpp"

namespace ModernBoy::Asset
{
    struct RawVertex{
        float position[3];
        float normal[3];
        float texcoord[2];
        float tangent[3];
    };

    using Vertices = std::vector<RawVertex>;
    using Indices = std::vector<uint32_t>;

    struct RawMeshPart{
        Vertices vertices;
        Indices indices;

    public:
        RawMeshPart() = default;
        RawMeshPart(Vertices vertices, Indices indices)
        :vertices(vertices), indices(indices){}
        RawMeshPart(size_t numVertices, size_t numIndices)
        :vertices(numVertices), indices(numIndices){}
    };
    using RawMesh = std::vector<RawMeshPart>;

    struct RawTexture{
        int width=0, height=0;
        int channels=0;
        std::vector<uint8_t> pixels;
    };



    struct Vertex{
        Vec3 position;
        Vec3 normal;
        Vec2 texcoord;
        Vec4 tangent;
    };
} // namespace ModernBoy::Asset

#endif // MODERNBOY_ASSET_RAWRESOURCE_HPP