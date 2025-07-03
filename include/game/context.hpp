#ifndef MODERNBOY_GAME_CONTEXT_HPP
#define MODERNBOY_GAME_CONTEXT_HPP

#include "fwd.hpp"
#include "interface.hpp"

class EventDispatcher{}; // global / local?
class UpdateScheduler{};
class SpatialSystem{};
class CollisionDetector{}; // query nearBy(radius

namespace ModernBoy::Game
{
    class Context{
    public:
        size_t yield_count() const noexcept;
        Generator<void> updateTask(DeltaTime dt);
        Generator<void> update(DeltaTime dt);

    private:
        EventDispatcher eventDispatcher;
    };static_assert(Schedulable<Context>);

    struct Actor{
        EntityID id;
        Context* world;

        AppState* app;
    };
}

#endif // MODERNBOY_GAME_CONTEXT_HPP