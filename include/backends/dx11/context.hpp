#ifndef __INC_DX11_CONTEXT_HPP
#define __INC_DX11_CONTEXT_HPP

#include <SDL3/SDL_video.h>
#include "render/render_command.hpp"
#include "render/render_context.hpp"
#include "resource_handle.hpp"
#include "resource_manager.hpp"
#include "type.hpp"
#include "mesh.hpp"
#include "shader.hpp"

namespace ModernBoy::DX11
{
    using MeshManager = ResourceManager<DX11::Mesh>;
    using MeshHandle = ResourceHandle<DX11::Mesh>;
    using ShaderManager = ResourceManager<DX11::DefaultShader>;
    using ShaderHandle = ResourceHandle<DX11::DefaultShader>;

    struct RenderContext{
        using Window = SDL_Window;
        using Mesh = DX11::Mesh;
        using Shader = DX11::DefaultShader;

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

        RenderContext(SDL_Window* in_window, MeshManager& in_meshManager,
            ShaderManager& in_shaderManager);

        void operator()(const FrameStartCommand<Shader>&);
        void operator()(const DrawCommand<Mesh>&);
        void operator()(const FrameEndCommand&);

    private:
        // Move semantics
        void moveFrom(RenderContext&& other);
    };
    static_assert(ModernBoy::RenderContext<DX11::RenderContext>);
}

#endif // __INC_DX11_CONTEXT_HPP