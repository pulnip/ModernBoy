#pragma once

#include <memory>
#include <SDL2/SDL_events.h>
#include "Attributes.hpp"
#include "Info.hpp"

namespace ModernBoy{
    class RenderAdaptor{
      public:
        ComPtr<Device> device;
        ComPtr<Context> context;

      private:
        ComPtr<ID3D11RenderTargetView> rtv;
      public:
        ComPtr<IDXGISwapChain> swapChain;

      private:
        // depth stencil buffer
        ComPtr<ID3D11Texture2D> depthStencilBuffer;
        ComPtr<ID3D11DepthStencilView> dsv;
        ComPtr<ID3D11DepthStencilState> dss;

        D3D11_VIEWPORT screenViewport{};

        UINT qualityLevel;

      public:
        RenderAdaptor(HWND hwnd, const ipoint2& screenSize);
        ~RenderAdaptor();

        void setup();
        void swap();
        void recreateRenderTarget(int width, int height);
    };
}
