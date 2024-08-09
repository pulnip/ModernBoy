#include "RenderAdaptor.inl"
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_dx11.h>
#include <stb_image.h>
#include "MeshBuffer.hpp"
#include "RenderAdaptor.hpp"

using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace ModernBoy;

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

    Impl(const WindowDesc& wd){
        // SDL_SetHint(SDL_HINT_RENDER_DRIVER, "direct3d11");
        throwIfTrue((window=createWindow(wd))==nullptr,
            "SDL_CreateWindow() Failed."
        );
        initDirect3D(wd);
        initGUI(wd);
    }
    ~Impl(){
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void setupRender(){
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

    void cleanupDeviceD3D(){
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
    void cleanupRenderTarget(){
        if(rtv){
            rtv->Release();
            rtv=nullptr;
        }
    }

  private:
    void initDirect3D(const WindowDesc& wd){
        tie(device, context)=createDevice();

        HWND hwnd=getHandle(window);
        const auto qualityLevel=getQualityLevel(device);
        swapChain=createSwapChain(hwnd, device, wd.size, qualityLevel);

        rtv=createRTV(device, swapChain);
        screenViewport=setScreenViewPort(wd.size, context);

        depthStencilBuffer=createDepthStencilBuffer(
            wd.size, qualityLevel, device
        );
        DX_throwIf(device->CreateDepthStencilView(
            depthStencilBuffer.Get(), 0, &dsv
        ));

        dss=createDSS(device);
    }
    void initGUI(const WindowDesc& wd){
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

    ShaderAdaptor(
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

    void update(float dt,
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
    void render(const ComPtr<ID3D11DeviceContext>& context){
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
        context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(),
            &stride, &offset
        );
        context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

        context->IASetPrimitiveTopology(
            D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
        );
        context->DrawIndexed(indexCount, 0, 0);
    }
};

struct RenderAdaptor::TextureAdaptor final{
    ComPtr<ID3D11Texture2D> texture;
    ComPtr<ID3D11ShaderResourceView> textureView;
    ComPtr<ID3D11SamplerState> sampler;

    TextureAdaptor(const ComPtr<ID3D11Device>& device,
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
            .SysMemPitch=static_cast<UINT>(chnum*width),
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

    void render(const ComPtr<ID3D11DeviceContext>& context){
        ID3D11ShaderResourceView* resources[]={
            textureView.Get()
        };

        context->PSSetShaderResources(
            0, sizeof(resources)/sizeof(ID3D11ShaderResourceView*),
            resources
        );
        context->PSSetSamplers(0, 1, sampler.GetAddressOf());
    }
};

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
                    camera.setScreenSize({
                        event.window.data1,
                        event.window.data2
                    });
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
