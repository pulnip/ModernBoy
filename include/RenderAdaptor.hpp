#pragma once

#include <memory>
#include <wrl/client.h>
#include <d3d11.h>
#include <SDL2/SDL_events.h>
#include "Attributes.hpp"
#include "Info.hpp"

namespace ModernBoy{
    class RenderAdaptor{
        template<typename T> using ComPtr=Microsoft::WRL::ComPtr<T>;
        using Device=ID3D11Device;
        using Context=ID3D11DeviceContext;

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
