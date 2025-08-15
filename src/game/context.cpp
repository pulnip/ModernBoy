#include "engine/log.hpp"
#include "engine/engine.hpp"
#include "context.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Game;

Context::Context(Engine& engine)
:engine(engine),
draw(registry, engine.viewService, engine.drawService),
action(registry, engine.scriptInvoker),
camRay(registry, engine.inputService, rayService),
control(registry, playerIntent, editorIntent),
input(engine.inputService, engine.engineCommandBus,
    commandBus, registry, playerIntent, editorIntent),
physics(registry, rayService, engine.drawService),
lifespan(registry){}

void Context::update(DeltaTime dt){
    auto commands = commandBus.drainCommands();
    for(const auto& cmd: commands)
        std::visit([this](const auto& c){
            (*this)(c);
        }, cmd);

    input.update();
    camRay.update();

    control.update(dt);
    action.update(dt);
    // auto actionFut = action.update(dt);
    // while(!actionFut.done())
    //     actionFut.next();

    physics.update(dt);
    // auto physicsFut = physics.update(dt);
    // while(!physicsFut.done())
    //     physicsFut.next();

    draw.update();
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

DeltaTime Context::getDeltaTime() const{
    return engine.getDeltaTime();
}

void Context::operator()(ActivateSystem command){
    switch (command.targetSystem){
    case CAMERA_RAY:
        camRay.setActive(command.activate);
        break;
    default:
        GameWarn("Disabling Target System Not Implemented");
    }
}

