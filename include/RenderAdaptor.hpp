#pragma once

#include <memory>
#include <windef.h>
#include <SDL2/SDL_events.h>
#include "Attributes.hpp"
#include "Info.hpp"

namespace ModernBoy{
    class RenderAdaptor{
        template<typename T> using ComPtr=Microsoft::WRL::ComPtr<T>;
        using Device=ID3D11Device;
        using Context=ID3D11DeviceContext;

        ComPtr<ID3D11RenderTargetView> rtv;
        ComPtr<IDXGISwapChain> swapChain;

        // depth buffer
        ComPtr<ID3D11Texture2D> depthStencilBuffer;
        ComPtr<ID3D11DepthStencilView> dsv;
        ComPtr<ID3D11DepthStencilState> dss;

        D3D11_VIEWPORT screenViewport{};

      public:
        ComPtr<Device> device;
        ComPtr<Context> context;

        RenderAdaptor(HWND hwnd, const ipoint2& screenSize);
        ~RenderAdaptor();

        void setup();
        void swap();
        void recreateRenderTargetView(int width, int height);

      private:
        void cleanupRenderTargetView();
    };
}
