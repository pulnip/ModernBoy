#include "engine/log.hpp"
#include "engine/engine.hpp"
#include "game/context.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Game;

Context::Context(Engine& engine)
:engine(engine),
debug(registry), draw(registry),
action(registry, engine.scriptInvoker),
input(registry, engine.inputDevice, engine.renderer),
physics(registry), lifespan(registry){}


void Context::update(DeltaTime dt){
    input.update(dt);
    action.update(dt);
    // auto actionFut = action.update(dt);
    // while(!actionFut.done())
    //     actionFut.next();

    physics.update(dt);
    // auto physicsFut = physics.update(dt);
    // while(!physicsFut.done())
    //     physicsFut.next();

    debug.update(dt);
    // auto debugFut = debug.update(dt);
    // while(!debugFut.done())
    //     debugFut.next();

    draw.update(dt);
    // auto drawFut = draw.update(dt);
    // while(!drawFut.done())
    //     drawFut.next();
}

uint32_t Context::issueID(){
    return engine.issueID();
}

DeltaTime Context::getDeltaTime(){
    return engine.getDeltaTime();
}

template<>
const std::vector<ViewTask>& Context::getBuffer() const{
    return draw.viewTasks;
}
template<>
const std::vector<DrawTask>& Context::getBuffer() const{
    return draw.drawTasks;
}

DeltaTime Context::getDeltaTime() const{
    return engine.getDeltaTime();
}

