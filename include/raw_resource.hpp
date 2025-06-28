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

    public:
        RawMeshPart() = default;
        RawMeshPart(Vertices vertices, Indices indices)
        :vertices(vertices), indices(indices){}
        RawMeshPart(size_t numVertices, size_t numIndices)
        :vertices(numVertices), indices(numIndices){}
    };

    struct RawTexture{
        int width=0, height=0;
        int channels=0;
        std::vector<uint8_t> pixels;
    };
    template<typename Resource>
    Resource import(const std::string&);
} // namespace ModernBoy

#endif // MODERNBOY_RAW_RESOURCE_HPP