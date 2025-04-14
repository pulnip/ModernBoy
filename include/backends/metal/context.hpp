#ifndef MODERNBOY_DX11_CONTEXT_HPP
#define MODERNBOY_DX11_CONTEXT_HPP

#include <SDL3/SDL_video.h>
#include "render/render_command.hpp"
#include "render/render_context.hpp"
#include "resource_handle.hpp"
#include "resource_manager.hpp"
#include "type.hpp"
#include "mesh.hpp"

namespace ModernBoy::Metal
{
    using MeshManager = ResourceManager<Mesh>;
    using MeshHandle = ResourceHandle<Mesh>;

    struct RenderContext{
        using Window = SDL_Window;
        using Mesh = Mesh;

        MeshManager& meshManager;

        RenderContext(SDL_Window* in_window, MeshManager& in_meshManager);
        ~RenderContext();

        void operator()(const FrameStartCommand&);
        void operator()(const DrawCommand<Mesh>&);
        void operator()(const FrameEndCommand&);
    };
    static_assert(ModernBoy::RenderContext<Metal::RenderContext>);
}

#endif // MODERNBOY_DX11_CONTEXT_HPP