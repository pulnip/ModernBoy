#pragma once

#include "Math.hpp"

namespace ModernBoy{
    struct Vertex{
        WorldPos position;
        Vector3 normal;
        union{
            Color color;
            UVPos uv;
        };
    };

    struct ColorVertex{
        WorldPos position;
        Vector3 color;
    };

    struct Constants{
        DirectX::SimpleMath::Matrix model{};
        DirectX::SimpleMath::Matrix view{};
        DirectX::SimpleMath::Matrix projection{};
    };
}
