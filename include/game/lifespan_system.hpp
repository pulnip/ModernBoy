#ifndef MODERNBOY_GAME_LIFESPAN_SYSTEM_HPP
#define MODERNBOY_GAME_LIFESPAN_SYSTEM_HPP

#include "interface.hpp"

namespace ModernBoy::Game
{
    class LifespanSystem{
    public:
        LifespanSystem(AppState& app);
        ~LifespanSystem() = default;

        size_t yield_count() const noexcept;
        Generator<void> updateTask(DeltaTime dt) noexcept;
        Generator<void> update(DeltaTime dt) noexcept;   

    private:
        AppState& app;
    }; static_assert(Schedulable<LifespanSystem>);
} // namespace ModernBoy::Game

#endif // MODERNBOY_GAME_LIFESPAN_SYSTEM_HPP