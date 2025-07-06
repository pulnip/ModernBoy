#ifndef MODERNBOY_GAME_INPUT_SYSTEM_HPP
#define MODERNBOY_GAME_INPUT_SYSTEM_HPP

#include <vector>
#include "interface.hpp"
#include "task.hpp"
#include "util/generator.hpp"
#include "game/game_fwd.hpp"

namespace ModernBoy::Game
{
    class InputSystem{
    public:
        InputSystem(Context& world);

        size_t yield_count() const noexcept;

        Generator<void> updateTask(DeltaTime dt);
        Generator<void> update(DeltaTime dt);

    private:
        Context& world;

        mutable size_t numTask;
        std::vector<InputTask> inputTasks;
    }; static_assert(Schedulable<InputSystem>);

} // namespace ModernBoy::Game

#endif // MODERNBOY_GAME_INPUT_SYSTEM_HPP