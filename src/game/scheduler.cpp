#include "game/scheduler.hpp"
#include "game/context.hpp"
#include "log.hpp"

using namespace ModernBoy::Game;

Scheduler::Scheduler(Context& world)
:world(world){}

void Scheduler::prepareScheduling(DeltaTime dt){
    deltaTime = dt;
    generators.clear();

    // world.prepareScheduling();
}
void Scheduler::startUpdate(){
    GameTrace("GameUpdate, Num Generators: {}",
        generators.size());

    while(true){
        bool allDone = true;

        for(auto& generator: generators){
            if(generator.done())
                continue;
            generator.next();
            allDone = false;
        }

        if(allDone)
            break;
    }
}


