#ifndef __INC_DX11_TYPES_HPP
#define __INC_DX11_TYPES_HPP

#include <wrl/client.h>
#include <d3d11.h>
#include <SimpleMath.h>
#include <DirectXColors.h>

#define ALIGN16(x) static_assert((sizeof(x) & 0xf) == 0,  #x " must be 16-byte aligned")

namespace ModernBoy::DX11
{
    using DevicePtr = Microsoft::WRL::ComPtr<ID3D11Device>;
    using ContextPtr = Microsoft::WRL::ComPtr<ID3D11DeviceContext>;

    using RasterizerPtr = Microsoft::WRL::ComPtr<ID3D11RasterizerState>;
    using BufferPtr = Microsoft::WRL::ComPtr<ID3D11Buffer>;
    using InputLayoutPtr = Microsoft::WRL::ComPtr<ID3D11InputLayout>;
    using VertexShaderPtr = Microsoft::WRL::ComPtr<ID3D11VertexShader>;
    using PixelShaderPtr = Microsoft::WRL::ComPtr<ID3D11PixelShader>;

    using Vector2 = DirectX::SimpleMath::Vector2;
    using Vector3 = DirectX::SimpleMath::Vector3;
    using Vector4 = DirectX::SimpleMath::Vector4;

    using Matrix = DirectX::SimpleMath::Matrix;

    struct Material{
        Vector3 ambient{0.1f, 0.1f, 0.1f};
        float shininess=1.0f;
        Vector3 diffuse{0.5f, 0.5f, 0.5f};
        float reflection=0.0f;
        Vector3 specular{0.5f, 0.5f, 0.5f};
        float transparency=0.0f;
    }; ALIGN16(Material);
    struct Light{
        Vector3 strength=Vector3::One;
        float fallOffStart=0.0f;
        Vector3 dir={0.0f, -0.70711f, 0.70711f};
        float fallOffEnd=10.0f;
        Vector3 pos={0.0f, 2.0f, -2.0f};
        float spotPower=1.0f;
    }; ALIGN16(Light);
}

#endif // __INC_DX11_TYPES_HPP