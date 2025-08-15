#ifndef MODERNBOY_DX11_CONTEXT_HPP
#define MODERNBOY_DX11_CONTEXT_HPP

#include <SDL3/SDL_video.h>
#include "engine/fwd.hpp"
#include "resource_manager.hpp"
#include "type.hpp"

namespace ModernBoy::DX11
{
    struct RenderContext{

        MeshManager& meshManager;
        ShaderManager& shaderManager;

        DevicePtr device;
        ContextPtr context;
        UINT qualityLevels = 0;

        Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv;
        D3D11_VIEWPORT viewport{};

        Microsoft::WRL::ComPtr<ID3D11Texture2D> dsb;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilView> dsv;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> dss;

    public:
        RenderContext()=delete;
        ~RenderContext();
        RenderContext(const RenderContext&)=delete;
        RenderContext(RenderContext&&);
        RenderContext& operator=(const RenderContext&)=delete;
        RenderContext& operator=(RenderContext&&)=delete;

        RenderContext(SDL_Window* in_window);

        void onFrameStart(Vec4 clearColor);
        void setView(float fov,
            Vec3 viewPos, Vec4 viewQuat);
        void setShader(NativePtr shader);
        void setTexture(NativePtr texture);
        void drawMesh(Vec3 position, Vec4 rotation,
            Vec3 scale, NativePtr mesh, float alpha);
        void onFrameEnd();

        NativePtr getDevice();
        NativePtr getContext();

    private:
        // Move semantics
        void moveFrom(RenderContext&& other);
    };
}

#endif // MODERNBOY_DX11_CONTEXT_HPP