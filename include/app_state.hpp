#ifndef MODERNBOY_APP_STATE_HPP
#define MODERNBOY_APP_STATE_HPP

#include <vector>
#include <unordered_map>
#include "resource_manager.hpp"
#include "mesh_importer.hpp"
#include "asset_loader.hpp"
#include "util/object_pool.hpp"
#include "util/bit.hpp"
#include "archetype_map.hpp"
#include "component.hpp"
#include "task.hpp"
#include "script/controller.hpp"
#include "input/device.hpp"
#include "input/system.hpp"
#include "render/gui.hpp"
#include "render/system.hpp"
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

    struct AppState{
    private:
        EntityID id_seed = 0;
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

        Render::System renderSystem;
        Input::System inputSystem;
        Script::Controller controller;

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