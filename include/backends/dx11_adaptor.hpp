#ifndef __INC_DX11_ADAPTOR
#define __INC_DX11_ADAPTOR

#include <wrl/client.h>
#include <d3d11.h>
#include <directxtk/SimpleMath.h>
#include <directxmath/DirectXColors.h>

struct VSConstants{
    DirectX::SimpleMath::Matrix model{};
    DirectX::SimpleMath::Matrix invTranspose{};
    DirectX::SimpleMath::Matrix view{};
    DirectX::SimpleMath::Matrix projection{};
};
// check 16-byte aligned
static_assert((sizeof(VSConstants) & 0xf) == 0);

struct Material{
    DirectX::SimpleMath::Vector3 ambient{0.1f, 0.1f, 0.1f};
    float shininess=1.0f;
    DirectX::SimpleMath::Vector3 diffuse{0.5f, 0.5f, 0.5f};
    float reflection=0.0f;
    DirectX::SimpleMath::Vector3 specular{0.5f, 0.5f, 0.5f};
    float transparency=0.0f;
};
// check 16-byte aligned
static_assert((sizeof(Material) & 0xf) == 0);
struct Light{
    DirectX::SimpleMath::Vector3 strength=DirectX::SimpleMath::Vector3::One;
    float fallOffStart=0.0f;
    DirectX::SimpleMath::Vector3 dir={0.0f, -0.70711f, 0.70711f};
    float fallOffEnd=10.0f;
    DirectX::SimpleMath::Vector3 pos={0.0f, 2.0f, -2.0f};
    float spotPower=1.0f;
};
// check 16-byte aligned
static_assert((sizeof(Light) & 0xf) == 0);

#define MAX_LIGHTS 3

struct PSConstants{
    // float xSplit=0.0f;
    // float dummy[3]={0.0f, 0.0f, 0.0f};
    DirectX::SimpleMath::Vector3 eyePos;
    bool useTexture;
    Material material;
    Light lights[MAX_LIGHTS];
    // Rim Lighting
    DirectX::SimpleMath::Vector3 rimColor=DirectX::Colors::White.v;
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

using RasterizerPtr = Microsoft::WRL::ComPtr<ID3D11RasterizerState>;
using BufferPtr = Microsoft::WRL::ComPtr<ID3D11Buffer>;
using InputLayoutPtr = Microsoft::WRL::ComPtr<ID3D11InputLayout>;
using VertexShaderPtr = Microsoft::WRL::ComPtr<ID3D11VertexShader>;
using PixelShaderPtr = Microsoft::WRL::ComPtr<ID3D11PixelShader>;

struct DX11ShaderState{
    RasterizerPtr rs;
    RasterizerPtr wireState;

    BufferPtr vertexBuffer;
    BufferPtr indexBuffer;

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

};

using DevicePtr = Microsoft::WRL::ComPtr<ID3D11Device>;
using ContextPtr = Microsoft::WRL::ComPtr<ID3D11DeviceContext>;

struct DX11RenderState{
    DevicePtr device;
    ContextPtr context;
    UINT qualityLevels;

    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv;
    D3D11_VIEWPORT viewport{};

    Microsoft::WRL::ComPtr<ID3D11Texture2D> dsb;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> dsv;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> dss;

    DX11ShaderState shaderState;
};

extern bool DX11RenderState_Init(HWND, DX11RenderState*);
extern bool DX11ShaderState_Init(DevicePtr&, DX11ShaderState*);

struct Vertex {
    float x, y;
    float r, g, b, a;
};

#endif // __INC_DX11_ADAPTOR