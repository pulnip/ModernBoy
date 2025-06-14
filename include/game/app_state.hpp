#ifndef MODERNBOY_APP_STATE_HPP
#define MODERNBOY_APP_STATE_HPP

#include <SDL3/SDL_render.h>
#include "component_system.hpp"
#include "game_context.hpp"
#include "resource_component.hpp"
#include "resource_manager.hpp"
#include "mesh_importer.hpp"
#include "resource_loader.hpp"
#include "render/renderer.hpp"
#if defined(USE_DIRECTX)
#include "backends/dx11/mesh.hpp"
#include "backends/dx11/context.hpp"
#elif defined(USE_METAL)
#include "backends/metal/mesh.hpp"
#include "backends/metal/shader.hpp"
#include "backends/metal/context.hpp"
#elif defined(USE_OPENGL)
#include "backends/opengl/mesh.hpp"
#include "backends/opengl/context.hpp"
#endif
#include "angelscript.h"

namespace ModernBoy
{
#if defined(USE_DIRECTX)
    using MeshManager = ResourceManager<DX11::Mesh>;
    using ShaderManager = ResourceManager<DX11::DefaultShader>;
    using MeshComponent = ResourceComponent<DX11::Mesh>;
    using MeshComponentSystem = ComponentSystem<MeshComponent>;
    using MeshRenderer = Renderer<DX11::RenderContext>;
#elif defined(USE_METAL)
    using MeshManager = ResourceManager<Metal::Mesh>;
    using ShaderManager = ResourceManager<Metal::Shader>;
    using MeshComponent = ResourceComponent<Metal::Mesh>;
    using MeshComponentSystem = ComponentSystem<MeshComponent>;
    using MeshRenderer = Renderer<Metal::RenderContext>;
    using MeshLoader = ResourceLoader<Metal::Mesh,
        MeshImporter, Metal::NativePtr>;
#elif defined(USE_OPENGL)
    using MeshManager = ResourceManager<OpenGL::Mesh>;
    using MeshRenderer = Renderer<OpenGL::RenderContext>;
    using MeshComponent = ResourceComponent<OpenGL::Mesh>;
    using MeshComponentSystem = ComponentSystem<MeshComponent>;
#endif

    struct AppState{
        MeshManager meshManager;
        ShaderManager shaderManager;
        MeshComponentSystem meshComponentSystem;
        MeshImporter meshImporter;
        SDL_Window* window;
        MeshRenderer renderer;
        MeshLoader meshLoader;
        GameContext game_ctx;
        Uint64 last_step;
        asIScriptEngine* scriptEngine;
        asIScriptContext* scriptContext;

        AppState(SDL_Window* window);
        ~AppState();
    };
}

#endif // MODERNBOY_APP_STATE_HPP