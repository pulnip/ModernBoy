#ifndef MODERNBOY_GAME_CONTEXT_HPP
#define MODERNBOY_GAME_CONTEXT_HPP

#include <ranges>
#include "engine/fwd.hpp"
#include "engine/interface.hpp"
#include "game/entity_registry.hpp"
#include "game/component.hpp"
#include "game/action_system.hpp"
#include "game/debug_system.hpp"
#include "game/draw_system.hpp"
#include "game/input_system.hpp"
#include "game/physics_system.hpp"
#include "game/lifespan_system.hpp"
#include "game/scheduler.hpp"
#include "game/event.hpp"
#include "core/thread/generator.hpp"

namespace ModernBoy::Game
{
    struct ComponentInfo{
        ArchetypeBit bit;
        Index chunkIndex;
    };

    using EntityTable = std::unordered_map<EntityID, ComponentInfo>;

    class Context{
    public:
        Context(Engine& engine);

        void update(DeltaTime dt);

        uint32_t issueID();
        DeltaTime getDeltaTime();

        template<Event event>
        void on();

        template<typename Task>
        const std::vector<Task>& getBuffer() const;

        DeltaTime getDeltaTime() const;

    private:
        Engine& engine;

    public:
        EntityRegistry registry;
    private:
        DebugSystem debug;
        DrawSystem draw;
        ActionSystem action;
        InputSystem input;
        PhysicsSystem physics;
        LifespanSystem lifespan;

        friend class ModernBoy::AssetLoader;
    };
} // namespace ModernBoy::Game

#endif // MODERNBOY_GAME_CONTEXT_HPP