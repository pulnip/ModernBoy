#pragma once

#include "Math.hpp"

namespace ModernBoy{
    struct Vertex{
        WorldPos position;
        Vector3 normal;
        union{
            Vector3 color;
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
