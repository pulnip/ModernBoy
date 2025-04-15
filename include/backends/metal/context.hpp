#ifndef MODERNBOY_DX11_CONTEXT_HPP
#define MODERNBOY_DX11_CONTEXT_HPP

#include <SDL3/SDL_video.h>
#include "render/render_command.hpp"
#include "render/render_context.hpp"
#include "resource_handle.hpp"
#include "resource_manager.hpp"
#include "type.hpp"
#include "mesh.hpp"
#include "shader.hpp"

namespace ModernBoy::Metal
{
    using MeshManager = ResourceManager<Mesh>;
    using MeshHandle = ResourceHandle<Mesh>;
    using ShaderManager = ResourceManager<Metal::DefaultShader>;
    using ShaderHandle = ResourceHandle<Metal::DefaultShader>;

    struct RenderContext{
        using Window = SDL_Window;
        using Mesh = Mesh;
        using Shader = Metal::DefaultShader;

        MeshManager& meshManager;
        ShaderManager& shaderManager;

        RenderContext(SDL_Window* in_window, MeshManager& in_meshManager,
            ShaderManager& in_shaderManager);
        ~RenderContext();

        void operator()(const FrameStartCommand<Shader>&);
        void operator()(const DrawCommand<Mesh>&);
        void operator()(const FrameEndCommand&);
    };
    static_assert(ModernBoy::RenderContext<Metal::RenderContext>);
}

#endif // MODERNBOY_DX11_CONTEXT_HPP