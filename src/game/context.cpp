#include "log.hpp"
#include "app_state.hpp"
#include "game/context.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Game;

Context::Context(AppState& app)
:app(app),
debug(registry), draw(registry),
action(registry, app.scriptInvoker),
input(registry, app.inputChord, app.renderer),
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
    return app.issueID();
}

DeltaTime Context::getDeltaTime(){
    return app.getDeltaTime();
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
    return app.getDeltaTime();
}

