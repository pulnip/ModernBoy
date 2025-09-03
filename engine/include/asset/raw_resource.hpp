#ifndef MODERNBOY_ASSET_RAWRESOURCE_HPP
#define MODERNBOY_ASSET_RAWRESOURCE_HPP

#include "math/type.hpp"

namespace ModernBoy::Asset
{
    struct Vertex{
        Vec3 position;
        Vec3 normal;
        Vec2 texcoord;
        Vec4 tangent;
    };
    static_assert(sizeof(Vertex)==sizeof(float)*12);

    using Indices = std::vector<uint32_t>;
} // namespace ModernBoy::Asset

#endif // MODERNBOY_ASSET_RAWRESOURCE_HPP