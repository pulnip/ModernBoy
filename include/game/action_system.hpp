#ifndef MODERNBOY_GAME_ACTION_SYSTEM_HPP
#define MODERNBOY_GAME_ACTION_SYSTEM_HPP

#include <vector>
#include "interface.hpp"
#include "task.hpp"
#include "core/thread/generator.hpp"
#include "game/game_fwd.hpp"

namespace ModernBoy::Game
{
    class ActionSystem{
    public:
        ActionSystem(EntityRegistry& registry,
            ::ModernBoy::Script::Invoker& invoker);

        // TaskTime expectedExecTime();
        void update(DeltaTime);
        // Generator<void> update(DeltaTime dt);

    private:
        EntityRegistry& registry;
        ::ModernBoy::Script::Invoker& invoker;

        std::vector<ActionTask> actionTasks;
    };

} // namespace ModernBoy::Game

#endif // MODERNBOY_GAME_ACTION_SYSTEM_HPP