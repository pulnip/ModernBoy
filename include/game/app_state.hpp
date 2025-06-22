#ifndef MODERNBOY_APP_STATE_HPP
#define MODERNBOY_APP_STATE_HPP

#include <SDL3/SDL_render.h>
#include "task_system.hpp"
#include "view_system.hpp"
#include "game_context.hpp"
#include "resource_component.hpp"
#include "resource_manager.hpp"
#include "mesh_importer.hpp"
#include "resource_loader.hpp"
#include "asset_loader.hpp"
#include "input/controller.hpp"
#include "input/device.hpp"
#include "input/component.hpp"
#include "render/renderer.hpp"
#include "render/component.hpp"
#include "render/gui.hpp"
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

namespace ModernBoy
{
#if defined(USE_DIRECTX)
    using MeshManager = ResourceManager<DX11::Mesh>;
    using ShaderManager = ResourceManager<DX11::DefaultShader>;
    using MeshComponent = ResourceComponent<DX11::Mesh>;
    using TaskManager = TaskManager<MeshComponent>;
    using MeshRenderer = Renderer<DX11::RenderContext>;
#elif defined(USE_METAL)
    using MeshManager = ResourceManager<Metal::Mesh>;
    using ShaderManager = ResourceManager<Metal::Shader>;
    using RenderComponent = Render::Component<Metal::Mesh>;
    using RenderTask = Render::Task<Metal::Mesh>;
    using RenderSystem = TaskSystem<RenderComponent, RenderTask>;
    using MeshRenderer = Render::Renderer<Metal::RenderContext>;
    using MeshLoader = ResourceLoader<Metal::Mesh,
        MeshImporter, Metal::NativePtr>;
#elif defined(USE_OPENGL)
    using MeshManager = ResourceManager<OpenGL::Mesh>;
    using MeshRenderer = Renderer<OpenGL::RenderContext>;
    using MeshComponent = ResourceComponent<OpenGL::Mesh>;
    using TaskManager = TaskManager<MeshComponent>;
#endif
    using InputSystem = TaskSystem<Input::Component, Input::Task>;
    using ActorLoader = AssetLoader<
        MeshLoader, RenderSystem, InputSystem>;

    struct AppState{
        UI gui;
        TransformManager transformManager;
        MeshImporter meshImporter;
        MeshManager meshManager;
        ShaderManager shaderManager;
        RenderSystem renderSystem;
        CameraManager cameraManager;
        ViewSystem viewSystem;
        SDL_Window* window;
        MeshRenderer renderer;
        MeshLoader meshLoader;
        Input::Device inputDevice;
        InputSystem inputSystem;
        Input::Controller controller;
        ActorLoader actorLoader;
        GameContext game_ctx;
        Uint64 last_step;

        AppState(SDL_Window* window);
        ~AppState();
    };
}

#endif // MODERNBOY_APP_STATE_HPP