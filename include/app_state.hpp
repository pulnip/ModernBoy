#ifndef MODERNBOY_APP_STATE_HPP
#define MODERNBOY_APP_STATE_HPP

#include "resource_manager.hpp"
#include "mesh_importer.hpp"
#include "resource_loader.hpp"
#include "asset_loader.hpp"
#include "util/object_pool.hpp"
#include "component.hpp"
#include "task_system.hpp"
#include "input/controller.hpp"
#include "input/device.hpp"
#include "input/component.hpp"
#include "render/renderer.hpp"
#include "render/component.hpp"
#include "render/gui.hpp"
#include "game/game_context.hpp"
#if defined(USE_DIRECTX)
#include "backends/dx11/mesh.hpp"
#elif defined(USE_METAL)
#include "backends/metal/mesh.hpp"
#include "backends/metal/shader.hpp"
#elif defined(USE_OPENGL)
#include "backends/opengl/mesh.hpp"
#endif

namespace ModernBoy
{
    // Archetype
    using EntityTable = ObjectPool<ResourceType>;
    template<typename Component>
    using ComponentPool = ObjectPool<Component>;

    struct AppState{
        // Important!! Initialize Order
        // Hardwares
        UI gui;
        MeshImporter meshImporter;
        SDL_Window* window;
        Input::Device inputDevice;
        // Resource Managers
        TransformManager transformManager;
        MeshManager meshManager;
        ShaderManager shaderManager;
        CameraManager cameraManager;
        EntityTable actorTable;
        ComponentPool transformPool;
        ComponentPool meshPool;
        ComponentPool shaderPool;
        ComponentPool cameraPool;
        RenderSystem renderSystem;
        ViewSystem viewSystem;
        InputSystem inputSystem;
        // Softwares
        Input::Controller controller;
        Renderer renderer;
        MeshLoader meshLoader;
        AssetLoader assetLoader;
        // Game State
        GameContext game_ctx;
        Uint64 last_step;

        AppState(SDL_Window* window);
        ~AppState();

        template<typename Resource>
        std::vector<Resource> import(const std::string&);

        template<typename Resource>
        Resource& get(ResourceHandle handle);
        template<typename Task>
        std::vector<Task> get();
    };
}

#endif // MODERNBOY_APP_STATE_HPP