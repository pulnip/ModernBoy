#ifndef MODERNBOY_APP_STATE_HPP
#define MODERNBOY_APP_STATE_HPP

#include <chrono>
#include <vector>
#include <unordered_map>
#include "resource_manager.hpp"
#include "asset_loader.hpp"
#include "util/object_pool.hpp"
#include "util/bit.hpp"
#include "archetype_map.hpp"
#include "component.hpp"
#include "task.hpp"
#include "input/device.hpp"
#include "input/system.hpp"
#include "render/gui.hpp"
#include "render/system.hpp"
#include "script/type.hpp"
#include "script/module.hpp"
#include "script/invoker.hpp"
#include "game/context.hpp"
#if defined(USE_DIRECTX)
#include "backends/dx11/mesh.hpp"
#elif defined(USE_METAL)
#include "backends/metal/mesh.hpp"
#include "backends/metal/texture.hpp"
#include "backends/metal/shader.hpp"
#elif defined(USE_OPENGL)
#include "backends/opengl/mesh.hpp"
#endif

namespace ModernBoy
{
    // Components
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

    struct AppState{
    private:
        EntityID id_seed = 0;
        EntityID issueID();
        
    public:
        UI ui;
        SDL_Window* window;
        Input::Device inputDevice;

        MeshManager meshManager;
        TextureManager textureManager;
        ShaderManager shaderManager;
        ModuleManager moduleManager;

        EntityTable actorTable;
        ArchetypeMap archetypeMap;

        Render::System renderSystem;
        Input::System inputSystem;
        Script::Invoker scriptInvoker;

        AssetLoader assetLoader;
        // Game State
        Game::Context world;

        Uint64 lastTicks;
        Uint64 deltaTime;
        Uint64 getDeltaTime() const;

        AppState(SDL_Window* window);
        ~AppState();

        EntityID createActor(ArchetypeBit bit,
            SparseChunk&& components);
        void destroyActor(EntityID ID);

        template<typename Resource>
        Resource& get(ResourceHandle);
        template<typename Resource>
        Resource& get(const std::string& name);
        template<typename Resource>
        ResourceHandle getHandle(const std::string& name);
        template<typename Component>
        std::optional<Component> query(EntityID actor);
    };
}

#endif // MODERNBOY_APP_STATE_HPP