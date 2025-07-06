#include "game/scheduler.hpp"
#include "game/context.hpp"
#include "log.hpp"

using namespace ModernBoy::Game;

Scheduler::Scheduler(Context& world)
:world(world){}

void Scheduler::prepareScheduling(){
    GameDebug("prepareScheduling");
}
void Scheduler::startUpdate(){
    GameDebug("startUpdate");
}


