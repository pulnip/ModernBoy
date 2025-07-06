#ifndef MODERNBOY_GAME_PHYSICS_SYSTEM_HPP
#define MODERNBOY_GAME_PHYSICS_SYSTEM_HPP

#include "fwd.hpp"
#include "interface.hpp"
#include "util/generator.hpp"
#include "game/game_fwd.hpp"

namespace ModernBoy::Game
{
    class PhysicsSystem{
    public:
        PhysicsSystem(Context& world);

        size_t yield_count() const noexcept;

        Generator<void> updateTask(DeltaTime dt);
        Generator<void> update(DeltaTime dt);

    private:
        Context& world;
    }; static_assert(Schedulable<PhysicsSystem>);
} // namespace ModernBoy::Game

#endif // MODERNBOY_GAME_PHYSICS_SYSTEM_HPP