#include <span>
#include <tuple>
#include <vector>
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
#include "Camera.hpp"
#include "helper.hpp"
#include "MeshBuffer.hpp"
#include "Vertex.hpp"
#include "Window.hpp"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace ModernBoy;
using Microsoft::WRL::ComPtr;

#if defined(DEBUG) || defined(_DEBUG)
static constexpr bool IS_DEBUG=true;
#else
static constexpr bool IS_DEBUG=false;
#endif

struct RenderAdaptor::Impl final{
    SDL_Window* window=nullptr;

    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;
    ComPtr<ID3D11RenderTargetView> renderTargetView;
    ComPtr<IDXGISwapChain> swapChain;

    // depth buffer
    ComPtr<ID3D11Texture2D> depthStencilBuffer;
    ComPtr<ID3D11DepthStencilView> depthStencilView;
    ComPtr<ID3D11DepthStencilState> depthStencilState;

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
    ComPtr<ID3D11Buffer> constantBuffer;

    UINT indexCount=0;

    Constants constants;

    Camera camera;

    ShaderAdaptor(const ComPtr<ID3D11Device>& device,
        const wstring& vsFileName, const wstring& psFileName,
        const float ASPECT_RATIO
    );

    void update(float dt, const ComPtr<ID3D11DeviceContext>& context);
    void render(const ComPtr<ID3D11DeviceContext>& context);

  private:
    void createVSAndIL(const std::wstring& fileName,
        const ComPtr<ID3D11Device>& device,
        const std::span<D3D11_INPUT_ELEMENT_DESC>& iedesc
    );
    void createPS(const std::wstring& fileName,
        const ComPtr<ID3D11Device>& device
    );
    void createIB(const std::vector<uint16_t>& indices,
        const ComPtr<ID3D11Device>& device
    );

    template<typename V>
    void createVB(const std::vector<V>& vertices,
        const ComPtr<ID3D11Device>& device
    ){
        D3D11_BUFFER_DESC bufferDesc;
        ZeroMemory(&bufferDesc, sizeof(bufferDesc));
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
            vertices.data(),
            0,
            0
        };

        DX_throwIf(device->CreateBuffer(&bufferDesc,
            &vertexBufferData,
            vertexBuffer.GetAddressOf()
        ));
    }

    template <typename T>
    void createCB(const T& constantBufferData,
        const ComPtr<ID3D11Device>& device
    ){
        D3D11_BUFFER_DESC cbDesc;
        cbDesc.ByteWidth=sizeof(constantBufferData);
        cbDesc.Usage=D3D11_USAGE_DYNAMIC;
        cbDesc.BindFlags=D3D11_BIND_CONSTANT_BUFFER;
        cbDesc.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
        cbDesc.MiscFlags=0;
        cbDesc.StructureByteStride=0;

        // Fill in the subresource data.
        D3D11_SUBRESOURCE_DATA InitData;
        InitData.pSysMem=&constantBufferData;
        InitData.SysMemPitch=0;
        InitData.SysMemSlicePitch=0;

        DX_throwIf(device->CreateBuffer(&cbDesc,
            &InitData,
            constantBuffer.GetAddressOf()
        ));
    }
};

RenderAdaptor::RenderAdaptor(const WindowDesc& wd)
: pImpl(std::make_unique<Impl>(wd))
, shader(std::make_unique<ShaderAdaptor>(pImpl->device,
    L"src/CVS.hlsl", L"src/CPS.hlsl",
    static_cast<float>(wd.size.x) / wd.size.y))
{}
RenderAdaptor::~RenderAdaptor()=default;

inline HWND getHandle(SDL_Window* window){
    SDL_SysWMinfo wmInfo; SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(window, &wmInfo);

    return wmInfo.info.win.window;
}

inline std::string getRendererName(SDL_Renderer* renderer){
    SDL_RendererInfo info;
    SDL_GetRendererInfo(renderer, &info);

    return info.name;
}

inline SDL_Window* createWindow(const WindowDesc& wd){
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

inline tuple<ComPtr<ID3D11Device>, ComPtr<ID3D11DeviceContext>> createDevice(){
    // constexpr auto DRIVER_TYPE=D3D_DRIVER_TYPE_WARP;
    constexpr auto DRIVER_TYPE=D3D_DRIVER_TYPE_HARDWARE;
    constexpr UINT CREATE_DEVICE_FLAGS=IS_DEBUG ? D3D11_CREATE_DEVICE_DEBUG : 0;
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

inline UINT getQualityLevel(const ComPtr<ID3D11Device>& device){
    UINT numQualityLevels;
    device->CheckMultisampleQualityLevels(
        DXGI_FORMAT_R8G8B8A8_UNORM, 4,
        &numQualityLevels
    );
    printIf(numQualityLevels <= 0, "MSAA not supported.");

    return numQualityLevels;
}

inline ComPtr<IDXGISwapChain> createSwapChain(HWND hwnd,
    const ComPtr<ID3D11Device>& device,
    const ipoint2& screenSize, UINT numQualityLevels
){
    const bool MSAA_4X=numQualityLevels > 0;

    DXGI_SWAP_CHAIN_DESC sc_desc;
    ZeroMemory(&sc_desc, sizeof(sc_desc));
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

inline ComPtr<ID3D11RenderTargetView> createRenderTargetView(
    const ComPtr<ID3D11Device>& device, const ComPtr<IDXGISwapChain>& swapChain
){
    ID3D11Texture2D* pBackBuffer;
    swapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    throwIfTrue(pBackBuffer==nullptr, "CreateRenderTargetView() error");

    ComPtr<ID3D11RenderTargetView> rtv;
    device->CreateRenderTargetView(pBackBuffer, nullptr, &rtv);
    pBackBuffer->Release();

    return rtv;
}

RenderAdaptor::Impl::Impl(const WindowDesc& wd){
    // SDL_SetHint(SDL_HINT_RENDER_DRIVER, "direct3d11");
    throwIfTrue((window=createWindow(wd))==nullptr,
        "SDL_CreateWindow() Failed."
    );
    initDirect3D(wd);
    initGUI(wd);
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
    context->OMSetRenderTargets(1, renderTargetView.GetAddressOf(),
        depthStencilView.Get()
    );
    context->ClearRenderTargetView(renderTargetView.Get(), clearColor);
    context->OMSetDepthStencilState(depthStencilState.Get(), 0);
    context->ClearDepthStencilView(depthStencilView.Get(),
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0
    );
}

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
                    pImpl->cleanupRenderTarget();
                    pImpl->swapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
                    pImpl->renderTargetView=createRenderTargetView(
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
        shader->update(dt, pImpl->context);

		ImGui::End();

        // Rendering
		ImGui::Render();
        
        pImpl->setupRender();
        shader->render(pImpl->context);
		
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        
		// Present with vsync
		pImpl->swapChain->Present(1, 0);
		// Present without vsync
        // pImpl->swapChain->Present(0, 0);
	}

    return true;
}

inline void setScreenViewPort(D3D11_VIEWPORT& screenViewport,
    const ComPtr<ID3D11DeviceContext>& context, const ipoint2& screenSize
){
    ZeroMemory(&screenViewport, sizeof(D3D11_VIEWPORT));
    screenViewport.TopLeftX=0;
    screenViewport.TopLeftY=0;
    screenViewport.Width=screenSize.x;
    screenViewport.Height=screenSize.y;
    screenViewport.MinDepth=0.0f;
    // Note: important for depth buffering
    screenViewport.MaxDepth=1.0f;
    context->RSSetViewports(1, &screenViewport);
}

inline ComPtr<ID3D11Texture2D> createDepthStencilBuffer(
    const ComPtr<ID3D11Device>& device, const ipoint2& screenSize,
    UINT numQualityLevels
){
    const bool MSAA_4X=numQualityLevels > 0;
    ComPtr<ID3D11Texture2D> dsb;

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

    DX_throwIf(device->CreateTexture2D(&dsb_desc, nullptr, dsb.GetAddressOf()));

    return dsb;
}

inline ComPtr<ID3D11DepthStencilState> createDepthStencilState(
    const ComPtr<ID3D11Device>& device
){
    ComPtr<ID3D11DepthStencilState> dss;

    D3D11_DEPTH_STENCIL_DESC dsd;
    ZeroMemory(&dsd, sizeof(D3D11_DEPTH_STENCIL_DESC));
    dsd.DepthEnable=true;
    dsd.DepthWriteMask=D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
    dsd.DepthFunc=D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
    DX_throwIf(device->CreateDepthStencilState(&dsd,
        dss.GetAddressOf())
    );

    return dss;
}

void RenderAdaptor::Impl::initDirect3D(const WindowDesc& wd){
    tie(device, context)=createDevice();

    HWND hwnd=getHandle(window);
    const auto qualityLevel=getQualityLevel(device);
    swapChain=createSwapChain(hwnd, device, wd.size, qualityLevel);

    renderTargetView=createRenderTargetView(device, swapChain);
    setScreenViewPort(screenViewport, context, wd.size);

    depthStencilBuffer=createDepthStencilBuffer(device, wd.size, qualityLevel);
    DX_throwIf(device->CreateDepthStencilView(depthStencilBuffer.Get(),
        0, &depthStencilView
    ));

    depthStencilState=createDepthStencilState(device);
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
    if(renderTargetView){
        renderTargetView->Release();
        renderTargetView=nullptr;
    }
}

inline ComPtr<ID3D11RasterizerState> createRS(
    const ComPtr<ID3D11Device>& device
){
    D3D11_RASTERIZER_DESC rd;
    ZeroMemory(&rd, sizeof(D3D11_RASTERIZER_DESC));
    rd.FillMode=D3D11_FILL_MODE::D3D11_FILL_SOLID;
    // rd.FillMode=D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
    rd.CullMode=D3D11_CULL_MODE::D3D11_CULL_NONE;
    rd.FrontCounterClockwise=false;

    ComPtr<ID3D11RasterizerState> rs;
    DX_throwIf(device->CreateRasterizerState(&rd, &rs));

    return rs;
}

RenderAdaptor::ShaderAdaptor::ShaderAdaptor(
const ComPtr<ID3D11Device>& device,
    const wstring& vsFileName, const wstring& psFileName,
    const float ASPECT_RATIO)
: rs(createRS(device)){
    camera.setAspectRatio(ASPECT_RATIO);

    MeshBuffer meshBuffer;
    auto [vertices, indices]=meshBuffer.extract();

    createVB(vertices, device);
    indexCount=static_cast<UINT>(indices.size());
    createIB(indices, device);
    createCB(constants, device);

    // create shader
    D3D11_INPUT_ELEMENT_DESC inputElements[]={
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 3*sizeof(float), D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    createVSAndIL(vsFileName, device, inputElements);
    createPS(psFileName, device);
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
    context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
    context->VSSetShader(vs.Get(), 0, 0);
    context->PSSetShader(ps.Get(), 0, 0);
    context->RSSetState(rs.Get());

    // select which vertex buffer to display
    UINT stride=sizeof(ColorVertex);
    UINT offset=0;
    context->IASetInputLayout(il.Get());
    context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
    context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

    context->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
    );
    context->DrawIndexed(indexCount, 0, 0);
}

void RenderAdaptor::ShaderAdaptor::createVSAndIL(const wstring& fileName,
    const ComPtr<ID3D11Device>& device,
    const span<D3D11_INPUT_ELEMENT_DESC>& iedesc
){
    ComPtr<ID3DBlob> shaderBlob;
    ComPtr<ID3DBlob> errorBlob;

    HRESULT hr=D3DCompileFromFile(fileName.c_str(),
        nullptr, nullptr, "main", "vs_5_0", 0, 0,
        &shaderBlob, &errorBlob
    );
    SC_throwIf(hr, errorBlob.Get());

    device->CreateVertexShader(
        shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(),
        nullptr,
        &vs
    );

    device->CreateInputLayout(
        iedesc.data(), static_cast<UINT>(iedesc.size()),
        shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(),
        &il
    );
}

void RenderAdaptor::ShaderAdaptor::createPS(const wstring& fileName,
    const ComPtr<ID3D11Device>& device
){
    ComPtr<ID3DBlob> shaderBlob;
    ComPtr<ID3DBlob> errorBlob;

    HRESULT hr=D3DCompileFromFile(fileName.c_str(),
        nullptr, nullptr, "main", "ps_5_0", 0, 0,
        &shaderBlob, &errorBlob
    );
    SC_throwIf(hr, errorBlob.Get());

    device->CreatePixelShader(
        shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(),
        nullptr,
        &ps
    );
}

void RenderAdaptor::ShaderAdaptor::createIB(const vector<uint16_t>& indices,
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

    device->CreateBuffer(&bufferDesc,
        &indexBufferData,
        indexBuffer.GetAddressOf()
    );
}

template <typename T>
static void updateBuffer(ComPtr<ID3D11Buffer>& buffer,
    const T& bufferData,
    const ComPtr<ID3D11DeviceContext>& context
){
    D3D11_MAPPED_SUBRESOURCE ms;
    context->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
    memcpy(ms.pData, &bufferData, sizeof(bufferData));
    context->Unmap(buffer.Get(), 0);
}

void RenderAdaptor::ShaderAdaptor::update(float dt,
    const ComPtr<ID3D11DeviceContext>& context
){
    static float rot=0.0f;
    rot+=dt;

    const auto translation=Matrix::CreateTranslation(0.0f, -0.3f, 1.0f);
    const auto rotation=Matrix::CreateRotationY(rot);
    const auto scaling=Matrix::CreateScale(0.5f);
    constants.model=scaling*rotation*translation;

    camera.setEyePos({0.0f, 0.0f, -1.0f});
    camera.setEyeDir(Vector3::UnitZ);
    camera.setUpDir(Vector3::UnitY);

    // constexpr float fov=XMConvertToRadians(70.0f);
    // camera.setPerspective(fov);

    constants.model=constants.model.Transpose();
    constants.view=camera.view().Transpose();
    constants.projection=camera.projection().Transpose();

    updateBuffer(constantBuffer, constants, context);
}

void Window::updateGUI(){
    // ImGui::Checkbox("usePerspective", &shader->usePerspective);
    static float fov=70.0f;
    ImGui::SliderFloat("Field of View", &fov, 0.0f, 120.0f);

    shader->camera.setPerspective(XMConvertToRadians(fov));
}

void Window::update([[maybe_unused]] float dt){}
