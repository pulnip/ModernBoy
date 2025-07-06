#ifndef MODERNBOY_GAME_EVENT_HPP
#define MODERNBOY_GAME_EVENT_HPP

namespace ModernBoy::Game
{
    enum class Event{
        SimulationStarted,
        SimulationFinished,
        PlayerSpawned,
        PlayerMove,
        PlayerDead,
        WorldReset
    };
}

#endif // MODERNBOY_GAME_EVENT_HPP