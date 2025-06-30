#ifndef MODERNBOY_GAME_CONTEXT_HPP
#define MODERNBOY_GAME_CONTEXT_HPP

#include "fwd.hpp"

class EventDispatcher{}; // global / local?
class UpdateScheduler{};
class SpatialSystem{};
class CollisionDetector{}; // query nearBy(radius

namespace ModernBoy::Game
{
    class Context{
        EventDispatcher eventDispatcher;
        
    };

    struct Actor{
        EntityID id;
        Context& context;

        AppState& app;
    };
}

#endif // MODERNBOY_GAME_CONTEXT_HPP