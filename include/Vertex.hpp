#pragma once

#include "Math.hpp"

namespace ModernBoy{
    struct ColorVertex{
        WorldPos position;
        Vector3 normal;
        Color color;
    };

    struct Vertex{
        WorldPos position;
        Vector3 normal;
        UVPos uv;
    };
}
