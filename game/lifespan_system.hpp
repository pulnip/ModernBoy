#ifndef MODERNBOY_GAME_LIFESPAN_SYSTEM_HPP
#define MODERNBOY_GAME_LIFESPAN_SYSTEM_HPP

#include "fwd.hpp"
#include "interface.hpp"
#include "thread/generator.hpp"
#include "game_fwd.hpp"

namespace ModernBoy::Game
{
    class LifespanSystem{
    public:
        LifespanSystem(EntityRegistry& registry);

        // size_t yield_count() const noexcept;

        void updateTask(DeltaTime) noexcept;
        void update(DeltaTime) noexcept;
        // Generator<void> updateTask(DeltaTime dt) noexcept;
        // Generator<void> update(DeltaTime dt) noexcept; 

    private:
        EntityRegistry& registry;

        mutable size_t numDeadActors;

    }; // static_assert(Schedulable<LifespanSystem>);
} // namespace ModernBoy::Game

#endif // MODERNBOY_GAME_LIFESPAN_SYSTEM_HPP