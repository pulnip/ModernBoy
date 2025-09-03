#include "interface/engine_command_bus.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Interface;

void EngineCommandBus::write(EngineCommand command){
    std::lock_guard lock(mtx);
    commands.push_back(command);
}

std::vector<EngineCommand> EngineCommandBus::drainCommands(){
    std::vector<EngineCommand> out;
    {
        std::lock_guard lock(mtx);
        out.swap(commands);
    }
    return out;
}
