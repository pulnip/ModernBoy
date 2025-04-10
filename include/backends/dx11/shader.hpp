#ifndef __INC_DX11_SHADER_HPP
#define __INC_DX11_SHADER_HPP

#include <string>
#include "type.hpp"

#define MAX_LIGHTS 3

namespace ModernBoy::DX11
{
    struct VSConstants{
        DirectX::XMFLOAT4X4 model{};
        DirectX::XMFLOAT4X4 invTranspose{};
        DirectX::XMFLOAT4X4 view{};
        DirectX::XMFLOAT4X4 projection{};
    }; ALIGN16(VSConstants);
    struct PSConstants{
        // float xSplit=0.0f;
        // float dummy[3]={0.0f, 0.0f, 0.0f};
        DirectX::XMFLOAT3 eyePos;
        bool useTexture;
        Material material;
        Light lights[MAX_LIGHTS];
        // Rim Lighting
        DirectX::XMFLOAT3 rimColor{1.0f, 1.0f, 1.0f};
        float rimStrength;
        float rimPower;
        float dummy[3];
    }; ALIGN16(PSConstants);

    struct NormalConstants{
        float scale;
        float dummy[3];
    }; ALIGN16(NormalConstants);

    struct Shader{
        RasterizerPtr rs;
        RasterizerPtr wireState;

        // Default Shaders
        VSConstants vsc;
        BufferPtr vscBuffer;
        PSConstants psc;
        BufferPtr pscBuffer;

        InputLayoutPtr il;
        VertexShaderPtr vs;
        PixelShaderPtr ps;

        // Normal Vector Shaders
        NormalConstants nvc;
        BufferPtr nvcBuffer;

        InputLayoutPtr nil;
        VertexShaderPtr nvs;
        PixelShaderPtr nps;

    public:
        Shader(DevicePtr& in_device,
            const std::wstring& in_vsFileName = L"shaders/VS.hlsl",
            const std::wstring& in_psFileName = L"shaders/PS.hlsl");
        ~Shader();

        void bind(RenderContext& in_context) const;
    };
}

#endif // __INC_DX11_SHADER_HPP