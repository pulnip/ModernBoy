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
    // check 16-byte aligned
    static_assert((sizeof(VSConstants) & 0xf) == 0);

    struct PSConstants{
        float xSplit=0.0f;
        float dummy[3]={0.0f, 0.0f, 0.0f};
    };
    // check 16-byte aligned
    static_assert((sizeof(PSConstants) & 0xf) == 0);
}
