#include <stdexcept>
#include <span>
#include <d3dcompiler.h>
#include "backends/dx11/mesh.hpp"
#include "backends/dx11/context.hpp"
#include "backends/dx11/shader.hpp"

#if defined(DEBUG) || defined(_DEBUG)
    static constexpr bool IS_DEBUG=true;
#else
    static constexpr bool IS_DEBUG=false;
#endif

static constexpr UINT COMPILE_FLAGS=IS_DEBUG ?
    D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION : 0;

using namespace Microsoft::WRL;
using namespace ModernBoy::DX11;

using BlobPtr = ComPtr<ID3DBlob>;

static bool _Rasterizer_Init(DevicePtr& in_device, D3D11_FILL_MODE in_fillMode,
    RasterizerPtr& out_rs);
template<typename T>
static bool _ConstantBuffer_Init(DevicePtr& in_device, const T& in_constants,
    BufferPtr& out_buffer);
static bool _Shader_Init(const std::wstring& in_vsFileName,
    const std::wstring& in_psFileName, DevicePtr& in_device,
    std::span<const D3D11_INPUT_ELEMENT_DESC> in_iedesc,
    InputLayoutPtr& out_il, VertexShaderPtr& out_vs, PixelShaderPtr& out_ps);

Shader::Shader(DevicePtr& in_device, const std::wstring& in_vsFileName,
    const std::wstring& in_psFileName)
{
    // Init Default Rasterizer
    if(!_Rasterizer_Init(in_device, D3D11_FILL_SOLID, rs))
        throw std::runtime_error("Failed to create rasterizer state.");
    // Init WireState Rasterizer
    if(!_Rasterizer_Init(in_device, D3D11_FILL_WIREFRAME, wireState))
        throw std::runtime_error("Failed to create wireframe rasterizer state.");

    // Init Vertex Shader Constants
    if(!_ConstantBuffer_Init(in_device, vsc, vscBuffer))
        throw std::runtime_error("Failed to create vertex shader constant buffer.");
    // Init Pixel Shader Constants
    if(!_ConstantBuffer_Init(in_device, psc, pscBuffer))
        throw std::runtime_error("Failed to create pixel shader constant buffer.");

    // Init shaders
    if(!_Shader_Init(in_vsFileName, in_psFileName,
        in_device, smolVertexDesc, il, vs, ps))
        throw std::runtime_error("Failed to create shaders.");
}
Shader::~Shader(){
    ps.Reset();
    vs.Reset();
    il.Reset();

    pscBuffer.Reset();
    vscBuffer.Reset();

    rs.Reset();
    wireState.Reset();
}

void Shader::bind(RenderContext& in_context) const{
    auto& ctx = in_context.context;

    // set the shader objects
    ctx->VSSetShader(vs.Get(), 0, 0);
    ctx->PSSetShader(ps.Get(), 0, 0);
}

static bool _Rasterizer_Init(DevicePtr& in_device,
    D3D11_FILL_MODE in_fillMode, RasterizerPtr& out_rs)
{
    D3D11_CULL_MODE cullMode = in_fillMode==D3D11_FILL_SOLID ?
        D3D11_CULL_BACK : D3D11_CULL_NONE;

    D3D11_RASTERIZER_DESC rd{};
    // ZeroMemory(&rd, sizeof(D3D11_RASTERIZER_DESC));
    rd.FillMode=in_fillMode;
    rd.CullMode=cullMode;
    rd.FrontCounterClockwise=false;
    rd.DepthClipEnable=true;

    if(in_device->CreateRasterizerState(&rd, out_rs.GetAddressOf()) < 0)
        return false;

    return true;
}

template<typename T>
static bool _ConstantBuffer_Init(DevicePtr& in_device,
    const T& in_constants, BufferPtr& out_buffer)
{
    D3D11_BUFFER_DESC cbDesc;
    cbDesc.ByteWidth=sizeof(in_constants);
    cbDesc.Usage=D3D11_USAGE_DYNAMIC;
    cbDesc.BindFlags=D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
    cbDesc.MiscFlags=0;
    cbDesc.StructureByteStride=0;

    // Fill in the subresource data.
    D3D11_SUBRESOURCE_DATA constantBufferData{
        .pSysMem=&in_constants,
        .SysMemPitch=0,
        .SysMemSlicePitch=0
    };

    if(in_device->CreateBuffer(&cbDesc, &constantBufferData,
        out_buffer.GetAddressOf()) < 0) return false;

    return true;
}

static bool _CompileShader(const std::wstring& in_fileName,
    const char* in_target, BlobPtr& out_shaderBlob)
{
    BlobPtr errorBlob;

    if(D3DCompileFromFile(in_fileName.c_str(),
        nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main", in_target, COMPILE_FLAGS, 0,
        out_shaderBlob.GetAddressOf(), errorBlob.GetAddressOf()
    ) < 0) return false;

    return true;
}

static bool _Shader_Init(const std::wstring& in_vsFileName,
    const std::wstring& in_psFileName, DevicePtr& in_device,
    std::span<const D3D11_INPUT_ELEMENT_DESC> in_iedesc,
    InputLayoutPtr& out_il, VertexShaderPtr& out_vs, PixelShaderPtr& out_ps)
{
    BlobPtr vsblob, psblob;

    if(!_CompileShader(in_vsFileName, "vs_5_0", vsblob))
        return false;
    if(!_CompileShader(in_psFileName, "ps_5_0", psblob))
        return false;

    if(in_device->CreateVertexShader(vsblob->GetBufferPointer(),
        vsblob->GetBufferSize(), nullptr, out_vs.GetAddressOf()
    ) < 0) return false;
    if(in_device->CreateInputLayout(in_iedesc.data(),
        static_cast<UINT>(in_iedesc.size()), vsblob->GetBufferPointer(),
        vsblob->GetBufferSize(), out_il.GetAddressOf()
    ) < 0) return false;

    if(in_device->CreatePixelShader(psblob->GetBufferPointer(),
        psblob->GetBufferSize(), nullptr, out_ps.GetAddressOf()
    ) < 0) return false;

    return true;
}
