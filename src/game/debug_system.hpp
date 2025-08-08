#ifndef MODERNBOY_DEBUG_SYSTEM_HPP
#define MODERNBOY_DEBUG_SYSTEM_HPP

#include "engine/fwd.hpp"
#include "core/thread/generator.hpp"
#include "game_fwd.hpp"

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