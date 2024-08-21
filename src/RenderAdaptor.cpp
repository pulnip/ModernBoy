#include <print>
#include "RenderAdaptor.inl"
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_dx11.h>
#include "MeshBuffer.hpp"
#include "RenderAdaptor.hpp"
#include "ShaderConstant.hpp"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace ModernBoy;

RenderAdaptor::RenderAdaptor(HWND hwnd, const ipoint2& screenSize){
    tie(device, context)=createDevice();

    qualityLevel=getQualityLevel(device);
    swapChain=createSwapChain(hwnd, device, screenSize, qualityLevel);

    rtv=createRTV(device, swapChain);
    screenViewport=setScreenViewPort(screenSize, context);

    tie(depthStencilBuffer, dsv)=createDepthStencil(
        screenSize, qualityLevel, device
    );

    dss=createDSS(device);
}

RenderAdaptor::~RenderAdaptor()=default;

void RenderAdaptor::recreateRenderTarget(int w, int h){
    rtv.Reset();
    swapChain->ResizeBuffers(0,
        w, h,
        DXGI_FORMAT_UNKNOWN,
        DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
    );
    rtv=createRTV(device, swapChain);
    tie(depthStencilBuffer, dsv)=createDepthStencil(
        {w, h}, qualityLevel, device
    );
    screenViewport=setScreenViewPort({w, h}, context);
}

void RenderAdaptor::setup(){
    // IA: Input-Assembler stage
    // VS: Vertex Shader
    // PS: Pixel Shader
    // RS: Rasterizer stage
    // OM: Output-Merger stage

    float clearColor[4]={0.0f, 0.0f, 0.0f, 1.0f};
    context->RSSetViewports(1, &screenViewport);
    // use depth buffer
    context->OMSetRenderTargets(1, rtv.GetAddressOf(),
        dsv.Get()
    );
    context->ClearRenderTargetView(rtv.Get(), clearColor);
    context->OMSetDepthStencilState(dss.Get(), 0);
    context->ClearDepthStencilView(dsv.Get(),
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0
    );
}

void RenderAdaptor::swap(){
    // Present with vsync
    swapChain->Present(1, 0);
    // Present without vsync
    // pImpl->swapChain->Present(0, 0);
}
