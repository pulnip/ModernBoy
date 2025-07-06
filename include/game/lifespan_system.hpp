#ifndef MODERNBOY_GAME_LIFESPAN_SYSTEM_HPP
#define MODERNBOY_GAME_LIFESPAN_SYSTEM_HPP

#include "fwd.hpp"
#include "interface.hpp"
#include "util/generator.hpp"
#include "game/game_fwd.hpp"

namespace ModernBoy::Game
{
    class LifespanSystem{
    public:
        LifespanSystem(Context& world);

        size_t yield_count() const noexcept;

        Generator<void> updateTask(DeltaTime dt) noexcept;
        Generator<void> update(DeltaTime dt) noexcept; 

    private:
        Context& world;

        mutable size_t numDeadActors;

    }; static_assert(Schedulable<LifespanSystem>);
} // namespace ModernBoy::Game

#endif // MODERNBOY_GAME_LIFESPAN_SYSTEM_HPP