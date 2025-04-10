#ifndef __INC_APP_STATE_HPP
#define __INC_APP_STATE_HPP

#include <SDL3/SDL_render.h>
#include "component_system.hpp"
#include "game_context.hpp"
#include "resource_component.hpp"
#include "resource_manager.hpp"
#include "render/renderer.hpp"
#ifdef USE_DIRECTX
#include "backends/dx11_mesh.hpp"
#include "backends/dx11_context.hpp"
#elif defined(USE_OPENGL)
#endif

namespace ModernBoy
{
#ifdef USE_DIRECTX
    using MeshManager = ResourceManager<DX11::Mesh>;
    using MeshRenderer = Renderer<DX11::RenderContext>;
    using MeshComponent = ResourceComponent<DX11::Mesh>;
    using MeshComponentSystem = ComponentSystem<MeshComponent>;
#elif defined(USE_OPENGL)
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