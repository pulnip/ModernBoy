#ifndef MODERNBOY_GAME_GAMECOMMANDBUS_HPP
#define MODERNBOY_GAME_GAMECOMMANDBUS_HPP

#include <mutex>
#include <variant>
#include <vector>

namespace ModernBoy::Game
{
    enum System{
        CAMERA_RAY
    };

    struct ActivateSystem{
        System targetSystem;
        bool activate;
    };

    using GameCommand = std::variant<
        ActivateSystem>;

    class GameCommandBus{
    public:
        GameCommandBus() = default;
        ~GameCommandBus() = default;

        void write(GameCommand command);

        std::vector<GameCommand> drainCommands();

    private:
        std::vector<GameCommand> commands;

        std::mutex mtx;
    };
}

#endif // MODERNBOY_GAME_GAMECOMMANDBUS_HPP