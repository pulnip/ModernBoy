#ifndef __INC_APP_STATE_HPP
#define __INC_APP_STATE_HPP

#include <SDL3/SDL_render.h>
#include "component_system.hpp"
#include "game_context.hpp"
#include "resource_component.hpp"
#include "resource_manager.hpp"
#include "render/renderer.hpp"
#if defined(USE_DIRECTX)
#include "backends/dx11/mesh.hpp"
#include "backends/dx11/context.hpp"
#elif defined(USE_METAL)
#include "backends/metal/mesh.hpp"
#include "backends/metal/context.hpp"
#elif defined(USE_OPENGL)
#include "backends/opengl/mesh.hpp"
#include "backends/opengl/context.hpp"
#endif

namespace ModernBoy
{
#if defined(USE_DIRECTX)
    using MeshManager = ResourceManager<DX11::Mesh>;
    using MeshRenderer = Renderer<DX11::RenderContext>;
    using MeshComponent = ResourceComponent<DX11::Mesh>;
    using MeshComponentSystem = ComponentSystem<MeshComponent>;
#elif defined(USE_METAL)
    using MeshManager = ResourceManager<Metal::Mesh>;
    using MeshRenderer = Renderer<Metal::RenderContext>;
    using MeshComponent = ResourceComponent<Metal::Mesh>;
    using MeshComponentSystem = ComponentSystem<MeshComponent>;
#elif defined(USE_OPENGL)
    using MeshManager = ResourceManager<OpenGL::Mesh>;
    using MeshRenderer = Renderer<OpenGL::RenderContext>;
    using MeshComponent = ResourceComponent<OpenGL::Mesh>;
    using MeshComponentSystem = ComponentSystem<MeshComponent>;
#endif

    struct AppState{
        SDL_Window* window;
        MeshManager meshManager;
        MeshComponentSystem meshComponentSystem;
        MeshRenderer renderer;
        GameContext game_ctx;
        Uint64 last_step;

        AppState(SDL_Window* window);
    };
}

#endif // __INC_APP_STATE_HPP