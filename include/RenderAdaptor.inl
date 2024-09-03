#pragma once

#include <tuple>
#include <wrl/client.h>
#include <d3d11.h>
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

    static constexpr UINT CREATE_DEVICE_FLAGS=IS_DEBUG ?
        D3D11_CREATE_DEVICE_DEBUG : 0;

    /**
     * @brief create DirectX 11 default device and context
     * 
     * @return smart pointer for device and context
    **/
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

    /**
     * @brief check MSAA support and get Quality Level
     * 
     * @param device DirectX 11 device
     * @return 0 for not support
    **/
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

    /**
     * @brief create default swap chain for DirectX device
     * 
     * @param hwnd window handle
     * @param device DirectX device
     * @param screenSize use for resolution
     * @param numQualityLevels MSAA param
     * @return smart pointer for swap chain
    **/
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

    // create DirectX 11 render target view
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

    [[nodiscard]] inline tuple<ComPtr<ID3D11Texture2D>,
        ComPtr<ID3D11DepthStencilView>
    > createDepthStencil(
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
        ComPtr<ID3D11DepthStencilView> dsv;
        DX_throwIf(device->CreateTexture2D(&dsb_desc,
            nullptr, dsb.GetAddressOf()
        ));
        DX_throwIf(device->CreateDepthStencilView(
            dsb.Get(), 0, &dsv
        ));

        return {dsb, dsv};
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
}
