#ifndef MODERNBOY_ASSET_RAWRESOURCE_HPP
#define MODERNBOY_ASSET_RAWRESOURCE_HPP

#include "core/math/type.hpp"

namespace ModernBoy::Asset
{
    struct Vertex{
        Vec3 position;
        Vec3 normal;
        Vec2 texcoord;
        Vec4 tangent;
    };
} // namespace ModernBoy::Asset

#endif // MODERNBOY_ASSET_RAWRESOURCE_HPP