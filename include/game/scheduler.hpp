#ifndef MODERNBOY_GAME_SCHEDULER_HPP
#define MODERNBOY_GAME_SCHEDULER_HPP

#include "game/game_fwd.hpp"

namespace ModernBoy::Game
{
    class Scheduler{
    public:
        Scheduler(Context& world);

        void prepareScheduling();
        void startUpdate();

    private:
        Context& world;
    };
}

#endif // MODERNBOY_GAME_SCHEDULER_HPP