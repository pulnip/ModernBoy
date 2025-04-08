#ifndef __INC_DX11_CONTEXT_HPP
#define __INC_DX11_CONTEXT_HPP

#include <SDL3/SDL_video.h>
#include "render/render_context.hpp"
#include "resource_handle.hpp"
#include "resource_manager.hpp"
#include "dx11_type.hpp"
#include "dx11_mesh.hpp"

#include "dx11_shader.hpp"

namespace ModernBoy::DX11
{
    using MeshManager = ResourceManager<Mesh>;
    using MeshHandle = ResourceHandle<Mesh>;

    struct RenderContext{
        using WindowType = SDL_Window;
        using MeshType = Mesh;

        MeshManager& meshManager;

        DevicePtr device;
        ContextPtr context;
        UINT qualityLevels = 0;

        Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv;
        D3D11_VIEWPORT viewport{};

        Microsoft::WRL::ComPtr<ID3D11Texture2D> dsb;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilView> dsv;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> dss;

        Shader* shader;

        RenderContext(SDL_Window* in_window, MeshManager& in_meshManager);
        ~RenderContext();

        void beginFrame();
        void render(MeshHandle&);
        void endFrame();
    };
    static_assert(ModernBoy::RenderContext<DX11::RenderContext>);
}

#endif // __INC_DX11_CONTEXT_HPP