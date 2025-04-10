#ifndef __INC_DX11_TYPE_HPP
#define __INC_DX11_TYPE_HPP

#include <wrl/client.h>
#include <d3d11.h>
#include <directxmath/DirectXColors.h>

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

    struct Material{
        DirectX::XMFLOAT3 ambient{0.1f, 0.1f, 0.1f};
        float shininess=1.0f;
        DirectX::XMFLOAT3 diffuse{0.5f, 0.5f, 0.5f};
        float reflection=0.0f;
        DirectX::XMFLOAT3 specular{0.5f, 0.5f, 0.5f};
        float transparency=0.0f;
    }; ALIGN16(Material);
    struct Light{
        DirectX::XMFLOAT3 strength{1.0f, 1.0f, 1.0f};
        float fallOffStart=0.0f;
        DirectX::XMFLOAT3 dir{0.0f, -0.70711f, 0.70711f};
        float fallOffEnd=10.0f;
        DirectX::XMFLOAT3 pos{0.0f, 2.0f, -2.0f};
        float spotPower=1.0f;
    }; ALIGN16(Light);
}

#endif // __INC_DX11_TYPE_HPP