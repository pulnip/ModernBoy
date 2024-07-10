#pragma once
#define USE_RAYTRACER

#include <vector>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <glm/glm.hpp>
#include <SDL2/SDL_video.h>
#include "Info.hpp"

#ifndef USE_RAYTRACER
#include "Rasterizer.hpp"
#else
#include "Raytracer.hpp"
#endif

namespace ModernBoy{
    LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    class Window{
        SDL_Window* window;

        ID3D11Device* device;
        ID3D11DeviceContext* deviceContext;
        IDXGISwapChain *swapChain;
        D3D11_VIEWPORT viewport;
        ID3D11RenderTargetView *renderTargetView;
        ID3D11VertexShader *vertexShader;
        ID3D11PixelShader *pixelShader;
        ID3D11InputLayout *layout;

        ID3D11Buffer *vertexBuffer = nullptr;
        ID3D11Buffer *indexBuffer = nullptr;
        ID3D11Texture2D *canvasTexture = nullptr;
        ID3D11ShaderResourceView *canvasTextureView = nullptr;
        ID3D11RenderTargetView *canvasRenderTargetView = nullptr;
        ID3D11SamplerState *colorSampler;
        UINT indexCount;

#ifndef USE_RAYTRACER
        Rasterizer rasterizer;
#else
        Raytracer raytracer;
#endif
        std::vector<fRGBA> pixels;

      public:
        Window(const WindowInfo& wi);
        void initShader();
        ~Window();

        bool update();
        void render();
    };
}
