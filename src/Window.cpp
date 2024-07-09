#include <iostream>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "InputSystem.hpp"
#include "Window.hpp"
#include "Vertex.hpp"

using namespace ModernBoy;

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI ModernBoy::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam){
    if(ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

    switch (msg){
	case WM_SIZE:
		// Reset and resize swapchain
		return 0;
	case WM_SYSCOMMAND:
		if((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_MOUSEMOVE:
		// std::cout << "Mouse " << LOWORD(lParam) << " " << HIWORD(lParam) << std::endl;
		break;
	case WM_LBUTTONUP:
		// std::cout << "WM_LBUTTONUP Left mouse button" << std::endl;
		break;
	case WM_RBUTTONUP:
		// std::cout << "WM_RBUTTONUP Right mouse button" << std::endl;
		break;
	case WM_KEYDOWN:
		// std::cout << "WM_KEYDOWN " << (int)wParam << std::endl;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}

	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

Window::Window(const WindowInfo& wi)
:wc{
    CS_CLASSDC,
    WndProc,
    0,
    0,
    GetModuleHandle(nullptr),
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    wi.title.c_str()
}, wr{
    0L,
    0L,
    wi.size.x,
    wi.size.y
}, raytracer(
    wi.resolution
){
    RegisterClass(&wc);

    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

    hwnd = CreateWindow(
		wc.lpszClassName,
		wi.title.c_str(),
		WS_OVERLAPPEDWINDOW,
        wi.position.x, wi.position.y,
		wi.size.x, wi.size.y,
		nullptr,
		nullptr,
		wc.hInstance,
		nullptr
    );

    ShowWindow(hwnd, SW_SHOWDEFAULT);
	UpdateWindow(hwnd);

    SetForegroundWindow(hwnd);

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
    // set the back buffer width
    swapChainDesc.BufferDesc.Width = wi.size.x;
    // set the back buffer height
    swapChainDesc.BufferDesc.Height = wi.size.y;
    // use 32-bit color
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; 
    // one back buffer
    swapChainDesc.BufferCount = 2;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    // how swap chain is to be used
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; 
    // the window to be used
    swapChainDesc.OutputWindow = hwnd; 
    // how many multisamples
    swapChainDesc.SampleDesc.Count = 1;
    // windowed/full-screen mode
    swapChainDesc.Windowed = TRUE;
    // allow full-screen switching
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; 
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    const D3D_FEATURE_LEVEL featureLevelArray[1] = {D3D_FEATURE_LEVEL_11_0};
    if(FAILED(D3D11CreateDeviceAndSwapChain(
            NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags,
            featureLevelArray, 1, D3D11_SDK_VERSION, &swapChainDesc, &swapChain,
            &device, NULL, &deviceContext))) {
        std::cout << "D3D11CreateDeviceAndSwapChain() error" << std::endl;
    }

    // CreateRenderTarget
    ID3D11Texture2D *pBackBuffer;
    swapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    if(pBackBuffer){
        device->CreateRenderTargetView(pBackBuffer, NULL, &renderTargetView);
        pBackBuffer->Release();
    }
    else{
        std::cout << "CreateRenderTargetView() error" << std::endl;
        exit(-1);
    }

    // Set the viewport
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = wi.size.x;
    viewport.Height = wi.size.y;
    viewport.MinDepth = 0.0f;
    // Note: important for depth buffering
    viewport.MaxDepth = 1.0f;
    deviceContext->RSSetViewports(1, &viewport);

    initShader();

    // Create texture and rendertarget
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    // D3D11_FILTER_MIN_MAG_MIP_LINEAR
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    // Create the Sample State
    device->CreateSamplerState(&sampDesc, &colorSampler);

    D3D11_TEXTURE2D_DESC textureDesc;
    ZeroMemory(&textureDesc, sizeof(textureDesc));
    textureDesc.MipLevels = textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DYNAMIC;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.MiscFlags = 0;
    textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    textureDesc.Width = raytracer.resolution.x;
    textureDesc.Height = raytracer.resolution.y;

    device->CreateTexture2D(&textureDesc, nullptr, &canvasTexture);

    if(canvasTexture){
        device->CreateShaderResourceView(
            canvasTexture, nullptr, &canvasTextureView
        );

        D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
        renderTargetViewDesc.Format = textureDesc.Format;
        renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        renderTargetViewDesc.Texture2D.MipSlice = 0;

        device->CreateRenderTargetView(
            canvasTexture, &renderTargetViewDesc,
            &canvasRenderTargetView
        );
    }
    else{
        std::cout << "CreateRenderTargetView() error" << std::endl;
    }

    // Create a vertex buffer
    {
        const std::vector<Vertex> vertices = {
            {
                {-1.0f, -1.0f, 0.0f, 1.0f},
                {0.f, 1.f},
            },
            {
                {1.0f, -1.0f, 0.0f, 1.0f},
                {1.f, 1.f},
            },
            {
                {1.0f, 1.0f, 0.0f, 1.0f},
                {1.f, 0.f},
            },
            {
                {-1.0f, 1.0f, 0.0f, 1.0f},
                {0.f, 0.f},
            },
        };

        D3D11_BUFFER_DESC bufferDesc;
        ZeroMemory(&bufferDesc, sizeof(bufferDesc));
        // write access access by CPU and GPU
        bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        // size is the VERTEX struct * 3
        bufferDesc.ByteWidth = UINT(sizeof(Vertex) * vertices.size());
        // use as a vertex buffer
        bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        // allow CPU to write in buffer
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        bufferDesc.StructureByteStride = sizeof(Vertex);

        D3D11_SUBRESOURCE_DATA vertexBufferData = {0, };
        vertexBufferData.pSysMem = vertices.data();
        vertexBufferData.SysMemPitch = 0;
        vertexBufferData.SysMemSlicePitch = 0;

        const HRESULT hr =
            device->CreateBuffer(&bufferDesc, &vertexBufferData, &vertexBuffer);
        if(FAILED(hr)) {
            std::cout << "CreateBuffer() failed. " << std::hex << hr
                      << std::endl;
        };
    }

    // Create an index buffer
    {
        const std::vector<uint16_t> indices = {
            3, 1, 0, 2, 1, 3,
        };

        indexCount = UINT(indices.size());

        D3D11_BUFFER_DESC bufferDesc = {};
        // write access access by CPU and GPU
        bufferDesc.Usage = D3D11_USAGE_DYNAMIC; 
        bufferDesc.ByteWidth = UINT(sizeof(uint16_t) * indices.size());
        // use as a vertex buffer
        bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER; 
        // allow CPU to write in buffer
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; 
        bufferDesc.StructureByteStride = sizeof(uint16_t);

        D3D11_SUBRESOURCE_DATA indexBufferData = {0};
        indexBufferData.pSysMem = indices.data();
        indexBufferData.SysMemPitch = 0;
        indexBufferData.SysMemSlicePitch = 0;

        device->CreateBuffer(&bufferDesc, &indexBufferData, &indexBuffer);
    }

    pixels.reserve(raytracer.resolution.x * raytracer.resolution.y);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.DisplaySize = ImVec2(wi.size.x, wi.size.y);
    ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
    ImGui_ImplDX11_Init(device, deviceContext);
    ImGui_ImplWin32_Init(hwnd);
}

void Window::initShader(){
    // 참고: 쉐이더 컴파일 방법 MS 공식 문서 링크
    // https://docs.microsoft.com/en-us/windows/win32/direct3d11/how-to--compile-a-shader

    ID3DBlob *vertexBlob = nullptr;
    ID3DBlob *pixelBlob = nullptr;
    ID3DBlob *errorBlob = nullptr;

    HRESULT hr=D3DCompileFromFile(
        L"src/VS.hlsl", nullptr, nullptr, "main", "vs_5_0", 0, 0,
        &vertexBlob, &errorBlob
    );
    if(FAILED(hr)) {
        if((hr & D3D11_ERROR_FILE_NOT_FOUND) != 0) {
            std::cout << "File not found." << std::endl;
        }

        if(errorBlob) {
            std::cout << "Vertex shader compile error\n"
                << (char*)errorBlob->GetBufferPointer() << std::endl;
        }

        exit(-1);
    }

    hr=D3DCompileFromFile(
        L"src/PS.hlsl", 0, 0, "main", "ps_5_0", 0, 0,
        &pixelBlob, &errorBlob
    );
    if(FAILED(hr)) {
        if((hr & D3D11_ERROR_FILE_NOT_FOUND) != 0) {
            std::cout << "File not found." << std::endl;
        }

        if(errorBlob) {
            std::cout << "Pixel shader compile error\n"
                << (char *)errorBlob->GetBufferPointer() << std::endl;
        }

        exit(-1);
    }

    device->CreateVertexShader(
        vertexBlob->GetBufferPointer(),
        vertexBlob->GetBufferSize(), NULL,
        &vertexShader
    );
    device->CreatePixelShader(
        pixelBlob->GetBufferPointer(),
        pixelBlob->GetBufferSize(), NULL,
        &pixelShader
    );

    // Create the input layout object
    D3D11_INPUT_ELEMENT_DESC ied[]={
        {
            "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,
            D3D11_INPUT_PER_VERTEX_DATA, 0
        },
        {
            "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16,
            D3D11_INPUT_PER_VERTEX_DATA, 0
        },
    };

    device->CreateInputLayout(
        ied, 2, vertexBlob->GetBufferPointer(),
        vertexBlob->GetBufferSize(), &layout
    );
    deviceContext->IASetInputLayout(layout);
}

Window::~Window(){
    ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

    if(layout) {
        layout->Release();
        layout = NULL;
    }
    if(vertexShader) {
        vertexShader->Release();
        vertexShader = NULL;
    }
    if(pixelShader) {
        pixelShader->Release();
        pixelShader = NULL;
    }
    if(vertexBuffer) {
        vertexBuffer->Release();
        vertexBuffer = NULL;
    }
    if(indexBuffer) {
        indexBuffer->Release();
        indexBuffer = NULL;
    }
    if(canvasTexture) {
        canvasTexture->Release();
        canvasTexture = NULL;
    }
    if(canvasTextureView) {
        canvasTextureView->Release();
        canvasTextureView = NULL;
    }
    if(canvasRenderTargetView) {
        canvasRenderTargetView->Release();
        canvasRenderTargetView = NULL;
    }
    if(colorSampler) {
        colorSampler->Release();
        colorSampler = NULL;
    }
    if(renderTargetView) {
        renderTargetView->Release();
        renderTargetView = NULL;
    }
    if(swapChain) {
        swapChain->Release();
        swapChain = NULL;
    }
    if(deviceContext) {
        deviceContext->Release();
        deviceContext = NULL;
    }
    if(device) {
        device->Release();
        device = NULL;
    }

    DestroyWindow(hwnd);
	UnregisterClass(wc.lpszClassName, wc.hInstance);
}

bool Window::update(){
    MSG msg;
    if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
        
        if(WM_QUIT==msg.message){
            return false;
        }
	}
	else{
		// Start the Dear ImGui frame
		// ImGui_ImplDX11_NewFrame();
		// ImGui_ImplWin32_NewFrame();
		// ImGui::NewFrame();
		// ImGui::Begin("Scene Control");
		// ImGui::End();
		// ImGui::Render();

        const auto size = raytracer.resolution.x * raytracer.resolution.y;
        pixels.resize(size, toRGBA(fBLACK));

        raytracer.render(pixels);

        D3D11_MAPPED_SUBRESOURCE ms;
        deviceContext->Map(
            canvasTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms
        );
        memcpy(ms.pData, pixels.data(),
            pixels.size() * sizeof(fRGBA)
        );
        deviceContext->Unmap(canvasTexture, 0);

        render();

		// ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        
		// switch the back buffer and the front buffer
		swapChain->Present(1, 0);
	}

    return true;
}

void Window::render(){
    float clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    deviceContext->RSSetViewports(1, &viewport);
    deviceContext->OMSetRenderTargets(1, &renderTargetView, nullptr);
    deviceContext->ClearRenderTargetView(renderTargetView, clearColor);

    // set the shader objects
    deviceContext->VSSetShader(vertexShader, 0, 0);
    deviceContext->PSSetShader(pixelShader, 0, 0);

    // select which vertex buffer to display
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);

    // https://github.com/Microsoft/DirectXTK/wiki/Getting-Started
    // https://github.com/microsoft/Xbox-ATG-Samples/tree/main/PCSamples/IntroGraphics/SimpleTexturePC
    deviceContext->PSSetSamplers(0, 1, &colorSampler); // TODO: samplers to array
    deviceContext->PSSetShaderResources(0, 1, &canvasTextureView);
    deviceContext->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
    );
    deviceContext->DrawIndexed(indexCount, 0, 0);
}
