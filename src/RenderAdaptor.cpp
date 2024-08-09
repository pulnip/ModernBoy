#include <span>
#include <tuple>
#include <vector>
#include <gsl/gsl-lite.hpp>
#include <wrl/client.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <dxgi1_4.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_dx11.h>
#include <stb_image.h>
#include "helper.hpp"
#include "MeshBuffer.hpp"
#include "Vertex.hpp"
#include "RenderAdaptor.hpp"

using namespace std;
using namespace gsl;
using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace ModernBoy;
using Microsoft::WRL::ComPtr;

#if defined(DEBUG) || defined(_DEBUG)
static constexpr bool IS_DEBUG=true;
#else
static constexpr bool IS_DEBUG=false;
#endif
static constexpr UINT CREATE_DEVICE_FLAGS=IS_DEBUG ?
    D3D11_CREATE_DEVICE_DEBUG : 0;
static constexpr UINT COMPILE_FLAGS=IS_DEBUG ?
    D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION : 0;

struct RenderAdaptor::Impl final{
    SDL_Window* window=nullptr;

    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;
    ComPtr<ID3D11RenderTargetView> rtv;
    ComPtr<IDXGISwapChain> swapChain;

    // depth buffer
    ComPtr<ID3D11Texture2D> depthStencilBuffer;
    ComPtr<ID3D11DepthStencilView> dsv;
    ComPtr<ID3D11DepthStencilState> dss;

    D3D11_VIEWPORT screenViewport{};

    Impl(const WindowDesc& wd);
    ~Impl();

    void setupRender();

    void cleanupDeviceD3D();
    void cleanupRenderTarget();

  private:
    void initDirect3D(const WindowDesc& wd);
    void initGUI(const WindowDesc& wd);
};

struct RenderAdaptor::ShaderAdaptor final{
    ComPtr<ID3D11VertexShader> vs;
    ComPtr<ID3D11PixelShader> ps;
    ComPtr<ID3D11InputLayout> il;
    ComPtr<ID3D11RasterizerState> rs;

    ComPtr<ID3D11Buffer> vertexBuffer;
    ComPtr<ID3D11Buffer> indexBuffer;
    ComPtr<ID3D11Buffer> vscBuffer;
    ComPtr<ID3D11Buffer> pscBuffer;

    UINT indexCount=0;

    VSConstants vsc;
    PSConstants psc;

    ShaderAdaptor(const ComPtr<ID3D11Device>& device,
        const wstring& vsFileName, const wstring& psFileName
    );

    void update(float dt,
        const ComPtr<ID3D11DeviceContext>& context,
        const Camera& camera, const float xSplit
    );
    void render(const ComPtr<ID3D11DeviceContext>& context);
};

struct RenderAdaptor::TextureAdaptor final{
    ComPtr<ID3D11Texture2D> texture;
    ComPtr<ID3D11ShaderResourceView> textureView;
    ComPtr<ID3D11SamplerState> sampler;

    TextureAdaptor(const ComPtr<ID3D11Device>& device,
        const string& fileName
    );

    void render(const ComPtr<ID3D11DeviceContext>& context);
};

[[nodiscard]] static SDL_Window* createWindow(
    const WindowDesc& wd
){
    SDL_throwIf(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS));
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

[[nodiscard]] static HWND getHandle(SDL_Window* window){
    SDL_SysWMinfo wmInfo; SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(window, &wmInfo);

    return wmInfo.info.win.window;
}

[[nodiscard, maybe_unused]]
static std::string getRendererName(
    SDL_Renderer* renderer
){
    SDL_RendererInfo info;
    SDL_GetRendererInfo(renderer, &info);

    return info.name;
}

[[nodiscard]] static tuple<ComPtr<ID3D11Device>,
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

[[nodiscard]] static UINT getQualityLevel(
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

[[nodiscard]] static ComPtr<IDXGISwapChain> createSwapChain(
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

[[nodiscard]] static ComPtr<ID3D11RenderTargetView> createRTV(
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

[[nodiscard]] static D3D11_VIEWPORT setScreenViewPort(
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

[[nodiscard]] static ComPtr<ID3D11Texture2D> createDepthStencilBuffer(
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

[[nodiscard]] static ComPtr<ID3D11DepthStencilState> createDSS(
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
static void updateBuffer(ComPtr<ID3D11Buffer>& buffer,
    const T& bufferData,
    const ComPtr<ID3D11DeviceContext>& context
){
    D3D11_MAPPED_SUBRESOURCE ms;
    context->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
    memcpy(ms.pData, &bufferData, sizeof(T));
    context->Unmap(buffer.Get(), 0);
}

RenderAdaptor::RenderAdaptor(const WindowDesc& wd)
: pImpl(std::make_unique<Impl>(wd))
, shader(std::make_unique<ShaderAdaptor>(pImpl->device,
    L"src/CVS.hlsl", L"src/CPS.hlsl"))
, texturer(std::make_unique<TextureAdaptor>(pImpl->device,
    "assets/crate.png"))
{
    camera.setScreenSize(wd.size);
}

RenderAdaptor::~RenderAdaptor()=default;

bool RenderAdaptor::run(){
    SDL_Event event;
    if(SDL_PollEvent(&event)){
        ImGui_ImplSDL2_ProcessEvent(&event);
        switch(event.type){
        case SDL_QUIT:
            return false;
        case SDL_WINDOWEVENT:
            // print("SDL Window Event");
            if(event.window.windowID==SDL_GetWindowID(pImpl->window)){
                switch(event.window.event){
                case SDL_WINDOWEVENT_CLOSE:
                    return false;
                case SDL_WINDOWEVENT_RESIZED:
                    camera.setScreenSize({event.window.data1, event.window.data2});
                    break;
                    pImpl->cleanupRenderTarget();
                    pImpl->swapChain->ResizeBuffers(2,
                        event.window.data1, event.window.data2,
                        DXGI_FORMAT_R8G8B8A8_UNORM,
                        DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
                    );
                    pImpl->rtv=createRTV(
                        pImpl->device, pImpl->swapChain
                    );
                    break;
                }
            }
            break;
        case SDL_MOUSEMOTION:
            // print("Mouse Button Move");
            break;
        case SDL_MOUSEBUTTONDOWN:
		    // print("Mouse Button Down");
            break;
        case SDL_MOUSEBUTTONUP:
		    // print("Mouse Button Up");
            break;
        case SDL_KEYDOWN:
            // print("Keyboard Button Down");
            break;
        }
    }
    else if(SDL_GetWindowFlags(pImpl->window)&SDL_WINDOW_MINIMIZED){
        SDL_Delay(10);
    }
	else{
		// Start the Dear ImGui frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Scene Control");
        ImGui::Text("%.2f FPS", ImGui::GetIO().Framerate);

        const float dt=ImGui::GetIO().DeltaTime;
        updateGUI();
        update(dt);
        shader->update(dt, pImpl->context, camera, xSplit);

		ImGui::End();

        // Rendering
		ImGui::Render();
        
        pImpl->setupRender();
        texturer->render(pImpl->context);
        shader->render(pImpl->context);

        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		// Present with vsync
		pImpl->swapChain->Present(1, 0);
		// Present without vsync
        // pImpl->swapChain->Present(0, 0);
	}

    return true;
}

RenderAdaptor::Impl::Impl(const WindowDesc& wd){
    // SDL_SetHint(SDL_HINT_RENDER_DRIVER, "direct3d11");
    throwIfTrue((window=createWindow(wd))==nullptr,
        "SDL_CreateWindow() Failed."
    );
    initDirect3D(wd);
    initGUI(wd);
}

void RenderAdaptor::Impl::initDirect3D(const WindowDesc& wd){
    tie(device, context)=createDevice();

    HWND hwnd=getHandle(window);
    const auto qualityLevel=getQualityLevel(device);
    swapChain=createSwapChain(hwnd, device, wd.size, qualityLevel);

    rtv=createRTV(device, swapChain);
    screenViewport=setScreenViewPort(wd.size, context);

    depthStencilBuffer=createDepthStencilBuffer(wd.size, qualityLevel, device);
    DX_throwIf(device->CreateDepthStencilView(depthStencilBuffer.Get(),
        0, &dsv
    ));

    dss=createDSS(device);
}

void RenderAdaptor::Impl::initGUI(const WindowDesc& wd){
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io=ImGui::GetIO(); (void)io;
    io.DisplaySize=ImVec2(wd.size.x, wd.size.y);
    // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup Dear ImGui style
    // ImGui::StyleColorsDark();
    ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    throwIfTrue(!ImGui_ImplSDL2_InitForD3D(window),
        "ImGui_SDL2 Init Failed."
    );
    throwIfTrue(!ImGui_ImplDX11_Init(device.Get(), context.Get()),
        "ImGui_DX11 Init Failed."
    );
}

void RenderAdaptor::Impl::cleanupDeviceD3D(){
    cleanupRenderTarget();

    if(swapChain){
        swapChain->Release();
        swapChain=nullptr;
    }
    if(context){
        context->Release();
        context=nullptr;
    }
    if(device){
        device->Release();
        device=nullptr;
    }
}

void RenderAdaptor::Impl::cleanupRenderTarget(){
    if(rtv){
        rtv->Release();
        rtv=nullptr;
    }
}

RenderAdaptor::Impl::~Impl(){
    ImGui_ImplDX11_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

    SDL_DestroyWindow(window);
    SDL_Quit();
}

void RenderAdaptor::Impl::setupRender(){
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

[[nodiscard]] static tuple<ComPtr<ID3D11VertexShader>,
    ComPtr<ID3D11InputLayout>
> createVSAndIL(
    const wstring& fileName,
    const ComPtr<ID3D11Device>& device,
    std::span<const D3D11_INPUT_ELEMENT_DESC> iedesc
){
    ComPtr<ID3DBlob> shaderBlob;
    ComPtr<ID3DBlob> errorBlob;

    HRESULT hr=D3DCompileFromFile(fileName.c_str(),
        nullptr, nullptr, "main", "vs_5_0", COMPILE_FLAGS, 0,
        &shaderBlob, &errorBlob
    );
    SC_throwIf(hr, errorBlob.Get());

    ComPtr<ID3D11VertexShader> vertexShader;
    ComPtr<ID3D11InputLayout> inputLayout;
    DX_throwIf(device->CreateVertexShader(
        shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(),
        nullptr,
        &vertexShader
    ));
    DX_throwIf(device->CreateInputLayout(
        iedesc.data(), static_cast<UINT>(iedesc.size()),
        shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(),
        &inputLayout
    ));

    return {vertexShader, inputLayout};
}

[[nodiscard]] static ComPtr<ID3D11PixelShader> createPS(
    const wstring& fileName,
    const ComPtr<ID3D11Device>& device
){
    ComPtr<ID3DBlob> shaderBlob;
    ComPtr<ID3DBlob> errorBlob;

    HRESULT hr=D3DCompileFromFile(fileName.c_str(),
        nullptr, nullptr, "main", "ps_5_0", COMPILE_FLAGS, 0,
        &shaderBlob, &errorBlob
    );
    SC_throwIf(hr, errorBlob.Get());

    ComPtr<ID3D11PixelShader> pixelShader;
    DX_throwIf(device->CreatePixelShader(
        shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(),
        nullptr,
        &pixelShader
    ));

    return pixelShader;
}

[[nodiscard]] static ComPtr<ID3D11RasterizerState> createRS(
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
[[nodiscard]] static ComPtr<ID3D11Buffer> createVB(
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

[[nodiscard]] static ComPtr<ID3D11Buffer> createIB(
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
[[nodiscard]] static ComPtr<ID3D11Buffer> createCB(
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

RenderAdaptor::ShaderAdaptor::ShaderAdaptor(
    const ComPtr<ID3D11Device>& device,
    const wstring& vsFileName, const wstring& psFileName)
: rs(createRS(device)){
    MeshBuffer meshBuffer;
    auto [vertices, indices]=meshBuffer.extract();

    vertexBuffer=createVB<Vertex>(vertices, device);
    indexCount=static_cast<UINT>(indices.size());
    indexBuffer=createIB(indices, device);
    vscBuffer=createCB(vsc, device);
    pscBuffer=createCB(psc, device);

    // create shader
    constexpr D3D11_INPUT_ELEMENT_DESC inputElements[]={
        {
            .SemanticName="POSITION",
            .SemanticIndex=0,
            .Format=DXGI_FORMAT_R32G32B32_FLOAT,
            .InputSlot=0,
            .AlignedByteOffset=0,
            .InputSlotClass=D3D11_INPUT_PER_VERTEX_DATA,
            .InstanceDataStepRate=0
        }, {
            .SemanticName="NORMAL",
            .SemanticIndex=0,
            .Format=DXGI_FORMAT_R32G32B32_FLOAT,
            .InputSlot=0,
            .AlignedByteOffset=3*sizeof(float),
            .InputSlotClass=D3D11_INPUT_PER_VERTEX_DATA,
            .InstanceDataStepRate=0
        }, {
            .SemanticName="COLOR",
            .SemanticIndex=0,
            .Format=DXGI_FORMAT_R32G32B32A32_FLOAT,
            .InputSlot=0,
            .AlignedByteOffset=6*sizeof(float),
            .InputSlotClass=D3D11_INPUT_PER_VERTEX_DATA,
            .InstanceDataStepRate=0
        }, {
            .SemanticName="TEXCOORD",
            .SemanticIndex=0,
            .Format=DXGI_FORMAT_R32G32_FLOAT,
            .InputSlot=0,
            .AlignedByteOffset=10*sizeof(float),
            .InputSlotClass=D3D11_INPUT_PER_VERTEX_DATA,
            .InstanceDataStepRate=0
        }
    };
    tie(vs, il)=createVSAndIL(vsFileName, device, inputElements);
    ps=createPS(psFileName, device);
}

void RenderAdaptor::ShaderAdaptor::render(
    const ComPtr<ID3D11DeviceContext>& context
){
    // IA: Input-Assembler stage
    // VS: Vertex Shader
    // PS: Pixel Shader
    // RS: Rasterizer stage
    // OM: Output-Merger stage

    // set the shader objects
    context->VSSetConstantBuffers(0, 1, vscBuffer.GetAddressOf());
    context->VSSetShader(vs.Get(), 0, 0);
    context->PSSetConstantBuffers(0, 1, pscBuffer.GetAddressOf());
    context->PSSetShader(ps.Get(), 0, 0);
    context->RSSetState(rs.Get());

    // select which vertex buffer to display
    UINT stride=sizeof(Vertex);
    UINT offset=0;
    context->IASetInputLayout(il.Get());
    context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
    context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

    context->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
    );
    context->DrawIndexed(indexCount, 0, 0);
}

void RenderAdaptor::ShaderAdaptor::update(float dt,
    const ComPtr<ID3D11DeviceContext>& context,
    const Camera& camera, const float xSplit
){
    static float rot=0.0f;
    rot+=dt;

    const auto translation=Matrix::CreateTranslation(0.0f, -0.3f, 1.0f);
    const auto rotation=Matrix::CreateRotationY(rot);
    const auto scaling=Matrix::CreateScale(0.5f);
    vsc.model=scaling*rotation*translation;

    // constexpr float fov=XMConvertToRadians(70.0f);
    // camera.setPerspective(fov);

    vsc.model=vsc.model.Transpose();
    vsc.view=camera.view().Transpose();
    vsc.projection=camera.projection().Transpose();

    updateBuffer(vscBuffer, vsc, context);

    psc.xSplit=xSplit;

    updateBuffer(pscBuffer, psc, context);
}

[[nodiscard]] static ComPtr<ID3D11SamplerState> createSS(
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

RenderAdaptor::TextureAdaptor::TextureAdaptor(
    const ComPtr<ID3D11Device>& device,
    const string& fileName)
: sampler(createSS(device)){
    // load texture from file
    int width, height, chnum;
    uint8_t* img=stbi_load(fileName.c_str(),
        &width, &height,
        &chnum, 4
    );
    assert(chnum==4);

    const auto size=chnum*width*height;
    auto image=new uint8_t[size];
    memcpy(image, img, size);

    // create texture
    D3D11_TEXTURE2D_DESC td{};
    td.Width=width;
    td.Height=height;
    td.MipLevels=td.ArraySize=1;
    td.Format=DXGI_FORMAT_R8G8B8A8_UNORM;
    td.SampleDesc.Count=1;
    td.Usage=D3D11_USAGE_IMMUTABLE;
    td.BindFlags=D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA textureData{
        .pSysMem=image,
        .SysMemPitch=narrow_cast<UINT>(chnum*width),
        .SysMemSlicePitch=0
    };

    DX_throwIf(device->CreateTexture2D(&td, &textureData,
        texture.GetAddressOf()
    ));
    DX_throwIf(device->CreateShaderResourceView(texture.Get(), nullptr,
        textureView.GetAddressOf()
    ));

    delete[] image;
}

void RenderAdaptor::TextureAdaptor::render(
    const ComPtr<ID3D11DeviceContext>& context
){
    ID3D11ShaderResourceView* resources[]={
        textureView.Get()
    };

    context->PSSetShaderResources(
        0, sizeof(resources)/sizeof(ID3D11ShaderResourceView*),
        resources
    );
    context->PSSetSamplers(0, 1, sampler.GetAddressOf());
}
