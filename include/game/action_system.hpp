#ifndef MODERNBOY_GAME_ACTION_SYSTEM_HPP
#define MODERNBOY_GAME_ACTION_SYSTEM_HPP

#include <vector>
#include "interface.hpp"
#include "task.hpp"
#include "util/generator.hpp"
#include "game/game_fwd.hpp"

namespace ModernBoy::Game
{
    class ActionSystem{
    public:
        ActionSystem(Context& game,
            Script::Invoker& invoker);

        TaskTime expectedExecTime();
        Generator<void> update(DeltaTime dt);

    private:
        Context& world;
        Script::Invoker& invoker;

        std::vector<ActionTask> actionTasks;
    };

} // namespace ModernBoy::Game

#endif // MODERNBOY_GAME_ACTION_SYSTEM_HPP