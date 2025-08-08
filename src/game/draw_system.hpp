#ifndef MODERNBOY_GAME_DRAW_SYSTEM_HPP
#define MODERNBOY_GAME_DRAW_SYSTEM_HPP

#include <vector>
#include "engine/fwd.hpp"
#include "engine/interface.hpp"
#include "core/thread/generator.hpp"
#include "game_fwd.hpp"
#include "engine/task.hpp"

namespace ModernBoy::Game
{
    class DrawSystem{
    public:
        DrawSystem(EntityRegistry& registry);

        // size_t yield_count() noexcept;

        void update(DeltaTime);
        // Generator<void> update(DeltaTime dt);

    private:
        EntityRegistry& registry;

        std::vector<ViewTask> viewTasks;
        std::vector<DrawTask> drawTasks;

    friend class Context;
    }; // static_assert(Schedulable<DrawSystem>);
} // namespace ModernBoy::Game

#endif // MODERNBOY_GAME_DRAW_SYSTEM_HPP