#pragma once

#include "Math.hpp"

namespace ModernBoy{
    struct Vertex{
        WorldPos position;
        Vector3 normal;
        Color color;
        UVPos uv;
    };

    struct VSConstants{
        DirectX::SimpleMath::Matrix model{};
        DirectX::SimpleMath::Matrix view{};
        DirectX::SimpleMath::Matrix projection{};
    };
}
