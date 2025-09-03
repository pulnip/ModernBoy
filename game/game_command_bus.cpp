#include "game_command_bus.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Game;

void GameCommandBus::write(GameCommand command){
    std::lock_guard lock(mtx);
    commands.push_back(command);
}

std::vector<GameCommand> GameCommandBus::drainCommands(){
    std::vector<GameCommand> out;
    {
        std::lock_guard lock(mtx);
        out.swap(commands);
    }
    return out;
}
