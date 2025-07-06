#ifndef MODERNBOY_GAME_DRAW_SYSTEM_HPP
#define MODERNBOY_GAME_DRAW_SYSTEM_HPP

#include <vector>
#include "fwd.hpp"
#include "interface.hpp"
#include "util/generator.hpp"
#include "game/game_fwd.hpp"
#include "task.hpp"

namespace ModernBoy::Game
{
    class DrawSystem{
    public:
        DrawSystem(Context& world);

        size_t yield_count() const noexcept;

        Generator<void> updateTask(DeltaTime dt);
        Generator<void> update(DeltaTime dt);



    private:
        Context& world;

        std::vector<ViewTask> viewTasks;
        std::vector<DrawTask> drawTasks;

    friend class Context;
    }; static_assert(Schedulable<DrawSystem>);
} // namespace ModernBoy::Game

#endif // MODERNBOY_GAME_DRAW_SYSTEM_HPP