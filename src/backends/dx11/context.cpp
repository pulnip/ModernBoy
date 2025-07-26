#include <stdexcept>
#include <SDL3/SDL_log.h>
#include <dxgi.h>
#include <dxgi1_4.h>
#include "backends/dx11/context.hpp"
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_dx11.h>

using namespace Microsoft::WRL;
using namespace ModernBoy::DX11;

static bool InitDevice(DevicePtr& out_device, ContextPtr& out_context,
    UINT& out_qualityLevels);
static bool InitRenderTarget(HWND in_hwnd, UINT in_qualityLevels,
    DevicePtr& in_device, ContextPtr& in_context,
    ComPtr<IDXGISwapChain>& out_swapChain,
    ComPtr<ID3D11RenderTargetView>& out_rtv, D3D11_VIEWPORT& out_viewport);
static bool InitDepthStencil(HWND in_hwnd, UINT in_qualityLevels,
    DevicePtr& in_device, ComPtr<ID3D11Texture2D>& out_dsb,
    ComPtr<ID3D11DepthStencilView>& out_dsv,
    ComPtr<ID3D11DepthStencilState>& out_dss);
static bool InitGUI(SDL_Window* window,
    DevicePtr& in_device, ContextPtr& in_context);

RenderContext::~RenderContext(){
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}
RenderContext::RenderContext(RenderContext&& other)
:meshManager(other.meshManager), shaderManager(other.shaderManager){
    moveFrom(std::move(other));
}
void RenderContext::moveFrom(RenderContext&& other){
    device.Swap(other.device);
    context.Swap(other.context);
    qualityLevels=other.qualityLevels;

    swapChain.Swap(other.swapChain);
    rtv.Swap(other.rtv);
    viewport=other.viewport;

    dsb.Swap(other.dsb);
    dsv.Swap(other.dsv);
    dss.Swap(other.dss);
}


RenderContext::RenderContext(SDL_Window* in_window){
    HWND hwnd = (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(in_window),
        SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);

    if(!InitDevice(device, context, qualityLevels))
        throw std::runtime_error("Failed to create device and context.");
    if(!InitRenderTarget(hwnd, qualityLevels, device, context,
        swapChain, rtv, viewport))
        throw std::runtime_error("Failed to create render target.");
    if(!InitDepthStencil(hwnd, qualityLevels, device,
        dsb, dsv, dss))
        throw std::runtime_error("Failed to create depth stencil.");

    // Init ImGui
    if(!InitGUI(in_window, device, context))
        throw std::runtime_error("Failed to initialize ImGui.");
}

void RenderContext::onFrameStart(Vec4 clearColor){
    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplSDL3_NewFrame();

    ImGui::NewFrame();
    ImGui::ShowDemoWindow(); // Show demo window! :)

    context->RSSetViewports(1, &viewport);
    context->OMSetRenderTargets(1, rtv.GetAddressOf(), dsv.Get());
    context->ClearRenderTargetView(rtv.Get(), clearColor.v);
    context->OMSetDepthStencilState(dss.Get(), 0);
    context->ClearDepthStencilView(dsv.Get(),
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0
    );

    shaderManager.get(cmd.shaderHandle)->bind(*this);
}

void RenderContext::setView(float fov, Vec3 viewPos, Vec4 viewQuat){
    
}

void RenderContext::drawMesh(Vec3 position, Vec4 rotation,Vec3 scale,
    NativePtr mesh, float alpha
){
    auto pMesh = meshManager.get(cmd.meshHandle);
    if(pMesh != nullptr){
        pMesh->bind(*this);
        context->DrawIndexed(pMesh->numIndices, 0, 0);
    }
}
void RenderContext::onFrameEnd(){
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    swapChain->Present(1, 0);
}

static bool InitDevice(DevicePtr& out_device,
    ContextPtr& out_context, UINT& out_qualityLevels)
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
        &out_qualityLevels
    )) return false;

    return true;
}

static bool InitRenderTarget(HWND in_hwnd, UINT in_qualityLevels,
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

static bool InitDepthStencil(HWND in_hwnd, UINT in_qualityLevels,
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

static bool InitGUI(SDL_Window* in_window,
    DevicePtr& in_device, ContextPtr& in_context)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    int w, h;
    if(!SDL_GetWindowSize(in_window, &w, &h)){
        SDL_Log("SDL_GetWindowSize Failed: %s", SDL_GetError());
        return false;
    }

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.DisplaySize = ImVec2(w, h);

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForD3D(in_window);
    ImGui_ImplDX11_Init(in_device.Get(), in_context.Get());

    return true;
}
