#pragma once

#include <tuple>
#include <span>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_image.h>
#include <wrl/client.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <dxgi1_4.h>
#include "Info.hpp"
#include "helper.hpp"

namespace ModernBoy{
    using namespace std;
    using Microsoft::WRL::ComPtr;

#if defined(DEBUG) || defined(_DEBUG)
    static constexpr bool IS_DEBUG=true;
#else
    static constexpr bool IS_DEBUG=false;
#endif

    [[nodiscard]] inline SDL_Window* createWindow(
        const WindowDesc& wd
    ){
        SDL_throwIf(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS));
        constexpr auto IMG_FLAGS=IMG_INIT_JPG|IMG_INIT_PNG;
        throwIfTrue(IMG_Init(IMG_FLAGS)!=IMG_FLAGS, IMG_GetError());
#ifdef SDL_HINT_IME_SHOW_UI
        SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

        return SDL_CreateWindow(
            wd.title.c_str(),
            wd.position.x, wd.position.y,
            wd.size.x, wd.size.y,
            0
            // SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
        );
    }

    [[nodiscard]] inline HWND getHandle(SDL_Window* window){
        SDL_SysWMinfo wmInfo; SDL_VERSION(&wmInfo.version);
        SDL_GetWindowWMInfo(window, &wmInfo);

        return wmInfo.info.win.window;
    }

    [[nodiscard, maybe_unused]]
    inline std::string getRendererName(
        SDL_Renderer* renderer
    ){
        SDL_RendererInfo info;
        SDL_GetRendererInfo(renderer, &info);

        return info.name;
    }

    static constexpr UINT CREATE_DEVICE_FLAGS=IS_DEBUG ?
        D3D11_CREATE_DEVICE_DEBUG : 0;

    [[nodiscard]] inline tuple<ComPtr<ID3D11Device>,
        ComPtr<ID3D11DeviceContext>
    > createDevice(){
        // constexpr auto DRIVER_TYPE=D3D_DRIVER_TYPE_WARP;
        constexpr auto DRIVER_TYPE=D3D_DRIVER_TYPE_HARDWARE;
        constexpr D3D_FEATURE_LEVEL FEATURE_LEVELS[]={
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_9_3
        };
        ComPtr<ID3D11Device> device;
        ComPtr<ID3D11DeviceContext> context;

        D3D_FEATURE_LEVEL featureLevel;
        DX_throwIf(D3D11CreateDevice(
            nullptr, DRIVER_TYPE, 0, CREATE_DEVICE_FLAGS,
            FEATURE_LEVELS, ARRAYSIZE(FEATURE_LEVELS),
            D3D11_SDK_VERSION,
            &device, &featureLevel, &context
        ));
        printIf(featureLevel!=FEATURE_LEVELS[0], "DirectX 11 not supported.");

        return {device, context};
    }

    [[nodiscard]] inline UINT getQualityLevel(
        const ComPtr<ID3D11Device>& device
    ){
        UINT numQualityLevels;
        DX_throwIf(device->CheckMultisampleQualityLevels(
            DXGI_FORMAT_R8G8B8A8_UNORM, 4,
            &numQualityLevels
        ));
        printIf(numQualityLevels <= 0, "MSAA not supported.");

        return numQualityLevels;
    }

    [[nodiscard]] inline ComPtr<IDXGISwapChain> createSwapChain(
        HWND hwnd, const ComPtr<ID3D11Device>& device,
        const ipoint2& screenSize, UINT numQualityLevels
    ){
        const bool MSAA_4X=numQualityLevels > 0;

        DXGI_SWAP_CHAIN_DESC sc_desc{};
        // ZeroMemory(&sc_desc, sizeof(DXGI_SWAP_CHAIN_DESC));
        // one back buffer
        sc_desc.BufferCount=2;
        // set the back buffer width
        sc_desc.BufferDesc.Width=screenSize.x;
        // set the back buffer height
        sc_desc.BufferDesc.Height=screenSize.y;
        // use 32-bit color
        sc_desc.BufferDesc.Format=DXGI_FORMAT_R8G8B8A8_UNORM;
        sc_desc.BufferDesc.RefreshRate.Numerator=60;
        sc_desc.BufferDesc.RefreshRate.Denominator=1;
        // allow full-screen switching
        sc_desc.Flags=DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        // how swap chain is to be used
        sc_desc.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT;
        // the window to be used
        sc_desc.OutputWindow=hwnd;
        // how many multisamples
        sc_desc.SampleDesc.Count=MSAA_4X ? 4 : 1; 
        sc_desc.SampleDesc.Quality=MSAA_4X ? numQualityLevels-1 : 0;
        // windowed/full-screen mode
        sc_desc.Windowed=TRUE;
        sc_desc.SwapEffect=DXGI_SWAP_EFFECT_DISCARD;

        ComPtr<IDXGIDevice3> dxgiDevice;
        DX_throwIf(device.As(&dxgiDevice));

        ComPtr<IDXGIAdapter> dxgiAdapter;
        DX_throwIf(dxgiDevice->GetAdapter(&dxgiAdapter));

        ComPtr<IDXGIFactory> dxgiFactory;
        DX_throwIf(dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory)));

        ComPtr<IDXGISwapChain> swapChain;
        DX_throwIf(dxgiFactory->CreateSwapChain(device.Get(), &sc_desc, &swapChain));

        return swapChain;
    }

    [[nodiscard]] inline ComPtr<ID3D11RenderTargetView> createRTV(
        const ComPtr<ID3D11Device>& device,
        const ComPtr<IDXGISwapChain>& swapChain
    ){
        ID3D11Texture2D* pBackBuffer;
        DX_throwIf(swapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer)));
        throwIfTrue(pBackBuffer==nullptr, "CreateRenderTargetView() error");

        ComPtr<ID3D11RenderTargetView> rtv;
        DX_throwIf(device->CreateRenderTargetView(pBackBuffer, nullptr, &rtv));
        pBackBuffer->Release();

        return rtv;
    }

    [[nodiscard]] inline D3D11_VIEWPORT setScreenViewPort(
        const ipoint2& screenSize,
        const ComPtr<ID3D11DeviceContext>& context
    ){
        D3D11_VIEWPORT screenViewport{};
        // ZeroMemory(&screenViewport, sizeof(D3D11_VIEWPORT));
        screenViewport.TopLeftX=0;
        screenViewport.TopLeftY=0;
        screenViewport.Width=screenSize.x;
        screenViewport.Height=screenSize.y;
        screenViewport.MinDepth=0.0f;
        // Note: important for depth buffering
        screenViewport.MaxDepth=1.0f;

        context->RSSetViewports(1, &screenViewport);

        return screenViewport;
    }

    [[nodiscard]] inline ComPtr<ID3D11Texture2D> createDepthStencilBuffer(
        const ipoint2& screenSize, UINT numQualityLevels,
        const ComPtr<ID3D11Device>& device
    ){
        const bool MSAA_4X=numQualityLevels > 0;

        D3D11_TEXTURE2D_DESC dsb_desc;
        dsb_desc.Width=screenSize.x;
        dsb_desc.Height=screenSize.y;
        dsb_desc.MipLevels=1;
        dsb_desc.ArraySize=1;
        dsb_desc.Format=DXGI_FORMAT_D24_UNORM_S8_UINT;
        // how many multisamples
        dsb_desc.SampleDesc.Count=MSAA_4X ? 4 : 1;
        dsb_desc.SampleDesc.Quality=MSAA_4X ? numQualityLevels-1 : 0;
        dsb_desc.Usage=D3D11_USAGE_DEFAULT;
        dsb_desc.BindFlags=D3D11_BIND_DEPTH_STENCIL;
        dsb_desc.CPUAccessFlags=0;
        dsb_desc.MiscFlags=0;

        ComPtr<ID3D11Texture2D> dsb;
        DX_throwIf(device->CreateTexture2D(&dsb_desc,
            nullptr, dsb.GetAddressOf()
        ));

        return dsb;
    }

    [[nodiscard]] inline ComPtr<ID3D11DepthStencilState> createDSS(
        const ComPtr<ID3D11Device>& device
    ){
        D3D11_DEPTH_STENCIL_DESC dsd{};
        // ZeroMemory(&dsd, sizeof(D3D11_DEPTH_STENCIL_DESC));
        dsd.DepthEnable=true;
        dsd.DepthWriteMask=D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
        dsd.DepthFunc=D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

        ComPtr<ID3D11DepthStencilState> dss;
        DX_throwIf(device->CreateDepthStencilState(&dsd,
            dss.GetAddressOf())
        );

        return dss;
    }

    template <typename T>
    inline void updateBuffer(ComPtr<ID3D11Buffer>& buffer,
        const T& bufferData,
        const ComPtr<ID3D11DeviceContext>& context
    ){
        D3D11_MAPPED_SUBRESOURCE ms;
        context->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
        memcpy(ms.pData, &bufferData, sizeof(T));
        context->Unmap(buffer.Get(), 0);
    }

    static constexpr UINT COMPILE_FLAGS=IS_DEBUG ?
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION : 0;

    [[nodiscard]] inline ComPtr<ID3DBlob> compileShader(
        const wstring& fileName, const char* target
    ){
        ComPtr<ID3DBlob> errorBlob;
        ComPtr<ID3DBlob> shaderBlob;
        HRESULT hr=D3DCompileFromFile(fileName.c_str(),
            nullptr, nullptr, "main", target, COMPILE_FLAGS, 0,
            &shaderBlob, &errorBlob
        );
        SC_throwIf(hr, errorBlob.Get());

        return shaderBlob;
    }

    [[nodiscard]] inline ComPtr<ID3D11VertexShader> createVS(
        const ComPtr<ID3DBlob>& shaderBlob,
        const ComPtr<ID3D11Device>& device
    ){
        ComPtr<ID3D11VertexShader> vs;
        DX_throwIf(device->CreateVertexShader(
            shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(),
            nullptr,
            &vs
        ));

        return vs;
    }

    [[nodiscard]] inline ComPtr<ID3D11InputLayout> createIL(
        const ComPtr<ID3DBlob>& shaderBlob,
        const ComPtr<ID3D11Device>& device,
        std::span<const D3D11_INPUT_ELEMENT_DESC> iedesc
    ){
        ComPtr<ID3D11InputLayout> il;
        DX_throwIf(device->CreateInputLayout(
            iedesc.data(), static_cast<UINT>(iedesc.size()),
            shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(),
            &il
        ));

        return il;
    }

    [[nodiscard]] inline tuple<ComPtr<ID3D11VertexShader>,
        ComPtr<ID3D11InputLayout>
    > createVSAndIL(
        const wstring& fileName,
        const ComPtr<ID3D11Device>& device,
        std::span<const D3D11_INPUT_ELEMENT_DESC> iedesc
    ){
        auto shaderBlob=compileShader(fileName, "vs_5_0");

        return {
            createVS(shaderBlob, device),
            createIL(shaderBlob, device, iedesc)
        };
    }

    [[nodiscard]] inline ComPtr<ID3D11PixelShader> createPS(
        const wstring& fileName,
        const ComPtr<ID3D11Device>& device
    ){
        auto shaderBlob=compileShader(fileName, "ps_5_0");

        ComPtr<ID3D11PixelShader> pixelShader;
        DX_throwIf(device->CreatePixelShader(
            shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(),
            nullptr,
            &pixelShader
        ));

        return pixelShader;
    }

    [[nodiscard]] inline ComPtr<ID3D11RasterizerState> createRS(
        const ComPtr<ID3D11Device>& device
    ){
        D3D11_RASTERIZER_DESC rd{};
        // ZeroMemory(&rd, sizeof(D3D11_RASTERIZER_DESC));
        rd.FillMode=D3D11_FILL_MODE::D3D11_FILL_SOLID;
        // rd.FillMode=D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
        // rd.CullMode=D3D11_CULL_MODE::D3D11_CULL_NONE;
        rd.CullMode=D3D11_CULL_MODE::D3D11_CULL_BACK;
        rd.FrontCounterClockwise=false;
        rd.DepthClipEnable=true;

        ComPtr<ID3D11RasterizerState> rs;
        DX_throwIf(device->CreateRasterizerState(&rd, &rs));

        return rs;
    }

    template<typename V>
    [[nodiscard]] inline ComPtr<ID3D11Buffer> createVB(
        std::span<const V> vertices,
        const ComPtr<ID3D11Device>& device
    ){
        D3D11_BUFFER_DESC bufferDesc{};
        // ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
        // write access access by CPU and GPU
        // bufferDesc.Usage=D3D11_USAGE_DYNAMIC;
        bufferDesc.Usage=D3D11_USAGE_IMMUTABLE;
        bufferDesc.ByteWidth=UINT(sizeof(V) * vertices.size());
        // use as a vertex buffer
        bufferDesc.BindFlags=D3D11_BIND_VERTEX_BUFFER;
        // allow CPU to write in buffer
        // bufferDesc.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
        bufferDesc.CPUAccessFlags=0;
        bufferDesc.StructureByteStride=sizeof(V);

        D3D11_SUBRESOURCE_DATA vertexBufferData{
            .pSysMem=vertices.data(),
            .SysMemPitch=0,
            .SysMemSlicePitch=0
        };

        ComPtr<ID3D11Buffer> vertexBuffer;
        DX_throwIf(device->CreateBuffer(&bufferDesc,
            &vertexBufferData,
            vertexBuffer.GetAddressOf()
        ));

        return vertexBuffer;
    }

    [[nodiscard]] inline ComPtr<ID3D11Buffer> createIB(
        std::span<const uint16_t> indices,
        const ComPtr<ID3D11Device>& device
    ){
        D3D11_BUFFER_DESC bufferDesc{};
        // write access access by CPU and GPU
        // bufferDesc.Usage=D3D11_USAGE_DYNAMIC;
        bufferDesc.Usage=D3D11_USAGE_IMMUTABLE;
        bufferDesc.ByteWidth=static_cast<UINT>(sizeof(uint16_t) * indices.size());
        // use as a index buffer
        bufferDesc.BindFlags=D3D11_BIND_INDEX_BUFFER;
        // allow CPU to write in buffer
        // bufferDesc.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
        bufferDesc.CPUAccessFlags=0;
        bufferDesc.StructureByteStride=sizeof(uint16_t);

        D3D11_SUBRESOURCE_DATA indexBufferData{
            .pSysMem=indices.data(),
            .SysMemPitch=0,
            .SysMemSlicePitch=0
        };

        ComPtr<ID3D11Buffer> indexBuffer;
        DX_throwIf(device->CreateBuffer(&bufferDesc,
            &indexBufferData,
            indexBuffer.GetAddressOf()
        ));

        return indexBuffer;
    }

    template <typename T>
    [[nodiscard]] inline ComPtr<ID3D11Buffer> createCB(
        const T& constants,
        const ComPtr<ID3D11Device>& device
    ){
        D3D11_BUFFER_DESC cbDesc;
        cbDesc.ByteWidth=sizeof(T);
        cbDesc.Usage=D3D11_USAGE_DYNAMIC;
        cbDesc.BindFlags=D3D11_BIND_CONSTANT_BUFFER;
        cbDesc.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
        cbDesc.MiscFlags=0;
        cbDesc.StructureByteStride=0;

        // Fill in the subresource data.
        D3D11_SUBRESOURCE_DATA constantBufferData{
            .pSysMem=&constants,
            .SysMemPitch=0,
            .SysMemSlicePitch=0
        };

        ComPtr<ID3D11Buffer> constantBuffer;
        DX_throwIf(device->CreateBuffer(&cbDesc,
            &constantBufferData,
            constantBuffer.GetAddressOf()
        ));

        return constantBuffer;
    }

    [[nodiscard]] inline ComPtr<ID3D11SamplerState> createSS(
        const ComPtr<ID3D11Device>& device
    ){
        D3D11_SAMPLER_DESC sd{};
        // ZeroMemory(&sd, sizeof(D3D11_SAMPLER_DESC));
        sd.Filter=D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sd.AddressU=D3D11_TEXTURE_ADDRESS_WRAP;
        sd.AddressV=D3D11_TEXTURE_ADDRESS_WRAP;
        sd.AddressW=D3D11_TEXTURE_ADDRESS_WRAP;
        sd.ComparisonFunc=D3D11_COMPARISON_NEVER;
        sd.MinLOD=0;
        sd.MaxLOD=D3D11_FLOAT32_MAX;

        ComPtr<ID3D11SamplerState> sampler;
        DX_throwIf(device->CreateSamplerState(&sd,
            sampler.GetAddressOf()
        ));

        return sampler;
    }
}
