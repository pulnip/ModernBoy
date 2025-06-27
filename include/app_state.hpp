#ifndef MODERNBOY_APP_STATE_HPP
#define MODERNBOY_APP_STATE_HPP

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
#include "script/invoker.hpp"
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
        UI gui;
        SDL_Window* window;
        Input::Device inputDevice;

        MeshManager meshManager;
        ShaderManager shaderManager;
        ModuleManager moduleManager;

        EntityTable actorTable;
        ArchetypeMap archetypeMap;

        Render::System renderSystem;
        Input::System inputSystem;
        Script::Invoker scriptInvoker;

        AssetLoader assetLoader;
        // Game State
        GameContext game_ctx;
        Uint64 last_step;

        AppState(SDL_Window* window);
        ~AppState();

        EntityID createActor(ArchetypeBit bit,
            SparseChunk&& components);
        void destroyActor(EntityID ID);

        template<typename Component>
        std::optional<Component> query(EntityID actor){
            const auto& comp = actorTable.at(actor);
            auto querybit = bit_of<Component>;
            if((comp.bit & querybit) != querybit)
                return std::nullopt;
            Component c;
            archetypeMap.at(comp.bit).get(comp.chunkIndex, &c);
        }
    };
}

#endif // MODERNBOY_APP_STATE_HPP