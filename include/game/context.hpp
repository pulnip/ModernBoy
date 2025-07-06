#ifndef MODERNBOY_GAME_CONTEXT_HPP
#define MODERNBOY_GAME_CONTEXT_HPP

#include <ranges>
#include "fwd.hpp"
#include "interface.hpp"
#include "game/archetype_map.hpp"
#include "game/component.hpp"
#include "game/draw_system.hpp"
#include "game/input_system.hpp"
#include "game/physics_system.hpp"
#include "game/lifespan_system.hpp"
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

        size_t yield_count() const noexcept;

        Generator<void> update(DeltaTime dt);

        uint32_t issueID();
        DeltaTime getDeltaTime();

        template<typename Component>
        std::optional<Component> query(EntityID actor){
            const auto& info = actorTable.at(actor);
            auto querybit = bit_of<Component>();
            if((info.bit & querybit) != querybit)
                return std::nullopt;

            return archetypeMap.get<Component>(
                info.bit, info.chunkIndex);
        }
        template<typename Component>
        ABNORMAL_FLAG update(EntityID actor, Component&& component){
            const auto& info = actorTable.at(actor);
            auto querybit = bit_of<Component>();
            if((info.bit & querybit) != querybit)
                return true;
            archetypeMap.set(std::forward<Component>(component),
                info.bit, info.chunkIndex);
            return false;
        }

        EntityID create(ArchetypeBit bit, SparseChunk&& chunk);
        void destroy(EntityID actor);

        template<typename... Component>
        auto query(){
            return archetypeMap
            | std::views::filter(
                [](auto&& pair){
                    return subset(bits_of<Component...>(),
                        pair.first);
                }
            );
        }

        template<Event event>
        void on();

        template<typename Task>
        const std::vector<Task>& getBuffer() const;

    private:
        AppState& app;

        ArchetypeMap archetypeMap;
        EntityTable actorTable;

        DrawSystem draw;
        InputSystem script;
        PhysicsSystem physics;
        LifespanSystem lifespan;

        friend class AppState;
    };

    struct Actor{
        EntityID id;
        Context* world;
    };
} // namespace ModernBoy::Game

#endif // MODERNBOY_GAME_CONTEXT_HPP