#pragma once

#include <vector>
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <glm/glm.hpp>
#include "Info.hpp"
#include "Raytracer.hpp"

namespace ModernBoy{
    LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    class Window{
      private:
        WNDCLASS wc;
        RECT wr;
        HWND hwnd;

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

        Raytracer raytracer;
        std::vector<fRGBA> pixels;

      public:
        Window(const WindowInfo& wi);
        void initShader();
        ~Window();

        bool update();
        void render();
    };
}
