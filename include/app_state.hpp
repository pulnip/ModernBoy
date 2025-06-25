#ifndef MODERNBOY_APP_STATE_HPP
#define MODERNBOY_APP_STATE_HPP

#include <vector>
#include <unordered_map>
#include "resource_manager.hpp"
#include "mesh_importer.hpp"
#include "asset_loader.hpp"
#include "util/object_pool.hpp"
#include "archetype_map.hpp"
#include "component.hpp"
#include "task.hpp"
#include "script/controller.hpp"
#include "input/device.hpp"
#include "render/renderer.hpp"
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
    // Components
    using InputComponent = ValueComponent<Input::InputMap>;
    struct SparseChunk{
        TransformComponent transform;
        CameraComponent camera;
        MeshComponent mesh;
        InputComponent input;
    };
    // Archetype
    struct ComponentInfo{
        ArchetypeBit bit;
        size_t chunkIndex;
    };
    using EntityTable = std::unordered_map<EntityID, ComponentInfo>;
    // Component Pool
    using TransformPool = ObjectPool<TransformComponent>;
    using CameraPool = ObjectPool<CameraComponent>;
    using MeshPool = ObjectPool<MeshComponent>;
    using InputPool = ObjectPool<InputComponent>;
    // Systems
    using RenderSystem = TaskMap<RenderTask>;
    using ViewSystem = TaskMap<ViewTask>;

    struct AppState{
    private:
        EntityID id_seed;
        EntityID issueID();

    public:
        // Important!! Initialize Order
        // Hardwares
        UI gui;
        MeshImporter meshImporter;
        SDL_Window* window;
        Input::Device inputDevice;
        // Resource Managers
        MeshManager meshManager;
        ShaderManager shaderManager;

        EntityTable actorTable;
        ArchetypeMap archetypeMap;
        
        TransformPool transformPool;
        CameraPool cameraPool;
        MeshPool meshPool;
        InputPool inputPool;
        
        RenderSystem renderSystem;
        ViewSystem viewSystem;
        InputSystem inputSystem;
        // Softwares
        Script::Controller controller;
        Renderer renderer;

        AssetLoader assetLoader;
        // Game State
        GameContext game_ctx;
        Uint64 last_step;

        AppState(SDL_Window* window);
        ~AppState();

        EntityID createActor(ArchetypeBit bit,
            SparseChunk&& components);
        void destroyActor(EntityID ID);

        template<typename Resource>
        std::vector<Resource> import(const std::string&);

        template<typename Component>
        std::optional<Component> get(EntityID actor);

        template<typename Task>
        std::vector<Task> get();
        template<typename Task>
        std::vector<Task> get(const Input::State& state);

    };
}

#endif // MODERNBOY_APP_STATE_HPP