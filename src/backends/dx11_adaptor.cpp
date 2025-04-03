#include <string>
#include <dxgi.h>
#include <dxgi1_4.h>
#include "exception.hpp"
#include "backends/dx11_adaptor.hpp"

using namespace Microsoft::WRL;

/* Renderer Initializers Start */
static bool _DX11Device_Init(DevicePtr& out_device,
    ContextPtr& out_context, UINT* out_qualityLevels)
{
    D3D_FEATURE_LEVEL FEATURE_LEVELS[]={
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };
    D3D_FEATURE_LEVEL featureLevel;

    if(D3D11CreateDevice(nullptr,
        D3D_DRIVER_TYPE_HARDWARE, 0, D3D11_CREATE_DEVICE_DEBUG,
        FEATURE_LEVELS, ARRAYSIZE(FEATURE_LEVELS), D3D11_SDK_VERSION,
        out_device.GetAddressOf(), &featureLevel, out_context.GetAddressOf()
    ) < 0) return false;

    if(FEATURE_LEVELS[0] != featureLevel) return false;

    if(out_device->CheckMultisampleQualityLevels(
        DXGI_FORMAT_R8G8B8A8_UNORM, 4,
        out_qualityLevels
    )) return false;

    return true;
}

static bool _DX11RenderTarget_Init(HWND in_hwnd, UINT in_qualityLevels,
    DevicePtr& in_device, ContextPtr& in_context,
    ComPtr<IDXGISwapChain>& out_swapChain,
    ComPtr<ID3D11RenderTargetView>& out_rtv, D3D11_VIEWPORT& out_viewport)
{
    const bool MSAA_4X = in_qualityLevels > 0;
    RECT rect; GetWindowRect(in_hwnd, &rect);
    UINT w=rect.right-rect.left, h=rect.bottom-rect.top;

    // Init SwapChain
    DXGI_SWAP_CHAIN_DESC scDesc{};
    // ZeroMemory(&scDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
    // one back buffer
    scDesc.BufferCount=2;
    // set the back buffer width
    scDesc.BufferDesc.Width=w;
    // set the back buffer height
    scDesc.BufferDesc.Height=h;
    // use 32-bit color
    scDesc.BufferDesc.Format=DXGI_FORMAT_R8G8B8A8_UNORM;
    scDesc.BufferDesc.RefreshRate.Numerator=60;
    scDesc.BufferDesc.RefreshRate.Denominator=1;
    // allow full-screen switching
    scDesc.Flags=DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    // how swap chain is to be used
    scDesc.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT;
    // the window to be used
    scDesc.OutputWindow=in_hwnd;
    // how many multisamples
    scDesc.SampleDesc.Count=MSAA_4X ? 4 : 1; 
    scDesc.SampleDesc.Quality=MSAA_4X ? in_qualityLevels-1 : 0;
    // windowed/full-screen mode
    scDesc.Windowed=TRUE;
    scDesc.SwapEffect=DXGI_SWAP_EFFECT_DISCARD;

    ComPtr<IDXGIDevice3> dxgiDevice;
    ComPtr<IDXGIAdapter> dxgiAdapter;
    ComPtr<IDXGIFactory> dxgiFactory;

    if(in_device.As(&dxgiDevice) < 0)
        return false;
    if(dxgiDevice->GetAdapter(&dxgiAdapter) < 0)
        return false;
    if(dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory)) < 0)
        return false;
    if(dxgiFactory->CreateSwapChain(in_device.Get(), &scDesc,
        out_swapChain.GetAddressOf()) < 0) return false;

    // Init RenderTargetView
    ID3D11Texture2D* pBackBuffer;
    if(out_swapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer)) < 0)
        return false;
    if(pBackBuffer == nullptr)
        return false;

    if(in_device->CreateRenderTargetView(pBackBuffer, nullptr,
        out_rtv.GetAddressOf()) < 0) return false;

    pBackBuffer->Release();

    // Init Viewport
    // ZeroMemory(&out_viewport, sizeof(D3D11_VIEWPORT));
    out_viewport.TopLeftX=0;
    out_viewport.TopLeftY=0;
    out_viewport.Width=w;
    out_viewport.Height=h;
    out_viewport.MinDepth=0.0f;
    // Note: important for depth buffering
    out_viewport.MaxDepth=1.0f;

    in_context->RSSetViewports(1, &out_viewport);

    return true;
}

static bool _DX11DepthStencil_Init(HWND in_hwnd, UINT in_qualityLevels,
    DevicePtr& in_device, ComPtr<ID3D11Texture2D>& out_dsb,
    ComPtr<ID3D11DepthStencilView>& out_dsv,
    ComPtr<ID3D11DepthStencilState>& out_dss)
{
    const bool MSAA_4X=in_qualityLevels > 0;
    RECT rect; GetWindowRect(in_hwnd, &rect);
    UINT w=rect.right-rect.left, h=rect.bottom-rect.top;

    // Init DepthStencil Buffer
    D3D11_TEXTURE2D_DESC dsbDesc;
    dsbDesc.Width=w;
    dsbDesc.Height=h;
    dsbDesc.MipLevels=1;
    dsbDesc.ArraySize=1;
    dsbDesc.Format=DXGI_FORMAT_D24_UNORM_S8_UINT;
    // how many multisamples
    dsbDesc.SampleDesc.Count=MSAA_4X ? 4 : 1;
    dsbDesc.SampleDesc.Quality=MSAA_4X ? in_qualityLevels-1 : 0;
    dsbDesc.Usage=D3D11_USAGE_DEFAULT;
    dsbDesc.BindFlags=D3D11_BIND_DEPTH_STENCIL;
    dsbDesc.CPUAccessFlags=0;
    dsbDesc.MiscFlags=0;

    if(in_device->CreateTexture2D(&dsbDesc, nullptr,
        out_dsb.GetAddressOf()) < 0) return false;
    // Init DepthStencilView
    if(in_device->CreateDepthStencilView(out_dsb.Get(), 0,
        out_dsv.GetAddressOf()) < 0) return false;

    // Init DepthStencilState
    D3D11_DEPTH_STENCIL_DESC dsd{};
    // ZeroMemory(&out_dsd, sizeof(D3D11_DEPTH_STENCIL_DESC));
    dsd.DepthEnable=true;
    dsd.DepthWriteMask=D3D11_DEPTH_WRITE_MASK_ALL;
    dsd.DepthFunc=D3D11_COMPARISON_LESS_EQUAL;

    if(in_device->CreateDepthStencilState(&dsd,
        out_dss.GetAddressOf()) < 0) return false;

    return true;
}

bool DX11RenderState_Init(HWND in_hwnd, DX11RenderState* out_renderState){
    if(!_DX11Device_Init(out_renderState->device, out_renderState->context,
        &out_renderState->qualityLevels)) return false;
    if(!_DX11RenderTarget_Init(in_hwnd, out_renderState->qualityLevels,
        out_renderState->device, out_renderState->context,
        out_renderState->swapChain, out_renderState->rtv,
        out_renderState->viewport)) return false;
    if(!_DX11DepthStencil_Init(in_hwnd, out_renderState->qualityLevels,
        out_renderState->device, out_renderState->dsb, out_renderState->dsv,
        out_renderState->dss)) return false;
    
    if(!DX11ShaderState_Init(out_renderState->device,
        &out_renderState->shaderState)) return false;

    return true;
}
/* Renderer Initializers End */

/* Shader Initializers Start */
static bool _DX11Rasterizer_Init(DevicePtr& in_device,
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
static bool _DX11ConstantBuffer_Init(DevicePtr& in_device,
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
template<typename T>
static bool _DX11VertexBuffer_Init(DevicePtr& in_device,
    const T& in_vertices, BufferPtr& out_buffer)
{
    D3D11_BUFFER_DESC vbDesc;
    vbDesc.ByteWidth=sizeof(in_vertices);
    vbDesc.Usage=D3D11_USAGE_IMMUTABLE;
    vbDesc.BindFlags=D3D11_BIND_VERTEX_BUFFER;
    vbDesc.CPUAccessFlags=0;
    vbDesc.MiscFlags=0;
    vbDesc.StructureByteStride=sizeof(in_vertices[0]);

    // Fill in the subresource data.
    D3D11_SUBRESOURCE_DATA vertexBufferData{
        .pSysMem=in_vertices,
        .SysMemPitch=0,
        .SysMemSlicePitch=0
    };

    if(in_device->CreateBuffer(&vbDesc, &vertexBufferData,
        out_buffer.GetAddressOf()) < 0) return false;

    return true;
}
template<typename T>
static bool _DX11IndexBuffer_Init(DevicePtr& in_device,
    const T& in_indices, BufferPtr& out_buffer)
{
    D3D11_BUFFER_DESC ibDesc;
    ibDesc.ByteWidth=sizeof(in_indices);
    ibDesc.Usage=D3D11_USAGE_IMMUTABLE;
    ibDesc.BindFlags=D3D11_BIND_INDEX_BUFFER;
    ibDesc.CPUAccessFlags=0;
    ibDesc.MiscFlags=0;
    ibDesc.StructureByteStride=sizeof(in_indices[0]);

    // Fill in the subresource data.
    D3D11_SUBRESOURCE_DATA indexBufferData{
        .pSysMem=in_indices,
        .SysMemPitch=0,
        .SysMemSlicePitch=0
    };

    if(in_device->CreateBuffer(&ibDesc, &indexBufferData,
        out_buffer.GetAddressOf()) < 0) return false;

    return true;
}

#include <span>
#include <d3dcompiler.h>

#if defined(DEBUG) || defined(_DEBUG)
    static constexpr bool IS_DEBUG=true;
#else
    static constexpr bool IS_DEBUG=false;
#endif

static constexpr UINT COMPILE_FLAGS=IS_DEBUG ?
    D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION : 0;

using BlobPtr = ComPtr<ID3DBlob>;

static bool _DX11_CompileShader(const std::wstring& in_fileName,
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

static bool _DX11Shader_Init(const std::wstring& in_vsFileName,
    const std::wstring& in_psFileName, DevicePtr& in_device,
    std::span<const D3D11_INPUT_ELEMENT_DESC> in_iedesc,
    VertexShaderPtr& out_vs, InputLayoutPtr& out_il,
    PixelShaderPtr& out_ps)
{
    BlobPtr vsblob, psblob;

    if(!_DX11_CompileShader(in_vsFileName, "vs_5_0", vsblob))
        return false;
    if(!_DX11_CompileShader(in_psFileName, "ps_5_0", psblob))
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

Vertex rect[] = {
    { -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f },
    { 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f },
    { 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f },
    { -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f },
};

UINT indices[] = {
    0, 1, 2,
    0, 2, 3
};

bool DX11ShaderState_Init(DevicePtr& in_device,
    DX11ShaderState* out_shaderState)
{
    // Init Default Rasterizer
    if(!_DX11Rasterizer_Init(in_device, D3D11_FILL_SOLID,
        out_shaderState->rs)) return false;
    // Init WireState Rasterizer
    if(!_DX11Rasterizer_Init(in_device, D3D11_FILL_WIREFRAME,
        out_shaderState->wireState)) return false;

    // Init Vertex Shader Constants
    if(!_DX11ConstantBuffer_Init(in_device, out_shaderState->vsc,
        out_shaderState->vscBuffer)) return false;
    // Init Pixel Shader Constants
    if(!_DX11ConstantBuffer_Init(in_device, out_shaderState->psc,
        out_shaderState->pscBuffer)) return false;
    
    // Init shaders
    constexpr D3D11_INPUT_ELEMENT_DESC inputElements[]={
        {
            .SemanticName="POSITION",
            .SemanticIndex=0,
            .Format=DXGI_FORMAT_R32G32_FLOAT,
            .InputSlot=0,
            .AlignedByteOffset=offsetof(Vertex, x),
            .InputSlotClass=D3D11_INPUT_PER_VERTEX_DATA,
            .InstanceDataStepRate=0
        }, {
            .SemanticName="COLOR",
            .SemanticIndex=0,
            .Format=DXGI_FORMAT_R32G32B32A32_FLOAT,
            .InputSlot=0,
            .AlignedByteOffset=offsetof(Vertex, r),
            .InputSlotClass=D3D11_INPUT_PER_VERTEX_DATA,
            .InstanceDataStepRate=0
        }
    };

    if(!_DX11Shader_Init(L"shaders/VS.hlsl", L"shaders/PS.hlsl",
        in_device, inputElements, out_shaderState->vs,
        out_shaderState->il, out_shaderState->ps
    )) return false;

    // Temporal Codes...
    if(!_DX11VertexBuffer_Init(in_device, rect,
        out_shaderState->vertexBuffer)) return false;
    if(!_DX11IndexBuffer_Init(in_device, indices,
        out_shaderState->indexBuffer)) return false;
    // <-- End

    return true;
}
/* Shader Initializers End */