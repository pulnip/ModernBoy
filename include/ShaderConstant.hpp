#pragma once

#include "Attributes.hpp"

#define MAX_LIGHTS 3

namespace ModernBoy{
    using Matrix=Matrix;

    struct VSConstants{
        Matrix model{};
        Matrix invTranspose{};
        Matrix view{};
        Matrix projection{};
    };
    // check 16-byte aligned
    static_assert((sizeof(VSConstants) & 0xf) == 0);

    struct PSConstants{
        // float xSplit=0.0f;
        // float dummy[3]={0.0f, 0.0f, 0.0f};
        Vector3 eyePos;
        bool useTexture;
        Material material;
        Light lights[MAX_LIGHTS];
        // Rim Lighting
        Vector3 rimColor=DirectX::Colors::White.v;
        float rimStrength;
        float rimPower;
        float dummy[3];
    };
    // check 16-byte aligned
    static_assert((sizeof(PSConstants) & 0xf) == 0);

    struct NormalConstants{
        float scale;
        float dummy[3];
    };
    // check 16-byte aligned
    static_assert((sizeof(NormalConstants) & 0xf) == 0);

    struct CubeMappingConstants{
        Matrix view{};
        Matrix projection{};
    };
    // check 16-byte aligned
    static_assert((sizeof(CubeMappingConstants) & 0xf) == 0);
}