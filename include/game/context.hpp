#ifndef MODERNBOY_GAME_CONTEXT_HPP
#define MODERNBOY_GAME_CONTEXT_HPP

#include <ranges>
#include "fwd.hpp"
#include "interface.hpp"
#include "game/entity_registry.hpp"
#include "game/component.hpp"
#include "game/action_system.hpp"
#include "game/draw_system.hpp"
#include "game/physics_system.hpp"
#include "game/lifespan_system.hpp"
#include "game/scheduler.hpp"
#include "game/event.hpp"
#include "util/generator.hpp"

namespace ModernBoy::Game
{
    struct ComponentInfo{
        ArchetypeBit bit;
        Index chunkIndex;
    };

    using EntityTable = std::unordered_map<EntityID, ComponentInfo>;

    class Context{
    public:
        Context(AppState& app);

        void update(DeltaTime dt);

        uint32_t issueID();
        DeltaTime getDeltaTime();

        template<Event event>
        void on();

        template<typename Task>
        const std::vector<Task>& getBuffer() const;

        DeltaTime getDeltaTime() const;

    private:
        AppState& app;

    public:
        EntityRegistry registry;
    private:
        DrawSystem draw;
        ActionSystem action;
        PhysicsSystem physics;
        LifespanSystem lifespan;

        friend class ModernBoy::AssetLoader;
    };

    struct Actor{
        EntityID id;
        Context* world;
    };
} // namespace ModernBoy::Game

#endif // MODERNBOY_GAME_CONTEXT_HPP