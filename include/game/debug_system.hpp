#ifndef MODERNBOY_DEBUG_SYSTEM_HPP
#define MODERNBOY_DEBUG_SYSTEM_HPP

#include "fwd.hpp"
#include "util/generator.hpp"
#include "game/game_fwd.hpp"

namespace ModernBoy::Game
{
    class DebugSystem{
    public:
        DebugSystem(EntityRegistry&);

        void update(DeltaTime);

    private:
        EntityRegistry& registry;
    };
}

#endif // MODERNBOY_DEBUG_SYSTEM_HPP