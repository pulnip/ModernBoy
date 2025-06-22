#ifndef MODERNBOY_APP_STATE_HPP
#define MODERNBOY_APP_STATE_HPP

#include "task_system.hpp"
#include "game/game_context.hpp"
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
#elif defined(USE_METAL)
#include "backends/metal/mesh.hpp"
#include "backends/metal/shader.hpp"
#elif defined(USE_OPENGL)
#include "backends/opengl/mesh.hpp"
#endif

namespace ModernBoy
{
    struct AppState{
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
    };
}

#endif // MODERNBOY_APP_STATE_HPP